# URI Parser

A c++ library for parsing URIs according to RFC 3986. This library can break down URIs into their components (scheme, authority, path, query, fragment) and validate them.

## What it does

Takes a URI string like `https://user:pass@example.com:8080/path?query=value#section` and breaks it into:

- **Scheme**: `https`
- **Authority**: `user:pass@example.com:8080`
- **Userinfo**: `user:pass`
- **Host**: `example.com`
- **Port**: `8080`
- **Path**: `/path`
- **Query**: `query=value`
- **Fragment**: `section`

## Dependencies

You need these installed on your system:

- **OS**: x86_64 GNU Linux only
- **Compiler**: g++ with C++23 support
- **Build tools**: cmake, make
- **Testing**: Google Test (gtest) - for running tests

## Building

### Quick start with CMake (recommended)

```bash
# Build the main program
make cmake-main-build

# Run the main program
make cmake-main-run
```

### Build and run tests

```bash
# Build tests
make cmake-test-build

# Run all tests
make cmake-test-run
```

### Manual build (without CMake)

If you prefer building without CMake:

```bash
# Build main program
make native-main

# Build tests
make native-test-build

# Build debug version
make native-build-debug
```

### Clean up

```bash
make clean
```

## Project structure

```
.
├── CMakeLists.txt          # Main CMake config
├── Makefile               # Build shortcuts
├── lib/                   # The actual URI parser library
│   ├── include/           # Header files
│   │   ├── uri_parser.hpp # Main parser class
│   │   └── uri_utils.hpp  # Helper utilities
│   └── src/               # Implementation
│       ├── uri_parser.cxx
│       └── uri_utils.cxx
├── src/
│   └── main.cxx          # Example usage
├── test/                 # All the tests
│   └── src/
│       ├── gtest_main.cxx      # Test runner
│       ├── basic_uri_tests.cxx # Basic parsing tests
│       ├── invalid_uri_tests.cxx # Error handling tests
│       ├── ipv6_tests.cxx      # IPv6 address tests
│       ├── parsing_edge_case_tests.cxx # Edge cases
│       ├── rfc_3986_cases_tests.cxx # RFC compliance tests
│       └── uri_component_tests.cxx # Component validation tests
└── build/                # Build output (created automatically)
```

## Usage

Here's how to use the library in your code:

```cpp
#include "uri_parser.hpp"

// Create a parser
URIParser parser;

// Parse a URI string
try {
    auto uri = parser.parse("https://example.com:8080/path?query#fragment");

    // Access components
    std::cout << "Scheme: " << uri->get_scheme() << std::endl;
    std::cout << "Host: " << uri->get_host() << std::endl;
    std::cout << "Port: " << uri->get_port().value_or(0) << std::endl;
    std::cout << "Path: " << uri->get_path() << std::endl;

    // Convert back to string
    std::cout << "Full URI: " << uri->to_string() << std::endl;

} catch (const URIParseException& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

## What works

- **Basic URIs**: `http://example.com/path`
- **With ports**: `https://example.com:8080/path`
- **User info**: `ftp://user:pass@example.com/path`
- **Query strings**: `http://example.com/path?key=value&foo=bar`
- **Fragments**: `http://example.com/path#section`
- **IPv6 addresses**: `http://[::1]:8080/path`
- **Relative URIs**: `/path/to/resource`
- **URI validation** according to RFC 3986

## Testing

The test suite covers:

- Basic URI parsing
- Invalid URI handling
- IPv6 address parsing
- Edge cases and corner cases
- RFC 3986 compliance
- Individual component validation

Run `make cmake-test-run` to see all tests pass.

## Common issues

**Build fails with C++23 errors**: Make sure your g++ version supports C++23. You might need g++ 11 or newer.

**Tests fail to build**: Make sure you have Google Test installed (check root Readme for more information). On Ubuntu/Debian:

```bash
sudo apt install libgtest-dev
```

**Permission denied when running**: The binaries are created in the `build/` directory. Make sure it's executable or run through make commands.
