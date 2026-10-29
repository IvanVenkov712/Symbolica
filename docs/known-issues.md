# Current problems

This document records observed behavior and problems visible in the current code.
It does not claim that these defects have been fixed. Console examples were
checked with the GCC C++14 build; variable/evaluation examples were checked through
the C++ API. Floating-point spellings such as `nan` and `-inf` can vary by runtime.
The unchecked stack access and ownership failure paths below were identified by
code inspection rather than deliberately triggering undefined behavior.

## Parser validation and input handling

| Priority | Problem and evidence | Consequence / direction for a fix |
| --- | --- | --- |
| Critical | [`convertToRPN()`](../Calculator/src/parsing/ShuntingYard.cpp) handles `)` by unconditionally popping the operator stack after its search. With input `)`, that stack is empty. | Undefined behavior rather than a parse error. Check for a matching `(` before popping and validate delimiters explicitly. |
| High | Unknown tokens have no rejection branch in `convertToRPN()` or `calculatePRN()`. `2+typo(3)` is accepted and printed as `2+3`, simplifying to `5`. | Misspelled names can silently change meaning. Reject unknown/empty tokens and require a valid operator/operand sequence. |
| Medium | `tokenize()` can emit an empty token for a whitespace-only fragment. `-x` works, but ` -x` fails because the empty fragment changes minus classification. | Ordinary whitespace changes syntax. Tokenize whitespace explicitly and track whether an operand is expected. |
| Medium | Commas are recognized despite the absence of multi-argument functions or a comma-expression grammar. For example, `1,` can be accepted as `1`. | Invalid syntax is inconsistently accepted. Reject commas until an arity-aware grammar exists. |
| Medium | Tokens contain strings but no source positions; parser exceptions are generally just `Invalid expression!`. | Users cannot locate errors. Store token kind, spelling, and source span; report the failing token and expected syntax. |

Number recognition only accepts a small decimal format. Conversion in `toNumber()`
does not check stream failure or range. This needs an explicit numeric conversion
policy along with syntax validation, rather than relying on a prior character check.

## Console lifecycle

In [`Main.cpp`](../Calculator/src/Main.cpp), the loop tests the previous input for
`exit`. The new line is parsed before the next loop condition, so typing `exit`
prints `An error occurred: Invalid expression!` before quitting. A failed
`std::getline()` is never checked. EOF can therefore leave the program repeatedly
printing prompts/errors instead of terminating. Check the stream result and the
exit command immediately after reading, before calling the factory.

The interface always differentiates with respect to `x`, does not expose numeric
evaluation or binding assignment, and constructs the same derivative twice per
iteration. These are interface/performance limitations rather than parser defects.

## Variable bindings

[`VariableInterpretation::removeVar()`](../Calculator/src/expressions/VariableInterpretation.cpp)
assigns `true` to `participatingVars[ch - 'a']`. After `addVar('x', 7)` followed by
`removeVar('x')`, `participates('x')` is still true and the count remains one.
Removing a previously absent variable also registers it. Set the flag to false
and check absent/present removal and the cached count.

[`Variable::populateVariables()`](../Calculator/src/expressions/Variable.cpp) calls
`addVar(var)` with zero. Discovering variables in an existing binding set silently
overwrites assigned values. For now, populate a fresh binding set first and assign
values afterward. A future API should distinguish registering a name from assigning
or resetting its value.

## Mathematical domain and simplification

These are correctness problems: the result can depend on whether an expression
was evaluated directly or simplified first.

| Example | Current behavior | Underlying issue |
| --- | --- | --- |
| `ln(0)` | `eval()` throws; `simplify()` produces negative infinity. | [`NaturalLogarithm::simplify()`](../Calculator/src/expressions/unary/NaturalLogarithm.cpp) calls `std::log` directly and bypasses the domain check. |
| `sqrt(-1)` | `eval()` throws; `simplify()` produces NaN. | [`SquareRoot::simplify()`](../Calculator/src/expressions/unary/SquareRoot.cpp) bypasses the evaluation check. |
| `(-2)^2` | `eval()` throws; `simplify()` produces `4`. | [`Power::eval()`](../Calculator/src/expressions/binary/Power.cpp) rejects every negative base, while folding uses `std::pow` directly. Integer exponents are not distinguished. |
| `0/x`, with `x = 0` | Direct evaluation produces NaN on the checked runtime; the simplified constant evaluates to zero. | The zero-numerator rule removes the denominator and its domain restriction. |
| `0^x`, with `x = 0` | Direct evaluation returns `1` on the checked runtime; simplification produces zero. | The zero-base rule ignores the unknown exponent, including zero and negative values. |

