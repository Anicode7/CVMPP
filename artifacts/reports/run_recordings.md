# CVM++ Run Recordings

## Note on Recording Format
This repository is a CLI program. In this environment, I captured terminal-session transcripts as reproducible run recordings.

## Build Recording
- Command:
```powershell
cmake -S . -B build
cmake --build build
```
- Full transcript: [`artifacts/logs/cmake_configure.log`](../logs/cmake_configure.log), [`artifacts/logs/cmake_build.log`](../logs/cmake_build.log)
- Result: Success (binary produced at `build/Debug/cvm.exe`)

## Successful Run Recordings

### 1) Math Sample
- Command:
```powershell
.\build\Debug\cvm.exe .\examples\math_test.cvm
```
- Transcript: [`artifacts/logs/run_success_math.log`](../logs/run_success_math.log)
- Observed output:
```text
0
```

### 2) If Sample (stdin = 7)
- Command:
```powershell
7 | .\build\Debug\cvm.exe .\examples\if_test.cvm
```
- Transcript: [`artifacts/logs/run_success_if.log`](../logs/run_success_if.log)
- Observed output:
```text
? 0
```

### 3) While/Loop Sample (stdin = 5)
- Command:
```powershell
5 | .\build\Debug\cvm.exe .\examples\loop_test.cvm
```
- Transcript: [`artifacts/logs/run_success_loop.log`](../logs/run_success_loop.log)
- Observed output:
```text
? 120
```

### 4) Instrumented Run (AST + Bytecode)
- Command:
```powershell
7 | .\build\Debug\cvm.exe --ast --bytecode .\examples\if_test.cvm
```
- Transcript: [`artifacts/logs/run_success_if_ast_bytecode.log`](../logs/run_success_if_ast_bytecode.log)
- Result: Success with AST and bytecode dump enabled.

## Unsuccessful Run Recording (Limitation Demonstration)

### Unsupported Comparison Operator (`>`)
- Program used: [`artifacts/limitation_unsupported_operator.cvm`](../limitation_unsupported_operator.cvm)
- Command:
```powershell
.\build\Debug\cvm.exe .\artifacts\limitation_unsupported_operator.cvm
```
- Clean transcript: [`artifacts/logs/run_failure_unsupported_operator_clean.log`](../logs/run_failure_unsupported_operator_clean.log)
- Observed error:
```text
Error: Unexpected character '>' at line 2
Error: Expect expression. Found '3'
```

### Limitation Summary
Current grammar/runtime supports `<` and `==`, but not `>`, `<=`, `>=`, or `!=`.
