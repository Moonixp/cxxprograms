#ifndef DETECT_PE_SSH__
#define DETECT_PE_SSH__

#include "pe_headers.hpp"
#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

class PESSHDetector {
private:
  std::vector<uint8_t> fileData;
  std::string dllMapFilePath{"config/dllMap.conf"};
  std::string sshMapFilePath{"config/sshMap.conf"};
  DOS_HEADER dosHeader;
  NT_HEADERS ntHeaders;
  std::vector<SECTION_HEADER> sectionHeaders;
  int confidence;
  std::vector<std::string> findings;
  std::map<std::string, size_t> sshStringsMap;
  std::map<std::string, size_t> sshLibrariesMap;

public:

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
  void loadMapFromConfig(const std::string &filename,
                         std::map<std::string, size_t> &map,
                         std::function<void()> setDefaultMap);
  std::string trimWhiteSpace(const std::string &str);
  bool fileExists(const std::string &path);
  void setDefaultDLLMap();
  void setDefaultSSHMap();

  PESSHDetector();
  PESSHDetector(std::string dllMapConfigPath, std::string sshMapConfigPath);
  bool isSSHClient();

  void analyzeImports();
  void loadDLLMapFromConfig();
  void loadSSHMapFromConfig();

  /**
   * Reads a PE file and stores its contents in the PESSHDetector class.
   *
   * @param filename the path to the PE file to read
   * @return true if the file was read successfully, false otherwise
   */
  bool loadPEFile(const std::string &filename);
  bool isPEFormat();
  void readSectionHeaders();
  void analyzeStrings();

  /**
   * Convert a Relative Virtual Address (RVA) to a file offset.
   * @param rva Relative Virtual Address to convert.
   * @return File offset of the given RVA, or 0 if the RVA is not valid.
   */
  uint32_t rvaToFileOffset(uint32_t rva);

  void additionalHeuristics();
  void printAnalysis();
};
#endif
