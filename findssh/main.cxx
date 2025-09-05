#include "headers.hpp"
#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

class PESSHDetector {
private:
  std::vector<uint8_t> fileData;
  DOS_HEADER dosHeader;
  NT_HEADERS ntHeaders;
  std::vector<SECTION_HEADER> sectionHeaders;
  int confidence;
  std::vector<std::string> findings;

  // map the string to confidence
  std::set<std::string> sshStrings = {
      "ssh",         "SSH",         "openssh",
      "OpenSSH",     "putty",       "PuTTY",
      "PUTTY",       "ssh-rsa",     "ssh-dss",
      "ssh-ed25519", "ecdsa-sha2",  "id_rsa",
      "id_dsa",      "known_hosts", "authorized_keys",
      ".ssh",        "~/.ssh",      "%USERPROFILE%\\.ssh",
      "ssh-keygen",  "ssh-add",     "ssh-agent",
      "SecureShell", "terminal",    "sftp",
      "scp"};

  //  libraries commonly used by SSH clients
  std::set<std::string> sshLibraries = {
      "ws2_32.dll",   "wsock32.dll",  "wininet.dll",  "crypt32.dll",
      "advapi32.dll", "bcrypt.dll",   "libssl",       "libcrypto",
      "openssl",      "libeay32.dll", "ssleay32.dll", "ncrypt.dll",
      "cryptsp.dll"};

public:
  bool loadPEFile(const std::string &filename) {
    if (!std::filesystem::exists(filename)) {
      std::cerr << "Error: " << filename << " Does not exist" << '\n';
      return false;
    }
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Cannot open file " << filename << '\n';
      return false;
    }

    // Read entire file
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    fileData.resize(fileSize);
    file.read(reinterpret_cast<char *>(fileData.data()), fileSize);
    file.close();

    confidence = 0;
    findings.clear();

