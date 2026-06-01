# Oraculum

**C++ real-time market microstructure recorder and feature engine for quantitative research.**

Oraculum connects to live exchange WebSocket feeds, reconstructs a full local order book in real time, computes a rich set of market microstructure features on every tick, and persists everything locally in an append-only format ready for backtesting, statistical modelling, and ML training.

96% C++ · C++17 · IXWebSocket · CPR · nlohmann/json · OpenSSL

---

## What it does

```
Exchange WebSocket feed
        │
        ▼
  oraculumSocket          ← persistent TLS connection, auto-reconnect
        │
        ▼
  callback dispatcher     ← routes depth deltas, trade events, quotes
        │
        ▼
  orderBookConstructor    ← REST snapshot + sequence-ordered delta application
        │
        ▼
  LocalOrderBook          ← live bid/ask price-level maps (full depth)
        │
        ▼
  FeatureEngine           ← microstructure features computed on every update
        │
        ▼
  CacheService            ← in-memory buffer, decouples hot path from I/O
        │
        ▼
  FileManager             ← append-only CSV, per-symbol, per-data-type
```

---

## Architecture

```
oraculum/
│
├── src/app/
│   ├── main/oraculum.cpp            ← orchestration, startup
│   └── validation/validator.cpp     ← input & config validation
│
├── socket/
│   ├── oraculumSocket/              ← IXWebSocket wrapper, TLS, reconnect
│   └── callbacks/callback.cpp       ← message dispatch
│
├── orderbook/
│   ├── constructor/                 ← snapshot + delta reconstruction
│   ├── localbook/LocalOrderBook     ← sorted bid/ask price-level maps
│   └── features/FeatureEngine       ← microstructure feature computation ★
│
├── connector/                       ← exchange connection lifecycle
├── infrastructure/market/           ← market data structures, types
├── task/                            ← async/scheduled task management
│
├── datasrc/
│   ├── providers/providers.cpp      ← exchange-specific feed configs
│   └── resolvers/providerResolver   ← symbol → endpoint resolution
│
├── cacheservice/CacheService        ← write buffer before disk flush
├── filemanager/                     ← file abstraction, path logic
├── cli/cli.cpp                      ← argument parsing
├── config/                          ← exchange and symbol configuration
├── keywords/                        ← data type constants
├── namespace/                       ← project-wide type aliases
├── utils/                           ← helpers
│
└── external/
    ├── IXWebSocket                  ← WebSocket client (OpenSSL TLS)
    ├── cpr                          ← HTTP client for REST snapshots
    └── json                         ← nlohmann/json
```

---

## Feature Engine ★

The `FeatureEngine` is the quantitative core of Oraculum. It runs on every order book update and computes a snapshot of microstructure signals derived from the live reconstructed book.

These features are the direct inputs to short-horizon alpha models, execution cost estimators, and ML classifiers trained on market microstructure.

### Computed features

#### Price & spread

| Feature | Formula | Quant relevance |
|---|---|---|
| **Mid price** | `(best_bid + best_ask) / 2` | Baseline reference price; used in nearly every microstructure model |
| **Spread (absolute)** | `best_ask − best_bid` | Direct measure of liquidity cost; mean-reverts around typical values |
| **Spread (bps)** | `spread / mid × 10000` | Normalised cost; comparable across instruments and price regimes |
| **Weighted mid** | Volume-weighted avg of top N levels | More robust mid estimate; less sensitive to thin top-of-book |

#### Order book imbalance

| Feature | Formula | Quant relevance |
|---|---|---|
| **Imbalance (L1)** | `(bid_qty₁ − ask_qty₁) / (bid_qty₁ + ask_qty₁)` | Strong short-horizon price direction signal; well-documented in the literature |
| **Imbalance (L5)** | Same formula over top 5 levels | Smoother signal; captures deeper queue pressure |
| **Imbalance (L10)** | Same formula over top 10 levels | Useful for detecting large hidden order pressure |
| **Depth ratio** | `cum_bid_depth / cum_ask_depth` (N levels) | Directional imbalance in total resting liquidity |

#### Price impact & depth

| Feature | Formula | Quant relevance |
|---|---|---|
| **Walk cost (buy)** | Cost to consume N levels on the ask | Instantaneous market impact estimate; execution cost input |
| **Walk cost (sell)** | Cost to consume N levels on the bid | Same on the bid side |
| **Bid depth (N levels)** | Cumulative qty resting on bids | Absolute liquidity supply measure |
| **Ask depth (N levels)** | Cumulative qty resting on asks | Absolute liquidity supply measure |

#### Flow & activity

| Feature | Formula | Quant relevance |
|---|---|---|
| **Trade flow (signed)** | `Σ buy_qty − Σ sell_qty` over rolling window | Order flow imbalance; core input to Kyle (1985) and Glosten-Milgrom models |
| **Quote intensity** | Book update rate (events/sec) | Proxy for informed trading activity; spikes precede adverse selection |
| **Mid price velocity** | `Δmid / Δt` | Short-term price momentum signal |

### Why these features matter

Book imbalance at L1 is one of the most studied predictors of short-term price movement. Stoikov & Avellaneda (2008), Cont, Kukanov & Stoikov (2014), and dozens of subsequent papers show that `(bid_qty − ask_qty) / (bid_qty + ask_qty)` at the top of the book carries statistically significant predictive power for next-tick returns in the 100ms–10s horizon. Oraculum records this at full tick resolution.

Signed trade flow is the empirical proxy for Kyle's lambda — the market impact coefficient. Accumulating it over a rolling window gives a real-time estimate of whether informed buying or selling pressure is building.

