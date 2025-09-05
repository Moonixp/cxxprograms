#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class EncryptionDetector {
private:
  std::vector<unsigned char> data;
  std::map<unsigned char, size_t> frequency;

  //  Shannon entropy
  double calculateEntropy() const {
    if (data.empty())
      return 0.0;

    double entropy = 0.0;
    size_t totalBytes = data.size();

    for (const auto &pair : frequency) {
      double probability = (double)(pair.second) / totalBytes;
      if (probability > 0) {
        entropy -= probability * log2(probability);
      }
    }

    return entropy;
  }

  // Calculate chi-square test for uniformity
  double calculateChiSquare() const {
    if (data.empty())
      return 0.0;

    double expected = static_cast<double>(data.size()) / 256.0;
    double chiSquare = 0.0;

    for (int i = 0; i < 256; i++) {
      unsigned char byte = static_cast<unsigned char>(i);
      size_t observed = frequency.count(byte) ? frequency.at(byte) : 0;
      double diff = observed - expected;
      chiSquare += (diff * diff) / expected;
    }

    return chiSquare;
  }

  // Check for ASCII text patterns (low values would suggest plaintext)
  double calculateAsciiRatio() const {
    if (data.empty())
      return 0.0;

    size_t printableCount = 0;
    size_t controlCount = 0;

    for (unsigned char byte : data) {
      if (byte >= 32 && byte <= 126) { // Printable ASCII
        printableCount++;
      } else if (byte < 32 || byte == 127) { // Control characters
        controlCount++;
      }
    }

    return static_cast<double>(printableCount) / data.size();
  }

  // Calculate byte value distribution variance
  double calculateVariance() const {
    if (data.empty())
      return 0.0;

    double mean = 0.0;
    for (unsigned char byte : data) {
      mean += byte;
    }
    mean /= data.size();

    double variance = 0.0;
    for (unsigned char byte : data) {
      double diff = byte - mean;
      variance += diff * diff;
    }

    return variance / data.size();
  }

  // Check for repeated patterns (encrypted data should have minimal repetition)
  double calculateRepetitionScore() const {
    if (data.size() < 4)
      return 0.0;

    std::map<std::vector<unsigned char>, size_t> patterns;
    size_t patternLength = 4; // Check for 4-byte patterns

    for (size_t i = 0; i <= data.size() - patternLength; i++) {
      std::vector<unsigned char> pattern(data.begin() + i,
                                         data.begin() + i + patternLength);
      patterns[pattern]++;
    }

    size_t repeatedPatterns = 0;
    for (const auto &pair : patterns) {
      if (pair.second > 1) {
        repeatedPatterns += pair.second - 1;
      }
    }

    return static_cast<double>(repeatedPatterns) /
           (data.size() - patternLength + 1);
  }

  // Analyze byte transitions for randomness
  double calculateTransitionEntropy() const {
    if (data.size() < 2)
      return 0.0;

    std::map<std::pair<unsigned char, unsigned char>, size_t> transitions;

    for (size_t i = 0; i < data.size() - 1; i++) {
      std::pair<unsigned char, unsigned char> transition = {data[i],
                                                            data[i + 1]};
      transitions[transition]++;
    }

    double entropy = 0.0;
    size_t totalTransitions = data.size() - 1;

    for (const auto &pair : transitions) {
      double probability = static_cast<double>(pair.second) / totalTransitions;
      if (probability > 0) {
        entropy -= probability * log2(probability);
      }
    }

    return entropy;
  }

