# Current Status

Snapshot date: June 5, 2026

## Project Identity

CVM++ is no longer just a minimal arithmetic-and-control-flow toy language. The repository now contains a full educational pipeline:

`Lexer -> Parser -> AST -> Bytecode Compiler -> Stack VM`

It also has a separate visualizer binary, a stateful REPL, curated example programs, and working support for user-defined functions with recursion.

## What Works Today

### Core pipeline

- Lexing of keywords, identifiers, numbers, punctuation, and the currently supported operators.
- Recursive-descent parsing into an explicit AST.
- Bytecode generation with 16-bit operands for constants, variable slots, and jump targets.
- Stack-based VM execution with control flow, variables, and function calls.

### Language features

- Integer literals and unary minus.
- Boolean literals: `true`, `false`.
- Arithmetic: `+`, `-`, `*`, `/`.
- Comparisons: `<`, `==`.
- Variable declaration and assignment with `let`.
- Chained assignment now works in practice.
- `if / else` and `while`.
- `print` and `input`.
- Function declaration via `fn`.
- Function calls with arguments.
- `return` with explicit value or implicit zero.
- Recursion.
- Local scope inside function execution, while retaining global access where intended.

### Tooling and DX

- Runtime executable: `cvm`.
- Visualizer executable: `cvm_visualiser`.
- AST and bytecode debug flags in the runtime.
- Example suites under `examples/successful` and `examples/unsuccessful`.
- Build system via CMake.

## Major Additions Beyond `docs/Project_Report.pdf`

The report is now behind the codebase in several important areas.

### 1. First-class functions are implemented

The report still describes function support as future work. That is no longer true.

- The token layer now includes `fn`, `return`, and `,`.
- The AST includes `FunctionDeclNode`, `CallNode`, and `ReturnNode`.
- The compiler emits function-related bytecode.
- The VM executes calls through explicit call frames.

### 2. Recursion is supported

This is demonstrated by the bundled recursive factorial program in `examples/successful/22_recursive_factorial.cvm`.

### 3. Runtime values are more advanced than the report suggests

The VM now uses a tagged runtime value model with support for:

- integers
- booleans
- strings as a runtime type container
- function objects

String literals are still not part of the language syntax, but the runtime already has partial groundwork for richer value types.

### 4. Chained assignment has been repaired

The report calls out chained assignment stack underflow as a known limitation. The current compiler now reloads the assigned value, so expressions like:

`a = b = 7;`

work after rebuilding the current source.

### 5. REPL state is stronger

The REPL now keeps both variable state and function state across interactive inputs, which makes it much more useful for exploration.

## Current Strengths

### Clear educational architecture

The repo is easy to understand phase-by-phase, which is ideal for learning, demos, and extension work.

### Good foundation for observability

The separate visualizer is already the repo's strongest differentiator. It gives the project an identity beyond "another toy language".

### Real language growth has already started

With functions, recursion, scopes, and a tagged value model, the project has crossed from "tiny syntax demo" into "small but real language runtime".

## Current Gaps

### Language gaps

- Missing comparison operators: `>`, `<=`, `>=`, `!=`.
- No logical operators such as `and`, `or`, `not`.
- No modulo operator.
- No string literal syntax.
- No arrays, maps, or structs.
- No anonymous functions or closures.

### Diagnostics gaps

- Parser diagnostics are still basic.
- Errors do not yet include rich source spans or caret highlights.
- Lexer errors can cascade into noisier parser failures.

### Tooling gaps

- No formal automated test harness integrated into the build.
- No CI workflow enforcing build and regression checks.
- No golden-output tests for tokens, AST, bytecode, or VM traces.

## Repository Inconsistencies To Clean Up

- `docs/Project_Report.pdf` under-describes the current implementation.
- `examples/unsuccessful/21_chained_assignment_stack_underflow.cvm` is now outdated because chained assignment succeeds after a rebuild.
- Prebuilt binaries can become stale relative to source, so analysis should always be based on a fresh build.

## Best Current Positioning

The strongest current positioning for CVM++ is:

`a glass-box educational language runtime where every phase is inspectable`

That identity is already visible in the architecture and tooling. Future work should reinforce it rather than dilute it.
