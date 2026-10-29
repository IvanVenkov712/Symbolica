# Symbolica

A C++ console calculator that parses mathematical expressions, simplifies them,
and computes symbolic derivatives with respect to `x`.

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

Enter expressions such as `2+3*4`, `x^2`, or `sin(x)`. Type `exit` to end the session.
