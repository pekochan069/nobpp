# nobpp (NoBuild c++)

## Glossary

This project is a build tool(kind of) for C/C++.

This project is heavily inspired by [tsoding](https://www.twitch.tv/tsoding)'s [nob.h](https://github.com/tsoding/nob.h).

The idea is simple.

> I should be able to create c++ binary with only compiler. No cmake, No make, No visual studio, No anything but compiler.

## How to use it

### Requirements

[clang](https://clang.llvm.org/) is required if you don't want to edit the nobpp source code.

> [!NOTE]
> I chose Clang as the compiler of choice for cross-platform support, but with some modifications to the source code, it can be configured to work with GCC, MSVC, or other compilers as needed.

### Using nobpp

Copy Paste the `nobpp.hpp` to your project.

```sh
wget https://raw.githubusercontent.com/pekochan069/nobpp/refs/heads/main/nobpp.hpp
```

or

```sh
curl -o nobpp.hpp https://raw.githubusercontent.com/pekochan069/nobpp/refs/heads/main/nobpp.hpp
```

After that, write build script for your project.

```c++
// build.cpp or nobpp.cpp or whatever.cpp
#include "nobpp.hpp"

int main()
{
    nobpp::CommandBuilder builder = nobpp::CommandBuilder();

    builder.set_language(nobpp::Language::cpp)
        .set_target_os(nobpp::TargetOS::windows)
        .set_optimization_level(nobpp::OptimizationLevel::o3)
        .add_options({"-ffast-math"})
        .add_file("./test.cpp")
        .add_build_dir("./bin")
        .set_output("test")
        .run();
}
```

### Compile nobpp

`nobpp` requires clang version which support c++14 or higher.

```sh
clang++ -std=c++14 -O3 build.cpp -o build.exe # On Windows
clang++ -std=c++14 -O3 build.cpp -o build     # On Linux
```

After compilation, you can just run the executable and your project will be compiled.

## support

### Platform

- [x] Windows
- [ ] Linux

### Output Type

- [x] Executable
- [ ] Static Library
- [ ] Dynamic Library

### Features

- [x] Command Queue
- [ ] Task
