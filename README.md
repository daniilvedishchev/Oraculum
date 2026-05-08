# Oraculum

Oraculum is a portable C++ market microstructure data recorder designed for quantitative research, order book analysis, and machine learning-based trading experiments.The project focuses on collecting high-frequency market data such as order book snapshots, trades, quotes, and derived features, then storing them locally in a clean append-only format for future backtesting and ML model training.
---

## Overview

In quantitative research, clean historical data is one of the most important assets.

Oraculum focuses on one core task:

> reliably record structured data streams into local storage without overwriting existing information.

The project is designed to be simple, fast, portable, and suitable for future expansion into a complete quant data toolkit.

---

## Use Cases

Oraculum can be used for:

- market data collection
- crypto tick data recording
- stock price logging
- trade and quote storage
- candle/OHLCV data collection
- backtesting dataset preparation
- local research data storage
- API response archiving
- trading bot data logging
- quantitative experiment tracking

---

## Core Idea

A typical quant workflow needs data like:

```text
symbol + data_type + timestamp + value
```

For example:

```text
BTCUSDT trades
AAPL price
ETHUSDT candles
SPY signals
```

Oraculum organizes this data into predictable local files.

Example command:

```bash
oraculum.exe BTCUSDT trades
```

Example output file:

```text
Documents/Oraculum/BTCUSDT-trades.csv
```

If the file already exists, Oraculum appends new data instead of overwriting old records.

---

## Features

- Written in modern C++
- Uses C++17 `std::filesystem`
- Portable executable design
- No compiler required on target machine
- Automatic folder creation
- Append-only file writing
- Safe path construction
- Symbol-based file organization
- Data-type based file naming
- Suitable for CSV/JSON storage
- Designed for quant research workflows
- Windows `.exe` support
- Future macOS `.pkg` packaging support

---

## Why Oraculum?

Many research tools rely on Python scripts, virtual environments, dependencies, and manual configuration.

Oraculum is designed to be different:

```text
compile once -> copy executable -> run anywhere -> collect data
```

This makes it useful for:

- running data collection on another machine
- creating small portable quant tools
- recording market data without installing a development environment
- building a local research database step by step

---

## Example

```bash
oraculum.exe BTCUSDT trades
```

Possible output:

```text
C:\Users\User\Documents\Oraculum\BTCUSDT-trades.csv
```

Another example:

```bash
oraculum.exe AAPL price
```

Possible output:

```text
C:\Users\User\Documents\Oraculum\AAPL-price.csv
```

---

## File Writing Behavior

Oraculum uses append mode for file output.

```cpp
std::ofstream file(path, std::ios::app);
```

This means:

| Situation | Behavior |
|---|---|
| File does not exist | Creates a new file |
| File already exists | Opens existing file |
| Existing data | Preserved |
| New data | Added to the end |
| Folder does not exist | Created automatically |

This is important for data collection because historical records should not be accidentally deleted.

---

## Quant Data Structure

A future record may look like this:

```csv
timestamp,symbol,type,price,volume
2026-01-01T12:00:00Z,BTCUSDT,trade,42150.25,0.018
```

Or for OHLCV data:

```csv
timestamp,symbol,open,high,low,close,volume
2026-01-01T12:00:00Z,BTCUSDT,42100.00,42200.00,42050.00,42150.25,182.45
```

The goal is to make every output file easy to use later in:

- backtesting engines
- Python notebooks
- pandas
- statistical models
- trading strategy research
- machine learning pipelines

---

## Project Structure

```text
oraculum/
├── src/
│   ├── main.cpp
│   ├── writer.cpp
│   └── writer.hpp
├── include/
├── build/
├── data/
├── logs/
├── README.md
├── .gitignore
└── CMakeLists.txt
```

Recommended ignored folders:

```text
build/
data/
logs/
```

The source code should be tracked by Git.  
Generated data, logs, and build artifacts should not be pushed.

---

## Build

### Using g++

```bash
g++ src/main.cpp src/writer.cpp -o oraculum.exe -std=c++17
```

### Using CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The compiled executable will be generated inside the build directory.

---

## Windows Usage

```bash
oraculum.exe <symbol> <type>
```

Examples:

```bash
oraculum.exe BTCUSDT trades
oraculum.exe ETHUSDT candles
oraculum.exe AAPL price
oraculum.exe SPY signals
```

Arguments:

