## Tree Overview

- The folders

```bash
% tree -L 1 -d
.
├── answer.md        # scenario based answers
├── detectenc        #Q2 detect encryption
├── detectpessh      #Q1 detect if PE binaries are SSH clients
├── reverseEndian    #Q4 reverse the Endianess of a hex
└── rfc3986uriparser #Q3 rfc3986 URI parser
```

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
    - openssl
    - curl

---

### Dependencies installation:

- Debian/Ubuntu

```bash
sudo apt update
sudo apt install build-essential google-mock cmake nodejs make libssl-dev \
libcurl4-openssl-dev
```

- Archlinux

```bash
sudo pacman -Syu
sudo pacman -S base-devel google-test cmake nodejs make openssl curl
```

- Fedora

```bash
sudo dnf check-update
sudo dnf install gcc-c++ googletest-devel cmake nodejs make openssl-devel libcurl-devel
```

- Opensuse

```bash
sudo zypper refresh
sudo zypper install gcc-c++ googletest-devel cmake nodejs make \
libopenssl-devel libcurl-devel
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

```bash
cd rfc3986uriparser
make [build option]
```

build options:

```bash
cmake-test-build   # build test with cmake
cmake-test-run     # build test with cmake and run
cmake-main-build   # build main with cmake
cmake-main-run     # build main with cmake and run
native-test-build  # build test using g++
native-main        # build main using g++
native-main-debug  # build main using g++ with debug symbols
clean              # delete build dirs
```
