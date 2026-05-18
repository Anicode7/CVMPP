# CVM++ Startup Guide

## Prerequisites
- CMake 3.10+
- Visual Studio Build Tools (MSVC)

## Build
```powershell
cmake -S . -B build
cmake --build build
```

## How To Write Code In Terminal
You have two simple ways:

1. Run from a `.cvm` file
```powershell
.\build\Debug\cvm.exe .\examples\successful\01_arithmetic_basic.cvm
```

2. Write directly in REPL (interactive terminal)
```powershell
.\build\Debug\cvm.exe
```
Then type CVM++ statements one line at a time (end each statement with `;`), for example:
```text
let x = 2;
if (x < 3) { print x; } else { print 0; }
exit
```
Use `exit` to leave the REPL.

## Visualiser (Major Pipeline Levels)
Visualizer file location:
- `cvm/visualiser/main.cpp`

Run with file input:
```powershell
.\build\Debug\cvm_visualiser.exe .\examples\successful\10_while_countdown_sum.cvm
```

Run with direct terminal input:
```powershell
.\build\Debug\cvm_visualiser.exe
```
Paste code, then type:
```text
END
```

It prints:
- Source
- Tokens
- AST
- Symbol table
- Bytecode (raw + disassembled)
- VM output

## Run With Debug Views (Main Runtime)
```powershell
7 | .\build\Debug\cvm.exe --ast --bytecode .\examples\if_test.cvm
```

## Known Limitations
- `>` / `<=` / `>=` / `!=` are not supported yet.
- String literals are not supported (for example `print("hello");`).
- Chained assignment like `a = b = 7;` currently causes a VM stack-underflow bug.

## Validation Artifacts
- Run recordings: `artifacts/reports/run_recordings.md`
- Analysis findings: `artifacts/reports/analysis_findings.md`
- Example suite results: `artifacts/logs/example_suite_results.csv`
