## Tree Overview

- The folders

```bash
% tree -L 1 -d
.
├── detectenc        #Q2 detect encryption
├── detectpessh      #Q1 detect if PE binaries are SSH clients
├── reverseEndian    #Q4 reverse the Endianess of a hex
└── rfc3986uriparser #Q3 rfc3986 URI parser
5 directories
```

- `answer.md` contains senario based answers

---

## Build Requirements (Dependencies)

- should be build on a `x86_64 GNU/Linux system`

### Deps:

    - g++ / gcc > 13
    - google test lib
    - cmake & ccmake
    - node
    - make
    - base devel

### Dependencies installation:

- Debian/Ubuntu

```bash
sudo apt update
sudo apt install build-essential google-mock cmake nodejs make
```

- Archlinux

```bash
sudo pacman -Syu
sudo pacman -S base-devel google-test cmake nodejs make
```

- Fedora

```bash
sudo dnf check-update
sudo dnf install gcc-c++ googletest-devel cmake nodejs make
```

- Opensuse

```bash
sudo zypper refresh
sudo zypper install gcc-c++ googletest-devel cmake nodejs make
```

---

## Build

### detectpessh

```bash
cd detectpessh/
make
ls build/*  # binary should be in build
```

### detectenc

```bash
cd detectenc/
make
ls build/*  # binary should be in build

```

### reverseEndian

```bash
cd reverseEndian/
node main.js
```

### rfc3986uriparser

It is structured into a library and a separate testing suite.
It uses `CMake` as its primary build system, but a native
`g++` build is also an option.

- build options:
  - cmake-test-build => build test with cmake
  - cmake-test-run => build test with cmake and run
  - cmake-main-build => build main with cmake
  - cmake-main-run => build main with cmake and run
  - native-test-build => build test using g++
  - native-main => build main using g++
  - native-main-debug => build main using g++ with debug symbols
  - clean => delete build dirs

#### Example build test with cmake

- build tests
  ```bash
  cd rfc3986uriparser/
  make cmake-test-build
  ls ./test_build/uri_parser_tests # binary path
  ```
- build and run tests
  ```bash
  cd rfc3986uriparser/
  make cmake-test-run
  ```

#### rfc3986uriparser Doc

- tree is as followed

```tree
% tree  -d
.
├── lib          # contains parser
│   ├── include
│   └── src
└── test        # contains tests
    ├── include
    └── src    # contains individual test files

7 directories
```

- 3 `CMakeLists.txt`
  - one in lib to build to a static lib
  - one in test to build
  - one in root to build main with lib
