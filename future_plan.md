# Future Plan

Theme date: June 5, 2026

## Recommended Theme

### CVM++ as a Glass-Box Language Lab

The best way for this repo to stand out is not by chasing feature count first.

The strongest unique characteristic is already present in the codebase:

`CVM++ can become a language where every stage of execution is visible, explainable, and teachable.`

That should be the theme.

Instead of trying to compete with larger scripting languages, CVM++ should aim to be:

- the most inspectable mini-language in its category
- a compiler/VM project that is excellent for demos, interviews, classrooms, and self-learning
- a repo where debugging and explanation are first-class features, not side utilities

## Guiding Principle

Every new feature should improve at least one of these:

- language expressiveness
- observability
- teaching value
- confidence in correctness

If a feature adds complexity without strengthening the glass-box identity, it should be lower priority.

## Feasible Next Advancements

These are practical, high-value improvements that fit the current architecture well.

### 1. Complete the comparison family

Add:

- `>`
- `<=`
- `>=`
- `!=`

Why this matters:

- closes the most visible feature gap
- aligns code with user expectations
- removes a limitation already called out in the docs

### 2. Add logical operators

Add:

- `and`
- `or`
- `not`

Why this matters:

- makes conditionals significantly more expressive
- unlocks more interesting teaching examples
- pairs naturally with control flow and boolean support

### 3. Upgrade diagnostics

Add better errors with:

- exact line and token context
- expected token reporting
- caret-style source highlights
- clearer runtime error messages for invalid calls, input failure, and undefined names

Why this matters:

- this is one of the highest-value improvements for an educational compiler
- it makes the repo feel much more polished than its size

### 4. Add an automated example test harness

Turn the existing example suites into repeatable regression tests.

Suggested coverage:

- successful program output checks
- unsuccessful program error checks
- a few golden snapshots for tokens, AST, and bytecode

Why this matters:

- protects the growing language surface
- gives confidence for future experimentation

### 5. Sync docs with reality

Update:

- `README.md`
- `docs/Project_Report.pdf` or add a newer markdown replacement
- outdated example labels

Why this matters:

- the repo now has more capability than its formal report claims
- cleaner documentation makes the project look actively maintained

## Best Medium-Term Advancements

These are the best next bets if you want the repo to become notably stronger, not just slightly larger.

### 1. Make the visualizer function-aware

Extend the visualizer to clearly show:

- function declarations
- function pool layout
- call frames
- argument binding
- return flow
- scope changes during recursion

Why this matters:

- this directly amplifies the standout identity
- recursion becomes much easier to understand visually

### 2. Add source-to-bytecode mapping

Track which AST node or source line generated each bytecode region.

Why this matters:

- bridges the gap between syntax and execution
- enables much better debugging and teaching output
- lays groundwork for trace views and explainers

### 3. Add a step-by-step trace mode

Introduce a mode that shows:

- current instruction
- stack before and after
- visible scopes
- jump decisions
- call stack growth and shrinkage

Why this matters:

- this can become the repo's signature feature
- it transforms CVM++ from a compiler project into a learning instrument

### 4. Add proper value introspection

Improve printing and debugging of runtime values by showing type-aware representations.

Examples:

- `42`
- `true`
- `<fn factorial/1>`

Why this matters:

- makes traces easier to read
- prepares the runtime for future value types

### 5. Add a formal bytecode spec

Document:

- opcode meanings
- operand widths
- stack effects
- call conventions
- truthiness rules

Why this matters:

- makes the project feel more complete and professional
- helps contributors reason about correctness

## Creative Standout Advancements

These are the ideas that can give the repo a memorable identity.

### 1. Narrative Trace Mode

Create an "explain my program" mode that narrates execution in plain English.

Example direction:

- "Evaluating condition `n == 0`"
- "Condition is false, entering recursive branch"
- "Calling `factorial` with argument `4`"
- "Returning `24` to caller"

Why this stands out:

- almost no small compiler/VM repos do this well
- it is highly demo-friendly
- it fits the glass-box theme perfectly

### 2. Time-Travel Execution Viewer

Store execution snapshots so users can move forward and backward through VM state.

Core views:

- stack timeline
- scope timeline
- call stack timeline
- bytecode pointer timeline

Why this stands out:

- makes debugging feel special
- turns the project into a mini research-grade teaching tool

### 3. Program Diff Visualizer

Let users compare two versions of a CVM++ program and inspect:

- token diffs
- AST diffs
- bytecode diffs
- output diffs

Why this stands out:

- very useful for learning
- excellent for presentations and assignments
- unusual enough to be memorable

### 4. Interactive Web or TUI Playground

Build a lightweight playground where users can:

- edit source
- see tokens, AST, bytecode, and output side-by-side
- step execution live

Why this stands out:

- dramatically improves accessibility
- makes the repo shareable beyond local builds

### 5. Challenge Mode

Turn the repo into a learning platform with guided exercises such as:

- "add `!=` support"
- "implement modulo"
- "fix a parser bug"
- "follow a recursive trace"

Why this stands out:

- makes the project useful both as software and as curriculum

## Recommended Signature Feature

If the repo should have one defining characteristic, it should be:

`Trace-first observability`

That means CVM++ should aim to be the mini-language where users can most easily answer:

- what did the lexer see?
- what AST was built?
- what bytecode was emitted?
- what happened on the stack?
- why did the VM branch here?
- how did recursion unfold?

This is more distinctive than only adding strings, arrays, or more syntax.

## Suggested Roadmap

### Phase 1: Foundation polish

- Add the missing comparison operators
- Add logical operators
- Improve diagnostics
- Add automated example-based tests
- Clean up outdated docs and example labels

### Phase 2: Standout observability

- Extend the visualizer for functions and recursion
- Add source-to-bytecode mapping
- Add step-by-step trace mode
- Improve runtime value display

### Phase 3: Signature experience

- Build narrative trace mode
- Export traces as markdown or JSON
- Add a playground or TUI-based inspector

### Phase 4: Selective language growth

Add only the features that help the theme:

- string literals
- modulo
- richer builtins
- arrays or lists

These should come after observability becomes the repo's clear identity.

## Final Recommendation

Do not position CVM++ as "a bigger toy language".

Position it as:

`a transparent, trace-first compiler and VM playground`

That identity is achievable with the current codebase, builds directly on the visualizer you already have, and gives the project a memorable reason to exist.
