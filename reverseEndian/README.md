# Endian Reversal Utility

A simple JavaScript utility that reverses the byte order (endianness) of 32-bit numbers. Useful for converting between little-endian and big-endian formats.

## What it does

Takes a 32-bit number and flips the order of its bytes:

- `0x12345678` becomes `0x78563412`
- `0xDEADBEEF` becomes `0xEFBEADDE`

This is commonly needed when working with binary data that needs to be converted between different endianness formats.

## Usage

```bash
node main.js [options]
```

### Options

```bash
node main.js --help / -h # print help
node main.js --test / -t # run inbuilt test
node main.js <hex>       # reverse <hex> Endianess and output it
```

### Example output

```
$ node main.js --help
Usage:
  node main.js <hexadecimal number>
  node main.js --test
  node main.js --help
$ node main.js 0x123456
Original:  0x00123456 (00 12 34 56)
Reversed:  0x56341200 (56 34 12 00)
```

## How it works

The function uses bit operations to move bytes around:

1. **Extract bytes**: Use bit masking (`&`) to isolate each byte
2. **Move bytes**: Use bit shifting (`<<`, `>>>`) to put each byte in its new position
3. **Combine**: Use bitwise OR (`|`) to put all bytes back together
4. **Ensure 32-bit**: Use unsigned right shift (`>>> 0`) to keep it as 32-bit unsigned

## Code docs

```javascript
const original = 0x12345678;
const reversed = reverseEndian32(original);
console.log(toHex(reversed)); // "0x78563412"
```

### With the helper functions

```javascript
const num = 0xdeadbeef;
console.log(`Original: ${toHex(num)} (${displayBytes(num)})`);
// Output: Original: 0xDEADBEEF (DE AD BE EF)

const flipped = reverseEndian32(num);
console.log(`Reversed: ${toHex(flipped)} (${displayBytes(flipped)})`);
// Output: Reversed: 0xEFBEADDE (EF BE AD DE)
```

- `reverseEndian32(num)`
  The main function that reverses byte order in a 32-bit number.

- `toHex(num)`
  Helper function that formats a number as a hex string.

- `displayBytes(num)`
  Helper function that shows the individual bytes of a number.

## Notes

- Only works with 32-bit numbers (4 bytes)
- Input numbers are automatically converted to 32-bit unsigned integers
- Reversing twice gives you back the original number
- Returns `undefined` for non-number inputs
- Command line accepts hex numbers with or without `0x` prefix
- Invalid hex input shows an error message and usage help
