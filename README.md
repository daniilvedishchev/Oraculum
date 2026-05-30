# Oraculum

> **Portable C++ market microstructure recorder for quantitative research, order book analysis, and ML-based trading experiments.**

Oraculum connects to live exchange WebSocket feeds, reconstructs a local order book in real time, computes market microstructure features, and persists everything to disk in a clean append-only format — ready for backtesting, statistical modelling, or training alpha-generating ML models.

---

## Table of Contents

- [Why Oraculum](#why-oraculum)
- [Architecture](#architecture)
- [Modules](#modules)
- [Quant Data Model](#quant-data-model)
- [Order Book Reconstruction](#order-book-reconstruction)
- [Feature Engine](#feature-engine)
- [Data Pipeline](#data-pipeline)
- [Build](#build)
- [Usage](#usage)
- [Output Format](#output-format)
- [Roadmap](#roadmap)
- [Design Principles](#design-principles)

---

## Why Oraculum

Proprietary tick data from vendors (Refinitiv, Bloomberg, Kaiko) costs thousands of dollars per year. Cloud-hosted data pipelines add latency, operational complexity, and vendor lock-in.

Oraculum is the alternative: compile once, run anywhere, collect everything locally.

```
exchange WebSocket feed
        │
        ▼
  Oraculum process
        │
   ┌────┴─────────────┐
   │  LocalOrderBook  │  ← full depth reconstruction
   │  FeatureEngine   │  ← derived microstructure signals
   │  CacheService    │  ← in-memory ring buffer
   │  FileManager     │  ← append-only disk persistence
   └──────────────────┘
        │
        ▼
  symbol/data-type.csv
  (ready for pandas, backtrader, PyTorch)
```

---

## Architecture

```
oraculum/
├── src/app/
│   ├── main/oraculum.cpp          ← entry point & orchestration
│   └── validation/validator.cpp   ← input & schema validation
│
├── socket/
│   ├── oraculumSocket/            ← IXWebSocket wrapper, reconnect logic
│   └── callbacks/callback.cpp     ← raw message dispatch
│
├── orderbook/
│   ├── constructor/               ← snapshot + delta application
│   ├── localbook/LocalOrderBook   ← bid/ask price-level maps
│   └── features/FeatureEngine     ← microstructure feature computation
│
├── datasrc/
│   ├── providers/providers.cpp    ← exchange-specific feed configs
│   └── resolvers/providerResolver ← symbol → feed URL resolution
│
├── cacheservice/CacheService      ← in-memory buffer before flush
├── filemanager/                   ← file creation, path logic, rotation
├── cli/cli.cpp                    ← argument parsing
├── config/                        ← exchange/symbol config
├── keywords/                      ← data type constants
├── namespace/                     ← project-wide type aliases
├── utils/                         ← misc helpers
│
├── external/
│   ├── IXWebSocket                ← WebSocket client (TLS via OpenSSL)
│   ├── cpr                        ← HTTP client (REST snapshots)
│   └── json                       ← nlohmann/json
│
└── CMakeLists.txt
```

---

## Modules

### `socket/` — WebSocket Layer

Wraps [IXWebSocket](https://github.com/machinezone/IXWebSocket) with automatic reconnection, TLS support (OpenSSL on macOS/Linux, native on Windows), and a callback dispatcher that routes raw JSON frames to the correct handler.

Key responsibilities:
- Establish and maintain the persistent WebSocket connection
- Parse the exchange protocol envelope (sequence numbers, event types)
- Forward order book events and trade events to downstream handlers

### `orderbook/` — Local Order Book

Maintains a full price-level order book in memory, applying real-time delta updates over an initial REST snapshot.

`LocalOrderBook` keeps separate sorted maps for bids (descending) and asks (ascending):

```
bids:  { 42150.00 → 1.824, 42149.50 → 3.100, ... }
asks:  { 42150.50 → 0.550, 42151.00 → 2.340, ... }
```

On each WebSocket delta:
1. Apply price-level additions, updates, and deletions
2. Recompute best bid / best ask
3. Trigger `FeatureEngine` to emit a new feature snapshot

### `orderbook/features/FeatureEngine` — Microstructure Features

Computes derived signals from the reconstructed book on every update tick. These are the bread and butter of market microstructure research and short-horizon alpha generation:

| Feature | Description |
|---|---|
| **Mid price** | `(best_bid + best_ask) / 2` |
| **Spread** | `best_ask − best_bid` (in ticks and bps) |
| **Book imbalance** | `(bid_qty − ask_qty) / (bid_qty + ask_qty)` at N levels |
| **Weighted mid** | Volume-weighted average of top N bid and ask levels |
| **Price impact** | Cost to walk N levels of the book |
| **Depth ratio** | Ratio of cumulative bid depth to ask depth |
| **Quote intensity** | Rate of book updates per second |
| **Trade flow** | Signed volume (buy-initiated vs sell-initiated trades) |

These features feed directly into statistical models (linear regression, logistic classification) or ML pipelines (gradient boosting, LSTM, reinforcement learning).

### `datasrc/` — Data Source Providers

Abstracts exchange-specific connection logic behind a common interface. The `providerResolver` maps a symbol string to the correct WebSocket endpoint, REST snapshot URL, and message schema.

Adding a new exchange requires implementing the provider interface — the rest of the pipeline is exchange-agnostic.

### `cacheservice/CacheService` — In-Memory Buffer

A ring buffer that accumulates feature snapshots before flushing to disk. This decouples the high-frequency write path from slower filesystem I/O, preventing the recording loop from blocking on disk latency.

### `filemanager/` — Persistent Storage

Handles append-only file writes. Files are organized by symbol and data type:

```
Documents/Oraculum/
├── BTCUSDT/
│   ├── trades.csv
│   ├── book_snapshots.csv
│   └── features.csv
├── ETHUSDT/
│   └── trades.csv
└── logs/
    └── oraculum.log
```

`std::ios::app` is used exclusively — historical data is never overwritten.

---

## Quant Data Model

### Trade Record

```csv
timestamp,symbol,side,price,qty,trade_id
2026-01-15T09:31:00.123Z,BTCUSDT,buy,42150.25,0.018,1748291023
```

### Order Book Snapshot

```csv
timestamp,symbol,bid_price_1,bid_qty_1,ask_price_1,ask_qty_1,...,bid_price_10,bid_qty_10,ask_price_10,ask_qty_10
2026-01-15T09:31:00.123Z,BTCUSDT,42150.00,1.824,42150.50,0.550,...
```

### Feature Snapshot

```csv
timestamp,symbol,mid,spread_bps,imbalance_1,imbalance_5,weighted_mid,depth_ratio,trade_flow_1s
2026-01-15T09:31:00.123Z,BTCUSDT,42150.25,0.24,0.312,-0.051,42150.18,1.14,−0.003
```

All timestamps are ISO 8601 UTC with millisecond precision. Every file is ready to load with `pd.read_csv(..., parse_dates=['timestamp'])`.

---

## Order Book Reconstruction

Oraculum uses the standard snapshot + delta reconstruction pattern:

```
1. REST GET /api/v3/depth?symbol=BTCUSDT&limit=1000
   → initialise LocalOrderBook with full snapshot

2. WebSocket stream: depthUpdate events
   → apply each delta in sequence number order
   → skip deltas with sequence < snapshot sequence
   → raise gap error if a sequence number is skipped
```

This is the same approach used by professional data vendors and HFT firms to maintain accurate books from public feeds.

---

## Feature Engine

The `FeatureEngine` is designed to be extended. To add a new feature:

```cpp
// In FeatureEngine.cpp
double myFeature(const LocalOrderBook& book, int levels) {
    // compute from book.bids() and book.asks()
    return result;
}
```

Computed features are appended to the current `FeatureSnapshot` struct and flushed through the `CacheService` to disk.

This architecture makes Oraculum suitable as a **feature store** for online and offline ML training. A Python notebook can simply:

```python
import pandas as pd

features = pd.read_csv("~/Documents/Oraculum/BTCUSDT/features.csv", parse_dates=["timestamp"])
features.set_index("timestamp", inplace=True)

# label: next-tick return
features["target"] = features["mid"].pct_change().shift(-1)

# train a classifier on microstructure features
from sklearn.ensemble import GradientBoostingClassifier
# ...
```

---

## Build

**Requirements:** C++17, CMake ≥ 3.20, OpenSSL (macOS: via Homebrew; Linux: `libssl-dev`; Windows: system native)

```bash
# Clone with submodules (IXWebSocket, cpr, nlohmann_json are external submodules)
git clone --recurse-submodules https://github.com/daniilvedishchev/Oraculum.git
cd Oraculum

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Executable
./build/ORACULUM          # Linux / macOS
.\build\ORACULUM.exe      # Windows
```

### macOS (Apple Silicon)

OpenSSL is detected automatically from `/opt/homebrew/opt/openssl@3`. If not found, install it:

```bash
brew install openssl@3
```

### Linux

```bash
sudo apt install libssl-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## Usage

```
ORACULUM <symbol> <data_type> [options]
```

```bash
# Record BTC/USDT trades and order book
./ORACULUM BTCUSDT trades
./ORACULUM BTCUSDT book_snapshots

# Record derived microstructure features
./ORACULUM ETHUSDT features

# Equities (when provider is configured)
./ORACULUM AAPL trades
./ORACULUM SPY signals
```

### Planned named arguments (Phase 2 CLI)

```bash
ORACULUM --symbol BTCUSDT --type features --depth 10 --interval 100ms
ORACULUM --symbol ETHUSDT --type trades --output ~/quant_data/
ORACULUM --symbol BTCUSDT --type book_snapshots --exchange binance --format csv
```

| Flag | Description | Default |
|---|---|---|
| `--symbol` | Instrument identifier | required |
| `--type` | `trades`, `book_snapshots`, `features`, `candles` | required |
| `--depth` | Order book depth levels to record | `10` |
| `--interval` | Feature snapshot interval (ms) | `100` |
| `--output` | Output directory | `~/Documents/Oraculum/` |
| `--exchange` | Data provider | auto-detected from symbol |
| `--format` | `csv` or `json` | `csv` |

---

## Output Format

### File naming

```
{output_dir}/{SYMBOL}/{data_type}.csv
```

### Append-only guarantee

```cpp
std::ofstream file(path, std::ios::app);
```

Historical data is never modified. If the process is killed and restarted, recording resumes from the next event. This makes Oraculum safe to run as a long-running background process or `systemd` service.

### `.gitignore` for data directories

```gitignore
build/
cmake-build-debug/
cmake-build-release/

data/
logs/

*.exe
*.o
*.obj
.DS_Store
.idea/
.vscode/
```

---

## Roadmap

### Phase 1 — Core Recorder ✅
- WebSocket connection with reconnect logic
- LocalOrderBook (snapshot + delta reconstruction)
- Append-only CSV persistence
- CLI (positional arguments)

### Phase 2 — Feature Store
- Full FeatureEngine (spread, imbalance, weighted mid, depth ratio, trade flow)
- Named CLI arguments
- Config file support (YAML/TOML)
- Custom output directory

### Phase 3 — Multi-Symbol & Exchanges
- Parallel recording of multiple symbols
- Exchange-agnostic provider interface (Binance, Coinbase, Kraken, Bybit)
- Equities feed support

### Phase 4 — Research Utilities
- Data integrity validation (gap detection, sequence checks)
- Compression (gzip rotation for long-running sessions)
- Python bindings for live feature access
- Real-time feature streaming over local socket

### Phase 5 — Distribution
- Windows `.exe` release build
- macOS `.pkg` installer
- Docker image for server-side recording
- GitHub Actions CI

---

## Design Principles

**Data safety above all.** Existing records are never overwritten. The append-only model means a crash or restart can never corrupt historical data.

**Exchange-agnostic core.** The order book reconstruction and feature engine are fully decoupled from exchange protocols. Adding Coinbase or Kraken support means writing a new provider, not modifying the core.

**Quant-first data layout.** Every output file is organised around the (symbol, data_type, timestamp) triple. Files load cleanly into pandas, polars, or any CSV-aware tool without pre-processing.

**Portable binary.** Compile on one machine, copy the executable, run anywhere. No Python environment, no runtime dependencies, no configuration beyond the command line.

**Latency-aware I/O.** The `CacheService` buffer decouples the hot recording path from disk writes. The WebSocket callback never blocks on filesystem I/O.

---

## Requirements

| Dependency | Version | Purpose |
|---|---|---|
| C++ standard | C++17 | `std::filesystem`, structured bindings |
| CMake | ≥ 3.20 | Build system |
| IXWebSocket | submodule | WebSocket client with TLS |
| cpr | submodule | HTTP client for REST snapshots |
| nlohmann/json | submodule | JSON parsing |
| OpenSSL | system | TLS for secure WebSocket connections |

---

## License

MIT License — free to use, modify, and distribute.

---

## Author

Built by **Daniil Vedishchev**.

**Oraculum** — market microstructure data, recorded locally, ready for research.
