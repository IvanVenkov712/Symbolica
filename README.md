# Symbolica

A C++ console calculator that parses mathematical expressions, simplifies them,
and computes symbolic derivatives with respect to `x`.

## Documentation

- [Architecture](docs/architecture.md): parsing pipeline, expression hierarchy,
  ownership, evaluation, differentiation, and extension points.
- [Current problems](docs/known-issues.md): observed defects, mathematical domain
  inconsistencies, maintenance limitations, and a suggested repair order.
- Public headers in `Calculator/include` contain Doxygen-style API comments,
  including ownership requirements, exceptions, and known behavior caveats.

## Project layout

```text
Calculator.sln                    Visual Studio solution
Calculator/
  Calculator.vcxproj              Visual Studio build configuration
  Calculator.vcxproj.filters      Solution Explorer folders matching the layout
  include/
    expressions/                  Expression types and variable bindings
    parsing/                      Expression factory and shunting-yard interface
    utility/                      Polymorphic object handler template
  src/
    Main.cpp                      Console entry point
    expressions/                  Core expression and variable implementations
      binary/                     Binary operations and their base class
      unary/                      Unary operations, functions, and their base class
    parsing/                      Tokenization, parsing, and expression creation
```

Headers use paths relative to `Calculator/include`, such as
`#include "expressions/MathExpression.h"`. Visual Studio's include path is configured
for all Debug/Release and Win32/x64 configurations. When adding files, place them
in the relevant folder and add them to the project and matching filter.

## Build and run

Open `Calculator.sln` in Visual Studio 2022 with the **Desktop development with C++**
workload and a Windows 10 SDK installed, select a configuration, and build/run the
`Calculator` project. From a Visual Studio Developer PowerShell, you can also run:

```powershell
msbuild Calculator.sln /p:Configuration=Debug /p:Platform=x64
```

With GCC available, build from the repository root in PowerShell:

```powershell
New-Item -ItemType Directory -Path build -Force | Out-Null
$sources = @(Get-ChildItem Calculator/src -Recurse -Filter *.cpp | ForEach-Object FullName)
g++ -std=c++14 -ICalculator/include @sources -o build/Calculator.exe
./build/Calculator.exe
```

Enter expressions such as `2+3*4`, `x^2`, or `sin(x)`. Type `exit` to end the session;
the current loop reports a parse error for that command before quitting. EOF is
not handled correctly yet; see the [current problems](docs/known-issues.md).

Variables are lowercase letters `a` through `z`. Built-in functions are `sin`,
`cos`, `tg`, `cotg`, `sqrt`, and `ln`; trigonometric inputs use radians. Constants
are `_pi` and `_e`. Multiplication must be explicit, such as `2*x`. The parser has
known validation defects, so malformed expressions may be accepted or unsafe.

## Using the expression API

The console does not ask for numeric variable values, but the C++ API supports
evaluation with explicit bindings:

```cpp
#include "parsing/MathExpressionFactory.h"

auto expression = MathExpressionFactory::create("x^2+1");
VariableInterpretation values;
expression->populateVariables(values); // Discover names before assigning values.
values['x'] = 3;
double result = expression->eval(values); // 10
auto derivative = expression->differentiate('x');
auto simplified = derivative->simplify();
```

Handlers own their trees: copying deep-clones the tree, and moving transfers
ownership. Transformations leave the source unchanged. Simplification applies
local rules and currently does not guarantee preservation of mathematical domains.

There is no committed automated test suite or documentation generator configuration.
The build commands above compile the application; they do not run regression tests.
