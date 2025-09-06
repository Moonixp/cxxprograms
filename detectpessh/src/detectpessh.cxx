#include "detectpessh.hpp"

PESSHDetector::PESSHDetector() {
  loadDLLMapFromConfig();
  loadSSHMapFromConfig();
}

PESSHDetector::PESSHDetector(std::string dllMapConfigPath,
                             std::string sshMapConfigPath)
    : dllMapFilePath(dllMapConfigPath), sshMapFilePath(sshMapConfigPath) {
  loadDLLMapFromConfig();
  loadSSHMapFromConfig();
}

bool PESSHDetector::fileExists(const std::string &path) {
  return std::filesystem::exists(path);
}

void PESSHDetector::setDefaultDLLMap() {
  sshLibrariesMap = {
      {"ws2_32.dll", 12},   {"wsock32.dll", 12},  {"wininet.dll", 12},
      {"crypt32.dll", 12},  {"advapi32.dll", 12}, {"bcrypt.dll", 12},
      {"libssl", 12},       {"libcrypto", 12},    {"openssl", 12},
      {"libeay32.dll", 12}, {"ssleay32.dll", 12}, {"ncrypt.dll", 12},
      {"cryptsp.dll", 12}};
}
void PESSHDetector::setDefaultSSHMap() {
  sshStringsMap = {
      {"ssh", 25},         {"openssh", 25},     {"putty", 25},
      {"PUTTY", 25},       {"ssh-rsa", 25},     {"ssh-dss", 25},
      {"ssh-ed25519", 15}, {"ecdsa-sha2", 25},  {"id_rsa", 12},
      {"id_dsa", 19},      {"known_hosts", 25}, {"authorized_keys", 12},
      {".ssh", 20},        {"~/.ssh", 20},      {"%USERPROFILE%\\.ssh", 25},
      {"ssh-keygen", 20},  {"ssh-add", 19},     {"ssh-agent", 20},
      {"SecureShell", 20}, {"terminal", 12},    {"sftp", 18},
      {"scp", 20}};
}

// Trim whitespace from both ends of a string
std::string PESSHDetector::trimWhiteSpace(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos)
    return "";

  size_t end = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(start, end - start + 1);
}


// reads map from file and loads it into sshLibrariesMap
void PESSHDetector::loadDLLMapFromConfig() {
  loadMapFromConfig(dllMapFilePath, sshLibrariesMap,
                    [&]() { setDefaultDLLMap(); });
}

// loads the map from a file and puts it into sshStringsMap
void PESSHDetector::loadSSHMapFromConfig() {
  loadMapFromConfig(sshMapFilePath, sshStringsMap,
                    [&]() { setDefaultSSHMap(); });
}

/**
 * Loads a map from a file and puts it into map.
 * If the file does not exist, it sets the default map using the setDeaultMap
 * function If there is an error while reading the file, it sets the default
 * map and writes an error message to cerr.
 *
 * @param filename the name of the file to read from
 * @param map the map to put the data into
 * @param setDefaultMap a function to call if the file does not exist or there
 * is an error while reading the file
 */
void PESSHDetector::loadMapFromConfig(const std::string &filename,
                       std::map<std::string, size_t> &map,
                       std::function<void()> setDefaultMap) {

  if (!fileExists(filename)) {
    setDefaultMap();
    return;
  }

  std::ifstream ifs(filename);
  if (ifs.bad()) {
    std::cout << "File : " << filename
              << " could not be opened\nUsing default Mappings";
    return;
  }

  std::string line{100};

  while (std::getline(ifs, line)) {
    try {

      size_t eq_pos = line.find('=');
      if (eq_pos == std::string::npos)
        throw;
        
      map.emplace(std::make_pair(trimWhiteSpace(line.substr(0, eq_pos)),
                                 std::stoi(trimWhiteSpace(line.substr(eq_pos + 1)))));

    } catch (...) {
      std::cerr << "Error occured with config file, Using default Mappings\n";
      setDefaultMap();
      return;
    }
  }
}

/**
 * Reads a PE file and stores its contents in the PESSHDetector class.
 *
 * @param filename the path to the PE file to read
 * @return true if the file was read successfully, false otherwise
 */
bool PESSHDetector::loadPEFile(const std::string &filename) {
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

bool PESSHDetector::isPEFormat() {
  if (fileData.size() < sizeof(DOS_HEADER)) {
    return false;
  }

  // Check DOS header
  std::memcpy(&dosHeader, fileData.data(), sizeof(DOS_HEADER));
  if (dosHeader.e_magic != 0x5A4D) { // "MZ"
    return false;
  }

  if (dosHeader.e_lfanew >= fileData.size() ||
      dosHeader.e_lfanew + sizeof(NT_HEADERS) >
          fileData.size()) {
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

void PESSHDetector::readSectionHeaders() {
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

void PESSHDetector::analyzeStrings() {
  std::string fileContent(fileData.begin(), fileData.end());
  std::transform(fileContent.begin(), fileContent.end(), fileContent.begin(),
                 ::tolower);

  int stringMatches = 0;
  for (const auto &sshString : sshStringsMap) {
    std::string lowerStr = sshString.first;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   ::tolower);

    if (fileContent.find(lowerStr) != std::string::npos) {
      findings.push_back("Found SSH-related string: " + sshString.first);
      stringMatches++;
      confidence += sshString.second;
    }
  }

  if (stringMatches > 0) {
    findings.push_back("Total SSH-related strings found: " +
                       std::to_string(stringMatches));
  }
}

void PESSHDetector::analyzeImports() {
    // Find import table
    const int IMPORT_TABLE_INDEX = 1;
    if (ntHeaders.OptionalHeader.NumberOfRvaAndSizes <= IMPORT_TABLE_INDEX) {
        return;
    }

    // Get import directory directly from OptionalHeader
    IMAGE_DATA_DIRECTORY importDir = ntHeaders.OptionalHeader.DataDirectory[IMPORT_TABLE_INDEX];
    
    if (importDir.VirtualAddress == 0 || importDir.Size == 0) {
        return;
    }

    // Convert RVA to file offset
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
            // Safer string construction with bounds checking
            const char* namePtr = (const char*)(fileData.data() + nameOffset);
            size_t maxLen = fileData.size() - nameOffset;
            size_t nameLen = strnlen(namePtr, maxLen);
            
            std::string dllName(namePtr, nameLen);
            std::transform(dllName.begin(), dllName.end(), dllName.begin(), ::tolower);

            auto it = sshLibrariesMap.find(dllName);
            if (it != sshLibrariesMap.end()) {
                findings.push_back("Found SSH-related import: " + dllName);
                confidence += it->second;
            }
        }

        currentOffset += sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }
}
/**
 * Convert a Relative Virtual Address (RVA) to a file offset.
 * @param rva Relative Virtual Address to convert.
 * @return File offset of the given RVA, or 0 if the RVA is not valid.
 */
uint32_t PESSHDetector::rvaToFileOffset(uint32_t rva) {
  for (const auto &section : sectionHeaders) {
    if (rva >= section.VirtualAddress &&
        rva < section.VirtualAddress + section.VirtualSize) {
      return rva - section.VirtualAddress + section.PointerToRawData;
    }
  }
  return 0;
}

void PESSHDetector::additionalHeuristics() {
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

bool PESSHDetector::isSSHClient() {
  if (!isPEFormat()) {
    return false;
  }

  readSectionHeaders();
  analyzeStrings();
  analyzeImports();
  additionalHeuristics();

  return confidence >= 50; // Threshold for SSH client detection
}

void PESSHDetector::printAnalysis() {
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
