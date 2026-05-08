# Oraculum

**Oraculum** is a lightweight C++ data recorder designed to collect, organize, and store structured data locally — safely, predictably, and without requiring a compiler on the target machine.

It is built for scenarios where you need a small portable executable that can run on another PC, create the required folders automatically, and append incoming data without overwriting existing files.

---

## What is Oraculum?

Oraculum is a local data-writing utility.

It can be used to record data such as:

- market data
- trading symbols
- API responses
- sensor output
- logs
- CSV/JSON records
- custom application events

The main idea is simple:

> Run the program, pass parameters, and let Oraculum save the data into clean, organized files.

---

## Features

- Portable C++ executable
- Works without installing a C++ compiler on the target machine
- Automatically creates folders if they do not exist
- Appends data instead of overwriting existing files
- Organizes files by symbol and data type
- Uses safe filesystem paths
- Suitable for Windows `.exe` builds
- Can be packaged for macOS as `.pkg`
- Simple command-line interface
- Designed for future CSV/JSON export support

---

## Example

```bash
oraculum.exe BTCUSDT trades
```

This can create or open a file like:

```text
Documents/Oraculum/BTCUSDT-trades
```

If the file already exists, Oraculum does not delete it.  
New data is appended to the end of the file.

---

## Why Oraculum?

Most small data tools require Python, dependencies, virtual environments, or manual setup.

Oraculum is different:

```text
compile once -> copy executable -> run anywhere
```

No compiler is needed on the second machine.

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
├── README.md
├── .gitignore
└── CMakeLists.txt
```

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

After building, the executable will be located inside the `build` directory.

---

## Windows Usage

```bash
oraculum.exe <symbol> <type>
```

Example:

```bash
oraculum.exe AAPL price
oraculum.exe BTCUSDT trades
oraculum.exe ETHUSDT candles
```

Arguments:

| Argument | Description |
|---|---|
| `symbol` | Name of the asset, source, or data group |
| `type` | Type of data being recorded |

---

## Example Output

If you run:

```bash
oraculum.exe BTCUSDT trades
```

Oraculum may create:

```text
C:\Users\User\Documents\Oraculum\BTCUSDT-trades
```

If the file already exists, Oraculum opens it in append mode.

That means existing data is preserved.

---

## File Writing Behavior

Oraculum uses append mode when opening files:

```cpp
std::ofstream file(path, std::ios::app);
```

This means:

| Situation | Result |
|---|---|
| File does not exist | File is created |
| File already exists | File is opened |
| File has old data | Old data is kept |
| New data is written | Added to the end |

---

## Folder Handling

Oraculum uses `std::filesystem` to work with paths safely.

Example:

```cpp
std::filesystem::create_directories(path);
```

This allows the program to create missing folders automatically.

If the folder already exists, it is not overwritten.

---

## Planned Features

- CSV export
- JSON export
- Config file support
- Custom output directory
- Logging system
- Windows installer
- macOS `.pkg` package
- Background recording mode
- Multiple symbols at once
- Timestamped records

---

## Example Future CLI

```bash
oraculum.exe --symbol BTCUSDT --type trades --format csv
```

```bash
oraculum.exe --symbol AAPL --type price --output "C:\Users\User\Documents\Data"
```

---

## Git Ignore Recommendation

The `build/` folder should not be pushed to GitHub.

Recommended `.gitignore`:

```gitignore
build/
cmake-build-debug/
cmake-build-release/

*.exe
*.o
*.obj
*.out

.vscode/
.idea/

logs/
data/
```

If `build/` was already pushed before, remove it from Git tracking:

```bash
git rm -r --cached build/
git add .gitignore
git commit -m "Ignore build folder"
git push
```

---

## Requirements

- C++17 or newer
- Windows, macOS, or Linux
- g++, clang++, MSVC, or CMake

---

## Build Target

Oraculum is designed to be compiled into a standalone executable:

```text
Windows -> oraculum.exe
macOS   -> oraculum.pkg
Linux   -> oraculum
```

The compiled program can be moved to another computer and run without installing a C++ compiler.

---

## Philosophy

Oraculum follows three simple rules:

1. Never overwrite user data accidentally.
2. Create the required structure automatically.
3. Keep the program portable and simple.

---

## Status

This project is currently in active development.

Core file-writing functionality is being implemented first.  
CLI improvements, packaging, and export formats will be added later.

---

## License

MIT License.

You are free to use, modify, and distribute this project.

---

## Author

Created by **Daniil Vedishchev**.

Oraculum — simple local data recording, made portable.
