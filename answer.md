## Question 1: Detecting Remote Reverse Engineering

Detecting someone reverse engineering your application on their own network is very difficult, but here are some ways a company could detect it:

#### Detection Methods:

A few possibilities come to mind:

- Phone-home functionality - If the app normally connects back to company
  servers for updates, license checks, or telemetry, they might notice connection attempts from unauthorized IPs or see the app running in unexpected environments
- Anti-debugging measures - The app might have built-in protections that detect debuggers and either crash, behave differently, or send alerts back home when tampered with
- Honeypot data - If the app contains fake API endpoints or credentials that would only be accessed through reverse engineering, they could monitor for attempts to use those
- Code obfuscation artifacts - Some obfuscation techniques leave traces when bypassed that could be detected

The reality is though, if someone has a standalone copy on an isolated network,
detection becomes really difficult.

---

## Question 2: Canary Trap Detection

This is a classic canary trap! Even though the SHA256 hashes are identical and filenames look the same, ThreatSpike likely used steganography - hiding unique identifiers in the files that don't change the hash or apparent content.
They probably embedded invisible watermarks like:

Slight spacing differences in text documents
Modified metadata that doesn't affect the hash
Pixel-level changes in images that are imperceptible
Different formatting or encoding that renders identically

Each employee got what appears to be an identical file but with their own unique "fingerprint" embedded. When the file showed up on the sharing site, they could extract the watermark and trace it back to the specific employee

---

## Question 3: Upward-Growing Stacks and Buffer Overflows

The real problem isn't direction, it's bounds checking
Stack overflows happen because programs write past the allocated stack space, not because of which direction it grows. Whether the stack grows up (toward higher memory addresses) or down (toward lower addresses), you can still overflow it.
Classic buffer overflow example:

```c
void some_func(char* input) {
    char buffer[64];
    strcpy(buffer, input);  // No bounds checking
}
```

If someone passes in 100 bytes of data:

Stack grows down: overwrites return address below the buffer
Stack grows up: would overwrite return address above the buffer
Result: Same vulnerability, just in the opposite direction

- What actually prevents stack overflows:

Stack canaries - Random values placed between buffers and return addresses
ASLR - Randomizing memory layout
NX bit - Making stack non-executable
Bounds checking - Actually validating input sizes
Stack guards - Hardware/OS protection of stack pages

---

## question 5

The solution: Network traffic analysis
When the baby wakes up and starts crying, the Skype call automatically increases its bandwidth usage to transmit the audio. The engineer can see this spike in network traffic on his switch dashboard.
Here's what's happening:

Baseline traffic: When baby is sleeping, Skype maintains a low-bandwidth connection (just keepalives, maybe some ambient noise)
Traffic spike: Baby wakes up and cries → Skype detects audio → bandwidth usage jumps dramatically to encode and transmit the crying
Dashboard monitoring: The network interface charts in PacketTrap show real-time bandwidth utilization - he'd see a sudden spike in transmit/receive on whichever port the computer is connected to

Looking at the dashboard:

Those network interface charts (Ports 2-4, 22-24) show real-time traffic patterns
You can see the historical bandwidth usage over time
A crying baby would cause a very obvious spike in the Skype traffic

Why this works so well:

Skype uses adaptive bitrate - quiet room = low bandwidth, crying baby = much higher bandwidth
Network switches log all this traffic data
Modern baby monitors work the same way - many parents actually use this technique!
