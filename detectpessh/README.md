# PE SSH Client Detector

A tool that analyzes Windows PE (Portable Executable) files to detect if they're SSH clients. It looks for SSH-related strings, libraries, and other patterns that SSH clients typically contain.

## Requirements

- **os**: x86_64 GNU Linux only
- **compiler**: g++ with C++23 support
- **build tools**: make
- **testing tools**: curl

## Building

Clone or download the project, then:

```bash
make       # Compiles the program
```

The compiled binary will be in `build/detectpessh`.

## Usage

```bash
./build/detectpessh <path_to_pe_file>
```

Example:

```bash
./build/detectpessh tests/sample_files/putty.exe
```

Or use the makefile shortcut:

```bash
make run file_in=tests/sample_files/putty.exe
```

## Testing

Run the included test suite:

```bash
make test
```

This runs tests on sample SSH clients (PuTTY, PSCP, PuTTYtel) in the `tests/sample_files/` directory.

## How It Works

### PE File Detection

First, the tool checks if the file is a valid Windows PE executable:

- Looks for DOS header magic bytes "MZ"
- Finds the NT header using the DOS header's pointer
- Verifies NT header signature "PE\0\0"

### SSH Client Detection Methods

The detector uses several methods to identify SSH clients:

#### 1. String Analysis

Scans the entire file for SSH-related strings like:

- `ssh`, `openssh`, `putty`
- Key types: `ssh-rsa`, `ssh-dss`, `ssh-ed25519`
- File paths: `.ssh`, `known_hosts`, `authorized_keys`
- Commands: `ssh-keygen`, `ssh-add`, `sftp`, `scp`

#### 2. Import Analysis

Checks what Windows DLLs the program imports:

- Network libraries: `ws2_32.dll`, `wininet.dll`
- Crypto libraries: `crypt32.dll`, `bcrypt.dll`, `openssl`
- SSL libraries: `libssl`, `ssleay32.dll`

#### 3. Additional Heuristics

- SSH config file references
- Protocol version strings (`ssh-2.0`, `ssh-1.`)
- Crypto algorithm names (`aes`, `3des`, `diffie-hellman`)
- File size check (SSH clients are usually > 100KB)

### Confidence Scoring

Each detection method adds points to a confidence score:

- SSH strings: 12-25 points each
- Crypto libraries: 12 points each
- Config references: 15 points each
- Protocol strings: 10 points each

**Results:**

- 80+ points: "Very likely an SSH client"
- 50-79 points: "Possibly an SSH client"
- 20-49 points: "Unlikely to be an SSH client"
- <20 points: "Not an SSH client"

### Configuration Files

You can customize detection patterns using config files:

- `config/dllMap.conf` - DLL names and their scores
- `config/sshMap.conf` - SSH strings and their scores

Format: `pattern=score` (one per line)

If config files don't exist, the tool uses built-in defaults.

## Exit Codes

- `0`: File is likely an SSH client
- `1`: File is not an SSH client or error occurred

## Example Output

```
=== PE SSH Client Analysis ===
File size: 524288 bytes
Confidence score: 85/100

Findings:
  • Valid Windows PE executable
  • Found SSH-related string: putty
  • Found SSH-related string: ssh-rsa
  • Found SSH-related import: ws2_32.dll
  • Found SSH-related import: crypt32.dll
  • Total SSH-related strings found: 12

Conclusion: Very likely an SSH client
```

## Project Structure

```
.
├── build/               # Compiled binaries
├── src/                 # Source code
│   ├── main.cxx        # Main program
│   ├── detectpessh.hpp # Class definition
│   ├── detectpessh.cxx # Implementation
│   └── pe_headers.hpp  # PE file structures
├── tests/              # Test files and scripts
│   ├── run_tests.sh    # Test runner
│   └── sample_files/   # Sample SSH clients
└── Makefile           # Build configuration
```

## Cleaning Up

```bash
make clean    # Removes build directory
```

## Limitations

- Only works on x86_64 GNU Linux
- Only analyzes Windows PE files
- Detection is heuristic-based (not 100% accurate)
- May give false positives for programs that use similar libraries
- Cannot detect heavily obfuscated or packed executables
