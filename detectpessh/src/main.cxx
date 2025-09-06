#include "detectpessh.hpp"

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
