// Function to reverse the endianness of a 32-bit number
function reverseEndian32(num) {
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

// Alternative implementation using ArrayBuffer and DataView
function reverseEndianBuffer(num) {
  const buffer = new ArrayBuffer(4);
  const view = new DataView(buffer);

  // Write as big-endian, read as little-endian (or vice versa)
  view.setUint32(0, num, false); // false = big-endian
  return view.getUint32(0, true); // true = little-endian
}

// Utility function to display number in hex format
function toHex(num) {
  return "0x" + (num >>> 0).toString(16).toUpperCase().padStart(8, "0");
}

// Utility function to display bytes
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

// Test the functions
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
  const reversed2 = reverseEndianBuffer(num);

  console.log(`Original:  ${toHex(num)} (${displayBytes(num)})`);
  console.log(`Reversed:  ${toHex(reversed1)} (${displayBytes(reversed1)})`);
  console.log(
    `Buffer method: ${toHex(reversed2)} (match: ${reversed1 === reversed2})`,
  );

  // Verify that reversing twice gives us back the original
  const doubleReversed = reverseEndian32(reversed1);
  console.log(
    `Double reversed: ${toHex(doubleReversed)} (original: ${doubleReversed === num})`,
  );
  console.log("---");
});

// Interactive example
console.log("\n=== Interactive Example ===");
console.log("To use these functions with your own numbers:");
console.log(
  "reverseEndian32(0x12345678) =>",
  toHex(reverseEndian32(0x12345678)),
);
console.log(
  "reverseEndianBuffer(0x12345678) =>",
  toHex(reverseEndianBuffer(0x12345678)),
);

// Export functions for use in other modules (if using Node.js modules)
if (typeof module !== "undefined" && module.exports) {
  module.exports = {
    reverseEndian32,
    reverseEndianBuffer,
    toHex,
    displayBytes,
  };
}
