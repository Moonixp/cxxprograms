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
    if (typeof (num) !== "number") {
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
            `Double reversed: ${toHex(doubleReversed)} (original: ${doubleReversed === num})`,
        );
        console.log("---");
    });
}

test()
