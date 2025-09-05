## Question 5: Detecting Remote Reverse Engineering

Detecting someone reverse engineering your application on their own network is very difficult, but here are some ways a company could detect it:

- phone-home functionality, if the app normally connects back to company servers for updates, license checks, or telemetry, they might notice connection attempts from unauthorized IPs or see the app running in unexpected environments.
- anti-debugging measures, the app might have built-in protections that detect debuggers and either crash, behave differently, or send alerts back home when tampered with.
- honeypot data , if the app contains fake API endpoints or credentials that would only be accessed through reverse engineering, they could monitor for attempts to use those.
- code obfuscation artifacts, some obfuscation techniques leave traces when bypassed that could be detected.

Basically, if someone has a standalone copy on an isolated network, it's really difficult to impossible? If your app is
talking to your servers, sure, you might notice something fishy. But if someone has
a copy on an air-gapped network, like a network that literally isn't connected to the
internet, theres no way

---

## Question 6: Trap Employee Leaking Sensitive Information

This is a classic canary trap. Even though the SHA256 hashes are identical and filenames look the same,
ThreatSpike likely used steganography, hiding unique things in the files that don't change the hash or apparent content.
They probably embedded invisible watermarks like:

- Slight spacing differences in text documents.
- Modified metadata that doesn't affect the hash.
- Pixel-level changes in images that can't be seen unless you're looking or know where it is.
- Different formatting or encoding that renders identically.

Each employee got what appears to be an identical file but with their own unique "fingerprint" embedded. When the file showed up on the sharing site, they could extract the watermark and trace it back to the specific employee

---

## question 7: baby cam hack

The solution: Network traffic analysis
When the baby wakes up and starts crying, the skype call automatically increases its bandwidth usage to transmit the audio so the engineer can see this spike in network traffic on his switch dashboard.
Here's what's happening:

- The Baseline traffic, When baby is sleeping, skype maintains a low-bandwidth connection (just keepalives, maybe some ambient noise)
- Traffic spike, when the baby wakes up and cries, skype detects audio then bandwidth usage jumps transmit the crying
- In the dashboard monitoring, it displays the network interface charts in PacketTrap to show real-time bandwidth utilization, so he'd see a sudden spike in transmit/receive on whichever port the computer is connected to

Looking at the dashboard:
Those network interface charts (Ports 2-4, 22-24) show real-time traffic patterns
You can see the historical bandwidth usage over time
A crying baby would cause a very obvious spike in the Skype traffic

this works because skype uses adaptive bitrate

- quiet room = low bandwidth,
- crying baby (audio, movement) = much higher bandwidth

## Question 8: Upward-Growing Stacks and Buffer Overflows

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

Stack grows down: this overwrites return address below the buffer
Stack grows up: this would overwrite return address above the buffer
Result: the same vulnerability, just in the opposite direction

What actually prevents stack overflows:

- Stack canaries - Random values placed between buffers and return addresses
- ASLR - Randomizing memory layout
- NX bit - Making stack non-executable
- Bounds checking - Actually validating input sizes
- Stack guards - Hardware/OS protection of stack pages

---