public:
  bool loadFile(const std::string &filename) {
    if (!std::filesystem::exists(filename)) {
      std::cerr << "Error: File does not exist :'" << filename << "'"
                << std::endl;
      return false;
    }
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
      return false;
    }

    // Read file into vector
    data.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
    file.close();

    if (data.empty()) {
      std::cerr << "Error: File is empty" << std::endl;
      return false;
    }

    frequency.clear();
    for (unsigned char byte : data) {
      frequency[byte]++;
    }

    return true;
  }

  struct AnalysisResult {
    double entropy;
    double chiSquare;
    double asciiRatio;
    double variance;
    double repetitionScore;
    double transitionEntropy;
    bool highCertaintyEncrypted;
    double confidenceScore;
  };

  AnalysisResult analyze() const {
    AnalysisResult result;

    result.entropy = calculateEntropy();
    result.chiSquare = calculateChiSquare();
    result.asciiRatio = calculateAsciiRatio();
    result.variance = calculateVariance();
    result.repetitionScore = calculateRepetitionScore();
    result.transitionEntropy = calculateTransitionEntropy();

    // Scoring system for encryption detection
    double score = 0.0;

    // High entropy (close to 8.0 bits) suggests encryption
    if (result.entropy > 7.5)
      score += 30;
    else if (result.entropy > 7.0)
      score += 20;
    else if (result.entropy > 6.0)
      score += 10;

    // Chi-square test: encrypted data should be close to uniform distribution
    // Lower chi-square values indicate more uniform distribution
    if (result.chiSquare < 300)
      score += 25;
    else if (result.chiSquare < 500)
      score += 15;
    else if (result.chiSquare < 1000)
      score += 5;

    // Low ASCII ratio suggests binary/encrypted data
    if (result.asciiRatio < 0.1)
      score += 15;
    else if (result.asciiRatio < 0.3)
      score += 10;
    else if (result.asciiRatio < 0.5)
      score += 5;

    // High variance suggests good distribution of byte values
    if (result.variance > 5000)
      score += 10;
    else if (result.variance > 3000)
      score += 5;

    // Low repetition score suggests encrypted data
    if (result.repetitionScore < 0.01)
      score += 10;
    else if (result.repetitionScore < 0.05)
      score += 5;

    // High transition entropy suggests randomness
    if (result.transitionEntropy > 10)
      score += 10;
    else if (result.transitionEntropy > 8)
      score += 5;

    result.confidenceScore = score;
    result.highCertaintyEncrypted = (score >= 70);

    return result;
  }

  void printDetailedAnalysis(const AnalysisResult &result) const {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n=== Encryption Detection Analysis ===" << std::endl;
    std::cout << "File size: " << data.size() << " bytes" << std::endl;
    std::cout << "Unique bytes: " << frequency.size() << "/256" << std::endl;
    std::cout << "\nStatistical Metrics:" << std::endl;
    std::cout << "  Shannon Entropy: " << result.entropy << "/8.0" << std::endl;
    std::cout << "  Chi-Square: " << result.chiSquare << std::endl;
    std::cout << "  ASCII Ratio: " << result.asciiRatio * 100 << "%"
              << std::endl;
    std::cout << "  Byte Variance: " << result.variance << std::endl;
    std::cout << "  Repetition Score: " << result.repetitionScore * 100 << "%"
              << std::endl;
    std::cout << "  Transition Entropy: " << result.transitionEntropy
              << std::endl;

    std::cout << "\nAnalysis Score: " << result.confidenceScore << "/100"
              << std::endl;

    if (result.highCertaintyEncrypted) {
      std::cout << "\n*** HIGH CERTAINTY: File appears to be ENCRYPTED ***"
                << std::endl;
      std::cout << "Confidence: " << result.confidenceScore << "%" << std::endl;
    } else {
      std::cout << "\n*** File does NOT appear to be encrypted ***"
                << std::endl;
      if (result.confidenceScore > 40) {
        std::cout
            << "Note: Some encryption indicators present but below threshold"
            << std::endl;
      }
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    std::cerr << "Analyzes a file to detect if it is likely encrypted"
              << std::endl;
    return 1;
  }

  EncryptionDetector detector;

  if (!detector.loadFile(argv[1])) {
    return 1;
  }

  auto result = detector.analyze();
  detector.printDetailedAnalysis(result);

  return result.highCertaintyEncrypted ? 0
                                       : 2; // Return 0 if encrypted, 2 if not
}