Walk cost is the correct measure of execution cost in a limit order book, and is an essential input for any execution algorithm or transaction cost analysis (TCA) model.

---

## Order Book Reconstruction

Oraculum uses the standard snapshot + delta pattern:

```
1. REST GET /depth?symbol=X&limit=1000
   → initialise LocalOrderBook from full snapshot

2. WebSocket depth stream
   → apply deltas in sequence number order
   → skip events with U ≤ lastUpdateId from snapshot
   → detect and handle sequence gaps (reconnect + re-snapshot)
```

`LocalOrderBook` maintains:
```cpp
std::map<double, double, std::greater<>>  bids;  // price → qty, descending
std::map<double, double>                  asks;  // price → qty, ascending
```

On each delta, price levels with qty = 0 are removed. Best bid and best ask are always `bids.begin()` and `asks.begin()` — O(log n) lookup, O(1) for best price after update.

---

## Data Output

Files are written by `FileManager` using `std::ios::app` — historical data is never overwritten.

```
~/Documents/Oraculum/
├── BTCUSDT/
│   ├── trades.csv
│   ├── book_snapshots.csv
│   └── features.csv
├── ETHUSDT/
│   └── features.csv
└── logs/
    └── oraculum.log
```

### Feature snapshot record

```csv
timestamp,symbol,mid,spread_abs,spread_bps,imbalance_l1,imbalance_l5,imbalance_l10,
weighted_mid,depth_ratio,walk_cost_buy,walk_cost_sell,bid_depth,ask_depth,
trade_flow,quote_intensity,mid_velocity

2026-01-15T09:31:00.123Z,BTCUSDT,42150.25,0.50,0.24,0.312,-0.051,0.008,
42150.18,1.14,0.032,0.028,18.42,16.10,-0.003,47.2,0.12
```

All timestamps are ISO 8601 UTC with millisecond precision. Load instantly with:

```python
import pandas as pd

df = pd.read_csv("~/Documents/Oraculum/BTCUSDT/features.csv",
                 parse_dates=["timestamp"],
                 index_col="timestamp")

# next-tick return label
df["target"] = df["mid"].pct_change().shift(-1).apply(lambda x: 1 if x > 0 else -1)

# train on microstructure features
features = ["imbalance_l1", "imbalance_l5", "depth_ratio",
            "spread_bps", "trade_flow", "quote_intensity"]
X = df[features].dropna()
y = df["target"].reindex(X.index)
```

---

## Build

**Requirements:** C++17, CMake ≥ 3.20, OpenSSL

```bash
git clone --recurse-submodules https://github.com/daniilvedishchev/Oraculum.git
cd Oraculum

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### macOS (Apple Silicon / Intel)

OpenSSL is detected automatically. If missing:

```bash
brew install openssl@3
```

### Linux

```bash
sudo apt install libssl-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Windows

MSVC or MinGW with CMake. OpenSSL detected from system path.

---

## Usage

```
ORACULUM <symbol> <data_type>
```

```bash
./ORACULUM BTCUSDT features        # record microstructure feature snapshots
./ORACULUM BTCUSDT trades          # record raw trade stream
./ORACULUM BTCUSDT book_snapshots  # record order book depth snapshots
./ORACULUM ETHUSDT features
```

---

## Dependencies

| Library | Role |
|---|---|
| [IXWebSocket](https://github.com/machinezone/IXWebSocket) | WebSocket client with TLS, reconnect, ping/pong |
| [cpr](https://github.com/libcpr/cpr) | HTTP client for REST snapshot requests |
| [nlohmann/json](https://github.com/nlohmann/json) | JSON parsing of exchange messages |
| OpenSSL | TLS for secure WebSocket connections |

All three are Git submodules — no separate install needed.

---

## Roadmap

**Done**
- WebSocket connection with reconnect
- REST snapshot + delta order book reconstruction
- LocalOrderBook (full depth, sorted maps)
- FeatureEngine (spread, imbalance L1/L5/L10, weighted mid, depth ratio, walk cost, trade flow, quote intensity, mid velocity)
- CacheService (buffered I/O)
- Append-only CSV persistence
- CLI
- Cross-platform CMake (macOS M1/Intel, Linux, Windows)

**Next**
- Named CLI flags (`--symbol`, `--depth`, `--interval`, `--output`)
- Multi-symbol parallel recording
- Additional exchange providers (Coinbase, Kraken, Bybit)
- Config file (YAML)
- Data integrity validation (gap detection, sequence checks on replay)
- Compression / file rotation for long sessions
- Python bindings for live feature streaming

---

## Design Principles

**Data safety first.** `std::ios::app` only — existing records are never modified. A crash or restart resumes recording from the next tick.

**Quant-first data layout.** Every output file maps directly to `(symbol, data_type, timestamp)`. No preprocessing required before loading into pandas, polars, or a backtesting engine.

**Exchange-agnostic core.** The order book reconstruction and feature engine are fully decoupled from exchange wire protocols. Adding a new venue means writing a provider — not touching `LocalOrderBook` or `FeatureEngine`.

**Latency-aware I/O.** `CacheService` decouples the hot recording path from disk writes. WebSocket callbacks never block on filesystem I/O.

**Portable binary.** Compile once, copy the executable, run anywhere. No Python environment, no runtime dependencies.

---

## License

MIT — free to use, modify, and distribute.

---

## Author

Built by **Daniil Vedishchev**.

**Oraculum** — market microstructure data, recorded locally, ready for research.
