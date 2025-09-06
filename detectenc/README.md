# Encryption Detector

A simple C++ tool that looks at files and tries to figure out if they're encrypted or not. It uses basic math and statistics to spot patterns that encrypted files usually have.

## What You Need

- **Linux x86_64** (only tested on GNU/Linux)
- **g++** compiler with C++23 support
- **make**
- **openssl** for generating encrypted files for testing
- **curl** for downloading dummy files

## How to Build

```bash
# Then just run make
make

# Or compile manually if you want:
g++ -std=c++23 -O2 -o detectenc main.cxx detectenc.cxx
```

## How to Use

```bash
# Check if a file looks encrypted
./detectenc myfile.txt

# Examples:
./detectenc document.pdf
./detectenc encrypted_archive.7z
./detectenc /etc/passwd
```

The program returns different exit codes:

- **0** = File looks encrypted (high confidence)
- **2** = File doesn't look encrypted
- **1** = Error (file not found, etc.)

## Testing

The project includes a test suite with sample files:

```bash
# Run all tests
make test

# This runs tests/run_tests.sh which checks various file types:
# - 15MBFILE.txt (should not be encrypted)
# - 20MBPDF.pdf (should not be encrypted)
# - 46MBVIDEO.mp4 (should not be encrypted)
# - 5MB_encrypted.bin, 10MB_encrypted.bin, 20MB_encrypted.bin (should be encrypted)
```

Try it on different file types to see how it works:

```bash
# These should score LOW (not encrypted):
./build/detectenc /etc/passwd          # Text file
./build/detectenc tests/sample_files/15MBFILE.txt    # Plain text
./build/detectenc tests/sample_files/20MBPDF.pdf     # PDF file

# These should score HIGH (likely encrypted):
./build/detectenc tests/sample_files/5MB_encrypted.bin   # Encrypted file
./build/detectenc encrypted.gpg        # GPG encrypted file
./build/detectenc password_protected.zip  # Password protected archive
```

## Code Docs

The tool runs 6 different tests on your file and gives each one a score. If the total score is 70 or higher, it says the file is probably encrypted.

### The Tests

**1. Shannon Entropy (30 points max)**

- Measures how "random" the bytes look
- Encrypted files have very random-looking data (close to 8.0 bits)
- Text files have patterns, so they score lower

**2. Chi-Square Test (25 points max)**

- Checks if all byte values (0-255) appear about equally
- Encrypted data should have roughly the same amount of each byte
- Text files have some bytes way more common than others

**3. ASCII Ratio (15 points max)**

- Counts how many bytes are normal text characters
- Text files are mostly printable characters
- Encrypted files look like random binary junk

**4. Byte Variance (10 points max)**

- Measures how spread out the byte values are
- Encrypted data uses the full range of possible bytes
- Some file types stick to certain byte ranges

**5. Pattern Repetition (10 points max)**

- Looks for repeating 4-byte patterns
- Encrypted data shouldn't repeat much
- Some files have headers or repeated structures

**6. Transition Entropy (10 points max)**

- Checks how random the transitions between bytes are
- In encrypted data, any byte can be followed by any other byte
- In normal files, certain byte combinations are more common

## Why These Tests Work

Encryption is basically supposed to make data look completely random. Good encryption makes it impossible to tell the difference between encrypted data and true random noise.

So we look for signs that data is random:

- All bytes appear equally often
- No patterns repeat
- You can't predict what byte comes next
- Nothing looks like normal text

Real files (documents, images, programs) all have patterns. Even compressed files have some structure left. Only encrypted files consistently look truly random.

## Limitations

- It's not perfect - some compressed files might look encrypted
- Very small files (under 1KB) are harder to analyze accurately
- Some file formats naturally look more random than others
- It won't tell you WHAT kind of encryption was used