Multiplication by zero and similar identities can also remove invalid child
expressions. Division and trigonometric singularities do not have a unified check
or error policy. Decide whether the engine preserves real-number domains, permits
extended floating-point results, or tracks assumptions, then share that policy
between evaluation and constant folding. Rewrite rules need preconditions.

`areEqual()` uses one absolute machine-epsilon threshold, with no relative scaling
or finite-value policy. Values very close to zero/one can trigger identities that
change exact floating-point behavior, while NaN/infinity never compare equal.
This is an implicit numerical policy that should be made explicit.

## Derivatives and display

The general power derivative in
[`Power::differentiate()`](../Calculator/src/expressions/binary/Power.cpp) builds
`u^v * ((u'/u)*v + ln(u)*v')` even when `v` is constant. For `x^2`, evaluating the
unsimplified derivative at `x = 0` throws from `ln(0)`, though the polynomial's
derivative there is zero. Its simplified tree still contains division by `x`.
Use specialized constant-exponent/base rules and preserve domain assumptions.

Local simplification cannot combine like terms or cancel symbolic factors:
`x+x` remains unchanged, and the simplified derivative of `x^2` prints as
`x^2*1/x*2`. Each operation has its own rewrite code, and there is no structural
equality or canonical expression representation. Handler `operator==` compares
pointers only; it cannot be used to detect equal subexpressions.

Printing uses default stream precision and a precedence system separate from the
parser. Numeric constant leaves can contain negative values, NaN, or infinity,
but those printed values are not all valid atomic parser tokens. For example,
a programmatically constructed `Power(Constant(-2), Variable('x'))` prints
`-2^x`, which reparses as `-(2^x)` instead of `(-2)^x`. Printed output is display
text, not a guaranteed serialization format.

## Ownership and exception safety

[`poly_obj_handler`](../Calculator/include/utility/poly_obj_handler.hpp) performs
manual `delete` and `clone()` operations. `free()` deletes the pointer without
clearing it. Copy assignment and `reset(const T&)` free the old object before
cloning the replacement. If cloning throws, the handler retains a dangling pointer,
which its destructor may delete again. This is a high-priority memory correctness
issue. Construct the replacement before releasing the old object, or use a standard
ownership type and a copy-and-swap strategy.

The reset overloads also permit unsafe aliases: `handler.reset(handler.getPtr())`
deletes the pointer it then stores, and `handler.reset(*handler)` clones an object
after deleting it. Constructors that adopt raw pointers rely on callers to avoid
duplicate owners. These preconditions are now documented, but the API does not
enforce them. Nested raw allocations while constructing derivative trees also
make allocation-failure cleanup difficult to guarantee.

## Maintainability, performance, and validation

- All concrete expression declarations are in one large header. The factory's
  public header includes the parser, which includes the full expression hierarchy.
  New operations therefore affect many translation units and couple API users to
  parser internals.
- Recognition, operation dispatch, function spellings, and precedence are defined
  in multiple places. Parser and printer precedence use different numeric scales.
  Operation-specific simplification repeats traversal and folding logic.
- Deep-copy ownership clones subtrees when handlers are passed as lvalues. This is
  intentional value semantics, but repeated transformations can allocate many nodes.
  `calculatePRN()` additionally copies its final stack root on return, and the
  console computes the derivative twice. Measure these costs before changing the
  ownership model.
- Traversal, destruction, and cloning are recursive, with no expression-size or
  nesting-depth limit. Very deep trees can exhaust stack or memory.
- Headers rely on transitive standard-library includes: `poly_obj_handler.hpp` uses
  `std::move` without including `<utility>` itself, and `VariableInterpretation.h`
  uses unqualified `size_t` without directly including `<cstddef>`.
- There is no committed automated test suite, CI configuration, dedicated library
  target, or portable build configuration. GCC can be invoked manually as documented
  in the README; Visual Studio is the only checked-in build project.

## Suggested repair order

1. Make parsing safe and strict; handle EOF/exit correctly. Cover invalid tokens,
   delimiter mismatches, whitespace, missing operands, and extra operands.
2. Repair handler exception/alias safety and variable removal. Check ownership with
   a deliberately throwing `clone()` implementation and verify variable counts.
3. Define domain and floating-point policies; make folding and evaluation agree.
   Check boundaries before adding more simplification identities.
4. Specialize power differentiation and test numerical derivatives where valid,
   including zero/negative inputs and singularities.
5. Add regression tests and CI, then address formatting, duplicated metadata,
   header dependencies, and measured allocation costs.
