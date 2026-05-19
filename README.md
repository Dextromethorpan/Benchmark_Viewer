# BenchmarkViewer

A Qt6 desktop application that displays memory architecture benchmark results in a terminal-style dashboard.

## What it does

Reads a `results.json` file produced by [Benchmark_Architectures](https://github.com/Dextromethorpan/Benchmark_Architectures) and renders a clean comparison table showing how two C++ architectures perform across four memory concepts:

- Timing Unpredictability
- Memory Churn
- Memory Fragmentation
- Concurrency Safety

Each metric shows a `[PASS]` or `[FAIL]` result in green or red, with a score bar at the top for each architecture.

## Requirements

- Windows 10 or later
- Visual Studio 2022
- CMake 3.20 or later
- Qt 6.x (tested with Qt 6.11.0)
- C++20

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:\Qt\6.11.0\msvc2022_64"
cmake --build . --config Release
```

Replace `6.11.0` with your installed Qt version if different.

## Run

```bash
set PATH=C:\Qt\6.11.0\msvc2022_64\bin;%PATH%
.\build\Release\BenchmarkViewer.exe
```

Or use the included batch file:

```bash
run_viewer.bat
```

## Load results

Once the viewer is open, either:
- Drag and drop `results.json` onto the window
- Click `[Open results.json]` in the top bar and browse to the file

`results.json` is produced by running `MemoryBenchmark.exe` from the Benchmark_Architectures project.

## Project structure

```
BenchmarkViewer/
  MainWindow.hpp     terminal-style Qt dashboard
  main.cpp           QApplication entry point
  CMakeLists.txt     build configuration
  run_viewer.bat     launch script with Qt DLL path
```

## Related project

[Benchmark_Architectures](https://github.com/Dextromethorpan/Benchmark_Architectures) — headless C++ benchmark that produces the `results.json` this viewer reads.
