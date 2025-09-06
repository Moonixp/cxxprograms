#include "detectenc.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    std::cerr << "Analyzes a file to detect if it is likely encrypted\n";

    std::flush(std::cout);
    return 1;
  }
  std::unique_ptr<AnalysisResult> result = std::make_unique<AnalysisResult>();
  EncryptionDetector detector(std::move(result));

  if (!detector.loadFile(argv[1])) {
    return 1;
  }

  detector.analyze();
  detector.printDetailedAnalysis();
  return detector.getResult().highCertaintyEncrypted
             ? 0
             : 2; // Return 0 if encrypted, 2 if not
}