    return true;
  }

  bool isPEFormat() {
    if (fileData.size() < sizeof(DOS_HEADER)) {
      return false;
    }

    // Check DOS header
    std::memcpy(&dosHeader, fileData.data(), sizeof(DOS_HEADER));
    if (dosHeader.e_magic != 0x5A4D) { // "MZ"
      return false;
    }

    if (dosHeader.e_lfanew >= fileData.size() ||
        dosHeader.e_lfanew + sizeof(NT_HEADERS) > fileData.size()) {
      return false;
    }

    // Check NT header
    std::memcpy(&ntHeaders, fileData.data() + dosHeader.e_lfanew,
                sizeof(NT_HEADERS));
    if (ntHeaders.Signature != 0x00004550) { // "PE\0\0"
      return false;
    }

    findings.push_back("Valid Windows PE executable");
    confidence += 10;
    return true;
  }

  void readSectionHeaders() {
    size_t sectionOffset = dosHeader.e_lfanew + sizeof(NT_HEADERS);
    sectionHeaders.resize(ntHeaders.FileHeader.NumberOfSections);

    for (int i = 0; i < ntHeaders.FileHeader.NumberOfSections; i++) {
      if (sectionOffset + sizeof(SECTION_HEADER) <= fileData.size()) {
        std::memcpy(&sectionHeaders[i], fileData.data() + sectionOffset,
                    sizeof(SECTION_HEADER));
        sectionOffset += sizeof(SECTION_HEADER);
      }
    }
  }

  void analyzeStrings() {
    std::string fileContent(fileData.begin(), fileData.end());
    std::transform(fileContent.begin(), fileContent.end(), fileContent.begin(),
                   ::tolower);

    int stringMatches = 0;
    for (const auto &sshString : sshStrings) {
      std::string lowerStr = sshString;
      std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                     ::tolower);

      if (fileContent.find(lowerStr) != std::string::npos) {
        findings.push_back("Found SSH-related string: " + sshString);
        stringMatches++;

        // Weight certain strings higher
        if (lowerStr == "openssh" || lowerStr == "putty") {
          confidence += 25;
        } else if (lowerStr == "ssh" || lowerStr.find("ssh-") == 0) {
          confidence += 15;
        } else {
          confidence += 10;
        }
      }
    }

    if (stringMatches > 0) {
      findings.push_back("Total SSH-related strings found: " +
                         std::to_string(stringMatches));
    }
  }

  void analyzeImports() {
    // Find import table
    const int IMPORT_TABLE_INDEX = 1;
    if (ntHeaders.OptionalHeader.NumberOfRvaAndSizes <= IMPORT_TABLE_INDEX) {
      return;
    }

    size_t importDirOffset = dosHeader.e_lfanew + sizeof(NT_HEADERS) +
                             sizeof(IMAGE_DATA_DIRECTORY) * IMPORT_TABLE_INDEX;

    if (importDirOffset + sizeof(IMAGE_DATA_DIRECTORY) > fileData.size()) {
      return;
    }

    IMAGE_DATA_DIRECTORY importDir;
    std::memcpy(&importDir, fileData.data() + importDirOffset,
                sizeof(IMAGE_DATA_DIRECTORY));

    if (importDir.VirtualAddress == 0 || importDir.Size == 0) {
      return;
    }

    // Convert RVA to file offset (simplified - assumes sections are loaded)
    uint32_t importOffset = rvaToFileOffset(importDir.VirtualAddress);
    if (importOffset == 0)
      return;

    // Parse import descriptors
    size_t currentOffset = importOffset;
    while (currentOffset + sizeof(IMAGE_IMPORT_DESCRIPTOR) <= fileData.size()) {
      IMAGE_IMPORT_DESCRIPTOR importDesc;
      std::memcpy(&importDesc, fileData.data() + currentOffset,
                  sizeof(IMAGE_IMPORT_DESCRIPTOR));

      if (importDesc.Name == 0)
        break; // End of imports

      uint32_t nameOffset = rvaToFileOffset(importDesc.Name);
      if (nameOffset > 0 && nameOffset < fileData.size()) {
        std::string dllName(
            reinterpret_cast<const char *>(fileData.data() + nameOffset));
        std::transform(dllName.begin(), dllName.end(), dllName.begin(),
                       ::tolower);

        for (const auto &sshLib : sshLibraries) {
          std::string lowerLib = sshLib;
          std::transform(lowerLib.begin(), lowerLib.end(), lowerLib.begin(),
                         ::tolower);

          if (dllName.find(lowerLib) != std::string::npos) {
            findings.push_back("Found SSH-related import: " + dllName);

            if (lowerLib.find("ssl") != std::string::npos ||
                lowerLib.find("crypto") != std::string::npos) {
              confidence += 20;
            } else if (lowerLib.find("ws2_32") != std::string::npos ||
                       lowerLib.find("wininet") != std::string::npos) {
              confidence += 15;
            } else {
              confidence += 10;
            }
            break;
          }
        }
      }

      currentOffset += sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }
  }

  uint32_t rvaToFileOffset(uint32_t rva) {
    for (const auto &section : sectionHeaders) {
      if (rva >= section.VirtualAddress &&
          rva < section.VirtualAddress + section.VirtualSize) {
        return rva - section.VirtualAddress + section.PointerToRawData;
      }
    }
    return 0;
  }

  void additionalHeuristics() {
    // Check for common SSH client characteristics
    std::string fileContent(fileData.begin(), fileData.end());
    std::transform(fileContent.begin(), fileContent.end(), fileContent.begin(),
                   ::tolower);

    // Look for SSH config paths
    std::vector<std::string> configPaths = {
        "/.ssh/config",    "\\.ssh\\config", "ssh_config", "known_hosts",
        "authorized_keys", "id_rsa",         "id_dsa"};

    for (const auto &path : configPaths) {
      if (fileContent.find(path) != std::string::npos) {
        findings.push_back("Found SSH config reference: " + path);
        confidence += 15;
      }
    }

    // Look for SSH protocol strings
    std::vector<std::string> protocolStrings = {
        "ssh-2.0", "ssh-1.", "protocol version", "diffie-hellman",
        "aes",     "3des",   "blowfish"};

    for (const auto &proto : protocolStrings) {
      if (fileContent.find(proto) != std::string::npos) {
        findings.push_back("Found SSH protocol reference: " + proto);
        confidence += 10;
      }
    }

    // Check file size (SSH clients are typically substantial)
    if (fileData.size() > 100000) { // > 100KB
      confidence += 5;
    }
  }

  bool isSSHClient() {
    if (!isPEFormat()) {
      return false;
    }

    readSectionHeaders();
    analyzeStrings();
    analyzeImports();
    additionalHeuristics();

    return confidence >= 50; // Threshold for SSH client detection
  }

  void printAnalysis() {
    std::cout << "\n=== PE SSH Client Analysis ===" << std::endl;
    std::cout << "File size: " << fileData.size() << " bytes" << std::endl;
    std::cout << "Confidence score: " << confidence << "/100" << std::endl;

    std::cout << "\nFindings:" << std::endl;
    for (const auto &finding : findings) {
      std::cout << "  • " << finding << std::endl;
    }

    std::cout << "\nConclusion: ";
    if (confidence >= 80) {
      std::cout << "Very likely an SSH client" << std::endl;
    } else if (confidence >= 50) {
      std::cout << "Possibly an SSH client" << std::endl;
    } else if (confidence >= 20) {
      std::cout << "Unlikely to be an SSH client" << std::endl;
    } else {
      std::cout << "Not an SSH client" << std::endl;
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <PE_file>" << std::endl;
    return 1;
  }

  PESSHDetector detector;

  if (!detector.loadPEFile(argv[1])) {
    return 1;
  }

  bool isSSH = detector.isSSHClient();
  detector.printAnalysis();

  return isSSH ? 0 : 1;
}
