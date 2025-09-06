function toHex(num) {
  return "0x" + (num >>> 0).toString(16).toUpperCase().padStart(8, "0");
}

function displayBytes(num) {
  const bytes = [
    (num >>> 24) & 0xff,
    (num >>> 16) & 0xff,
    (num >>> 8) & 0xff,
    num & 0xff,
  ];
  return bytes
    .map((b) => b.toString(16).toUpperCase().padStart(2, "0"))
    .join(" ");
}

function reverseEndian32(num) {
  if (typeof num !== "number") {
    return;
  }

  // Ensure the input is treated as a 32-bit unsigned integer
  num = num >>> 0;

  // Extract each byte using bit shifting and masking
  const byte0 = (num & 0x000000ff) << 24; // Move least significant byte to most significant
  const byte1 = (num & 0x0000ff00) << 8; // Move second byte to third position
  const byte2 = (num & 0x00ff0000) >>> 8; // Move third byte to second position
  const byte3 = (num & 0xff000000) >>> 24; // Move most significant byte to least significant

  // Combine all bytes in reversed order
  return (byte0 | byte1 | byte2 | byte3) >>> 0;
}

// Test the functions
function test() {
  console.log("=== 32-bit Endian Reversal Tests ===\n");

  const testNumbers = [
    0x12345678,
    0xdeadbeef,
    0x00ff00ff,
    0x11223344,
    255, // 0x000000FF
  ];

  testNumbers.forEach((num) => {
    const reversed1 = reverseEndian32(num);
    console.log(`Original:  ${toHex(num)} (${displayBytes(num)})`);
    console.log(`Reversed:  ${toHex(reversed1)} (${displayBytes(reversed1)})`);

    // reversing twice gives us back the original
    const doubleReversed = reverseEndian32(reversed1);
    console.log(
      `Double reversed: ${toHex(doubleReversed)} (original: ${
        doubleReversed === num
      })`
    );
    console.log("---");
  });
}
function print_help() {
  console.log("Usage:");
  console.log("  node main.js <hexadecimal number>");
  console.log("  node main.js --test");
  console.log("  node main.js --help");
}

function main() {
  if (process.argv.length > 2) {
    if (process.argv[2] === "--test" || process.argv[2] === "-t") {
      test();
    } else if (process.argv[2] === "--help" || process.argv[2] === "-h") {
      print_help();
    } else {
      if (process.argv[2].startsWith("0x")) {
        process.argv[2] = process.argv[2].slice(2);
      }
      const inputNumber = parseInt(process.argv[2], 16);
      if (isNaN(inputNumber)) {
        console.log("Error: Please enter a valid hexadecimal number");
        console.log("\n\nUsage:");
        console.log("  node main.js <hexadecimal number>");
        process.exit(1);
      }
      const reversedNumber = reverseEndian32(inputNumber);
      console.log(
        `Original:  ${toHex(inputNumber)} (${displayBytes(inputNumber)})`
      );
      console.log(
        `Reversed:  ${toHex(reversedNumber)} (${displayBytes(reversedNumber)})`
      );
    }
  } else {
    print_help();
  }
}

main();