| Argument | Description |
|---|---|
| `symbol` | Trading symbol, asset name, or data source |
| `type` | Data category such as trades, candles, price, signals, logs |

---

## Example Workflow

1. Build the executable:

```bash
g++ src/main.cpp src/writer.cpp -o oraculum.exe -std=c++17
```

2. Copy `oraculum.exe` to another Windows machine.

3. Run:

```bash
oraculum.exe BTCUSDT trades
```

4. Oraculum creates the required folder structure.

5. New records are appended to the correct data file.

---

## Planned CLI

Future versions may support named arguments:

```bash
oraculum.exe --symbol BTCUSDT --type trades --format csv
```

```bash
oraculum.exe --symbol AAPL --type price --output "C:\Users\User\Documents\QuantData"
```

```bash
oraculum.exe --symbol ETHUSDT --type candles --interval 1m
```

Possible future options:

| Option | Description |
|---|---|
| `--symbol` | Market symbol |
| `--type` | Data type |
| `--format` | Output format: csv/json/txt |
| `--output` | Custom output folder |
| `--interval` | Recording interval |
| `--exchange` | Exchange or data provider |
| `--append` | Append mode |
| `--log` | Enable logs |

---

## Planned Features

- CSV writer
- JSON writer
- Config file support
- Custom output directory
- Timestamped records
- Data validation
- Logging system
- Error reporting
- Multiple symbols support
- Exchange/source field
- OHLCV format support
- Trade data format support
- Signal recording
- Windows release build
- macOS `.pkg` installer
- Background recording mode
- Basic data integrity checks

---

## Example Future Data Layout

```text
Documents/
└── Oraculum/
    ├── BTCUSDT/
    │   ├── trades.csv
    │   ├── candles.csv
    │   └── signals.csv
    ├── ETHUSDT/
    │   ├── trades.csv
    │   └── candles.csv
    └── logs/
        └── oraculum.log
```

This layout is useful for research because each symbol and data type can be separated cleanly.

---

## Design Principles

Oraculum follows several important principles:

### 1. Data safety

Existing files should not be overwritten accidentally.

### 2. Simple storage

Data should be saved in formats that are easy to inspect and reuse.

### 3. Portability

The compiled program should run on another machine without requiring a compiler.

### 4. Predictable paths

Files should be stored in clear, consistent locations.

### 5. Quant-first structure

The project is designed around symbols, data types, timestamps, and records.

---

## `.gitignore`

Recommended `.gitignore`:

```gitignore
build/
cmake-build-debug/
cmake-build-release/

data/
logs/

*.exe
*.o
*.obj
*.out

.vscode/
.idea/
.DS_Store
```

If a folder was already pushed to GitHub, remove it from Git tracking:

```bash
git rm -r --cached build/
git rm -r --cached data/
git rm -r --cached logs/

git add .gitignore
git commit -m "Ignore generated folders"
git push
```

This keeps the folders locally but removes them from the repository.

---

## Requirements

- C++17 or newer
- `std::filesystem`
- g++, clang++, MSVC, or CMake
- Windows, macOS, or Linux

---

## Build Targets

```text
Windows -> oraculum.exe
macOS   -> oraculum.pkg
Linux   -> oraculum
```

The goal is to make Oraculum easy to distribute as a compiled executable.

---

## Roadmap

### Phase 1 — Core Writer

- create folders
- create files
- append data
- accept command-line parameters

### Phase 2 — Quant Data Format

- CSV structure
- timestamps
- symbol/type fields
- clean file naming

### Phase 3 — Config System

- config file
- custom output directory
- default symbols
- default data types

### Phase 4 — Data Pipeline

- API integration
- exchange/source support
- scheduled recording
- logs and error handling

### Phase 5 — Distribution

- Windows `.exe`
- macOS `.pkg`
- release builds
- installation guide

---

## Long-Term Vision

Oraculum is intended to become a small but reliable local quant data engine.

Possible future direction:

```text
data collection -> local storage -> research dataset -> backtesting -> strategy analysis
```

The project starts with safe local writing, but the architecture can grow into a complete market data recording system.

---

## Status

Oraculum is currently in early development.

The first goal is to build a stable local file writer that can safely create folders and append records without data loss.

---

## License

MIT License.

You are free to use, modify, and distribute this project.

---

## Author

Created by **Daniil Vedishchev**.

**Oraculum** — portable market data recording for quantitative research.
