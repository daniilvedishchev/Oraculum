# Oraculum

> **C++17 market microstructure engine for live order book reconstruction, feature engineering, and quantitative trading research.**

Oraculum is a low-level data engine built for collecting high-frequency crypto market data directly from exchange feeds. It connects to Binance live depth streams, synchronizes them with REST snapshots, reconstructs a local limit order book, and writes both raw market data and engineered microstructure features to disk.

The goal of this project is not only to record data, but to build the kind of infrastructure used before any serious quant trading model: clean market data ingestion, order book state reconstruction, latency tracking, tick-normalized prices, and feature generation for short-horizon alpha research.

---

## Table of Contents

- [Why this project matters](#why-this-project-matters)
- [What Oraculum does](#what-oraculum-does)
- [Current implementation](#current-implementation)
- [Architecture](#architecture)
- [Market data pipeline](#market-data-pipeline)
- [Order book reconstruction](#order-book-reconstruction)
- [Quant feature engineering](#quant-feature-engineering)
- [Output data](#output-data)
- [Build from source](#build-from-source)
- [Usage](#usage)
- [Example research workflow](#example-research-workflow)
- [Roadmap](#roadmap)
- [Skills demonstrated](#skills-demonstrated)
- [Author](#author)

---

## Why this project matters

A quantitative trading strategy is only as good as the data pipeline behind it.

For short-horizon trading, especially market making, statistical arbitrage, and order book prediction, raw OHLCV candles are often not enough. The useful information is inside the limit order book:

- how much liquidity is available at the bid and ask,
- how quickly the book changes,
- whether the bid side or ask side dominates,
- how wide the spread is,
- how far the microprice is from the mid price,
- whether depth pressure predicts the next price move.

Oraculum is designed around this idea. It transforms live exchange messages into a structured dataset that can later be used for:

- next-tick / next-second return prediction,
- spread and liquidity modelling,
- order flow imbalance research,
- short-term alpha generation,
- feature stores for machine learning models,
- backtesting of market making or execution logic.

---

## What Oraculum does

Oraculum currently focuses on **Binance spot order book data**.

At a high level, it performs the following steps:

```text
Binance REST snapshot + Binance WebSocket depth updates
                         │
                         ▼
              Snapshot / delta synchronization
                         │
                         ▼
              LocalOrderBook reconstruction
                         │
                         ▼
              FeatureEngine computation
                         │
                         ▼
        Raw updates, snapshots, and features saved locally
```

The project is written in modern C++ because market data systems benefit from:

- low latency,
- deterministic memory management,
- explicit control over threading and buffers,
- portable compiled binaries,
- minimal runtime dependencies compared with Python-only collectors.

---

## Current implementation

Implemented in the current version:

- Binance provider support
- Binance symbol validation through `exchangeInfo`
- REST snapshot download
- WebSocket depth stream connection
- snapshot + delta order book reconstruction
- gap detection and re-synchronization logic
- local bid/ask price-level maps
- tick-size and step-size normalization
- raw snapshot persistence
- raw update persistence
- engineered feature persistence
- command-line interface
- CMake-based C++17 build

Current focus:

- `depth` streams
- Binance spot symbols such as `BTCUSDT`, `ETHUSDT`, etc.
- feature generation from reconstructed order book state

This project is research infrastructure. It does **not** place trades, manage orders, or connect to a broker execution API.

---

## Architecture

```text
Oraculum/
├── src/app/main/                  # Application entry point and orchestration
├── src/app/validation/            # CLI and market-data configuration validation
│
├── cli/                           # Command-line argument parser
├── config/                        # Runtime configuration structure
├── keywords/                      # CLI keywords and stream-type constants
│
├── datasrc/                       # Exchange/provider abstraction
│   ├── providers/                 # Supported data providers
│   ├── resolvers/                 # Provider and URL resolution
│   ├── endpoints/                 # REST/WebSocket endpoint builders
│   └── binance/                   # Binance-specific stream configuration
│
├── socket/                        # WebSocket layer
│   ├── oraculumSocket/            # IXWebSocket wrapper
│   ├── callbacks/                 # WebSocket message callbacks
│   └── ring/                      # Thread-safe ring buffer for updates
│
├── orderbook/                     # Market microstructure core
│   ├── constructor/               # Snapshot + delta synchronization
│   ├── localbook/                 # Local bid/ask book representation
│   ├── depth/                     # Depth update structure
│   └── features/                  # Quantitative feature engine
│
├── cacheservice/                  # Symbol metadata cache: tick size / step size
├── filemanager/                   # File creation and append-only persistence
├── connector/                     # URL builder and retry logic
├── infrastructure/market/         # Market interval definitions
├── utils/                         # String utilities
│
├── external/                      # Third-party dependencies
│   ├── IXWebSocket
│   ├── cpr
│   └── nlohmann/json
│
└── CMakeLists.txt
```

---

## Market data pipeline

Oraculum uses two Binance data sources:

1. **REST API snapshot**  
   Used to initialize the local order book with a consistent state.

2. **WebSocket depth updates**  
   Used to apply live incremental changes to the local book.

The application first opens the depth WebSocket stream and waits for the first update. Then it downloads a REST snapshot and synchronizes the stream against the snapshot update ID.

This approach follows the standard pattern used when reconstructing an exchange order book from public market data feeds.

---

## Order book reconstruction

The local book stores price levels in integer tick units instead of raw floating-point prices.

```cpp
std::map<int64_t, int64_t, std::greater<>> bids;
std::map<int64_t, int64_t, std::less<>> asks;
```

This is important for market data work because floating-point arithmetic can introduce precision errors when comparing prices or quantities.

Example:

```text
raw price:  42150.25000000
price tick: 0.01000000
stored as:  4215025 ticks
```

The same principle is used for quantities via the exchange `stepSize`.

### Update logic

For every depth update:

1. Convert price and quantity strings into tick units.
2. If quantity is zero, remove the price level.
3. Otherwise, insert or update the level.
4. Update the last processed exchange update ID.
5. Recompute features from the new book state.
6. Write the raw update and feature row to disk.

The reconstruction loop also checks for sequence gaps. If an update is no longer compatible with the current local book state, Oraculum re-downloads a fresh snapshot and rebuilds the book.

---

## Quant feature engineering

This is the most important part of the project from a quantitative trading perspective.

The `FeatureEngine` converts the live reconstructed order book into a row of numerical features. These features are suitable for modelling short-term price movement, liquidity pressure, adverse selection, and book imbalance.

Generated feature schema:

```csv
ts_local_ms,ts_provider_ms,latency_ms,spread_ticks,best_ask,best_bid,mid,
microprice_l1,relative_microprice_bps,
imb_10,imb_20,imb_50,imb_100,imb_200,imb_500,imb_1000,
imb_1bps,imb_2bps,imb_5bps,imb_10bps,imb_20bps,imb_50bps,imb_100bps
```

### 1. Timestamp and latency features

| Feature | Meaning | Quant use |
|---|---|---|
| `ts_local_ms` | Local machine timestamp in milliseconds | Aligning features with local capture time |
| `ts_provider_ms` | Exchange event timestamp | Exchange-side event timing |
| `latency_ms` | `ts_local_ms - ts_provider_ms` | Feed quality control, latency filtering |

Latency is useful because stale updates can distort a high-frequency dataset. A model trained on delayed or inconsistent observations may learn noise instead of true order book dynamics.

---

### 2. Best bid, best ask, spread, and mid price

| Feature | Formula / definition | Quant use |
|---|---|---|
| `best_bid` | Highest bid price level | Top-of-book liquidity |
| `best_ask` | Lowest ask price level | Top-of-book liquidity |
| `spread_ticks` | `best_ask - best_bid` | Transaction cost / liquidity regime |
| `mid` | `(best_bid + best_ask) / 2` | Reference price for returns and labels |

The spread is one of the most important microstructure variables. A wider spread often means lower liquidity, higher uncertainty, and higher execution cost. A tighter spread usually indicates a more competitive and liquid market.

---

### 3. L1 liquidity-weighted price / microprice

The current feature row includes:

```text
microprice_l1
relative_microprice_bps
```

`microprice_l1` is computed from the best bid, best ask, and their displayed quantities. `relative_microprice_bps` measures how far this value is from the mid price in basis points:

```text
relative_microprice_bps = ((microprice_l1 - mid) / mid) * 10,000
```

Why it matters:

- If the liquidity-weighted price moves away from the mid, it can indicate pressure on one side of the book.
- If the bid side or ask side dominates, the next mid-price move may become asymmetric.
- This type of feature is commonly used in short-horizon price direction models.

---

### 4. N-level order book imbalance

For several depth windows, Oraculum computes cumulative bid/ask imbalance:

```text
imbalance_N = (sum_bid_qty_N - sum_ask_qty_N) / (sum_bid_qty_N + sum_ask_qty_N)
```

Implemented depth windows:

```text
10, 20, 50, 100, 200, 500, 1000 levels
```

Feature columns:

```text
imb_10, imb_20, imb_50, imb_100, imb_200, imb_500, imb_1000
```

Interpretation:

| Value | Interpretation |
|---|---|
| close to `+1` | bid-side depth dominates |
| close to `0` | book is balanced |
| close to `-1` | ask-side depth dominates |

Why this is useful for quant trading:

- Positive imbalance may suggest upward pressure if aggressive buyers consume the ask.
- Negative imbalance may suggest downward pressure if sellers dominate liquidity.
- Comparing imbalance across multiple depths helps distinguish shallow top-of-book noise from deeper liquidity structure.

For example, `imb_10` can react quickly to very local pressure, while `imb_500` is more stable and describes the broader book shape.

---

### 5. Basis-point price-band imbalance

Fixed level-depth features can behave differently across assets. Ten levels in one instrument may represent a very different price distance than ten levels in another.

To make features more comparable, Oraculum also computes imbalance inside price bands around the mid price:

```text
±1 bps, ±2 bps, ±5 bps, ±10 bps, ±20 bps, ±50 bps, ±100 bps
```

Feature columns:

```text
imb_1bps, imb_2bps, imb_5bps, imb_10bps, imb_20bps, imb_50bps, imb_100bps
```

This is especially useful for cross-symbol research because the feature is defined by a relative price distance, not a raw number of levels.

Example interpretation:

- `imb_1bps`: very near-touch pressure
- `imb_10bps`: short-range liquidity pressure
- `imb_100bps`: broader book-side dominance

This gives the model information about both immediate liquidity and deeper supply/demand zones.

---

## Output data

Oraculum writes data under the user home directory:

```text
~/Oraculum/
├── cache/
│   └── binance/
│       └── symbolsMeta
│
└── <symbol>/
    ├── orderbook/
    │   ├── snapshots/
    │   │   └── <symbol>-DEPTH-<depth>-snapshot-<lastUpdateId>.json
    │   └── updates/
    │       └── <symbol>-DEPTH-<depth>-update.json
    │
    └── features/
        └── features.csv
```

### Example feature row

```csv
ts_local_ms,ts_provider_ms,latency_ms,spread_ticks,best_ask,best_bid,mid,microprice_l1,relative_microprice_bps,imb_10,imb_20,imb_50,imb_100,imb_200,imb_500,imb_1000,imb_1bps,imb_2bps,imb_5bps,imb_10bps,imb_20bps,imb_50bps,imb_100bps
1717000000123,1717000000101,22,1,4215026,4215025,4215025.5,4215025.48,-0.00047,0.12,0.08,0.04,0.02,-0.01,-0.03,-0.05,0.18,0.11,0.07,0.02,-0.01,-0.04,-0.08
```

Prices and quantities are stored internally in tick units. This makes the system more robust for order book updates, price-level comparisons, and later statistical analysis.

---

## Build from source

### Requirements

- C++17 compiler
- CMake 3.20+
- OpenSSL
- Git submodules

Third-party dependencies are included as submodules:

- `IXWebSocket` for WebSocket connectivity
- `cpr` for REST requests
- `nlohmann/json` for JSON parsing

### Clone

```bash
git clone --recurse-submodules <your-repository-url>
cd Oraculum
```

If the repository was cloned without submodules:

```bash
git submodule update --init --recursive
```

### macOS

```bash
brew install openssl@3
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Linux

```bash
sudo apt-get update
sudo apt-get install -y cmake g++ libssl-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Run executable

```bash
./build/ORACULUM
```

Depending on your platform and build directory, the executable may also be available as:

```bash
./ORACULUM
```

from inside the `build/` folder.

---

## Usage

Current CLI syntax:

```bash
./ORACULUM -write-live-data -provider binance -symbol <symbol> -type depth -d <depth>
```

Example:

```bash
./ORACULUM -write-live-data -provider binance -symbol ethusdc -type depth -d 20
```

Because Binance is the default provider, this is also valid:

```bash
./ORACULUM -write-live-data -symbol ethusdc -type depth -d 20
```

Supported depth values:

```text
5, 10, 20, 50, 100, 500, 1000
```

Supported provider in the current implementation:

```text
binance
```

Supported recording mode in the current implementation:

```text
depth
```

---

## Example research workflow

After collecting data, the feature file can be loaded into Python for modelling.

```python
import pandas as pd

features = pd.read_csv("~/Oraculum/ethusdc/features/features.csv")

# Convert timestamps
features["ts_local"] = pd.to_datetime(features["ts_local_ms"], unit="ms")
features["ts_provider"] = pd.to_datetime(features["ts_provider_ms"], unit="ms")

# Example target: next feature-row mid-price direction
features["future_mid"] = features["mid"].shift(-1)
features["target_up"] = (features["future_mid"] > features["mid"]).astype(int)

# Example feature set
X = features[
    [
        "spread_ticks",
        "relative_microprice_bps",
        "imb_10",
        "imb_20",
        "imb_50",
        "imb_1bps",
        "imb_2bps",
        "imb_5bps",
        "imb_10bps",
        "latency_ms",
    ]
]

y = features["target_up"]
```

Possible models:

- logistic regression for interpretable directional prediction,
- random forest / gradient boosting for nonlinear feature interactions,
- XGBoost / LightGBM for tabular alpha modelling,
- sequence models for temporal order book dynamics,
- reinforcement learning environment for market making experiments.

Important research note: before using these features in a trading model, the dataset should be cleaned for missing updates, abnormal latency, duplicated rows, and exchange/network interruptions.

---

## Roadmap

### Data engineering

- Add compressed file rotation for long-running data collection
- Add configurable output directory
- Add structured logging
- Add data quality reports
- Add sequence-number diagnostics

### Market data

- Add trade stream support
- Add aggregated trade support
- Add kline/candle stream support
- Add multi-symbol recording
- Add more exchanges such as Coinbase, Kraken, Bybit, and OKX

### Quant research

- Add order flow imbalance features
- Add rolling update intensity
- Add realized volatility features
- Add spread regime classification
- Add short-horizon label generation
- Add Python notebook examples
- Add backtesting examples using generated features

### Production engineering

- Add unit tests
- Add CI with GitHub Actions
- Add Docker support
- Add benchmark scripts
- Add graceful shutdown and signal handling

---

## Skills demonstrated

This project demonstrates skills that are directly relevant to quantitative trading and market data engineering:

- C++17 systems programming
- WebSocket market data ingestion
- REST API integration
- exchange-specific data normalization
- local limit order book reconstruction
- market microstructure feature engineering
- tick-size / step-size precision handling
- multi-threaded producer-consumer architecture
- ring-buffer based event processing
- append-only data persistence
- CMake project organization
- data preparation for machine learning research

---

## Disclaimer

This project is for educational and research purposes only. It does not provide financial advice and does not execute trades. Any trading model built using this data should be properly validated with realistic transaction costs, latency assumptions, slippage, and out-of-sample testing.

---

## Author

Built by **Daniil Vedishchev**.

I am interested in quantitative trading, market microstructure, algorithmic trading systems, and machine learning for financial markets. Oraculum is part of my effort to build real trading infrastructure from the data layer upward, instead of only working with pre-cleaned datasets.
