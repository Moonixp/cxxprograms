#ifndef DETECT_ENC_H_
#define DETECT_ENC_H_
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
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

class EncryptionDetector {
private:
  std::vector<unsigned char> data;
  std::map<unsigned char, size_t> frequency;
  std::unique_ptr<AnalysisResult> result;

  /**
   * @brief how much the data is like random noise
   * @return 0.0 if not random, close to 8.0 if really random
   *
   * this is a measure of how mixed up the bytes are. if all the bytes are the
   * same, this is 0.0. if all the bytes are evenly distributed, this is 8.0.
   * most data is somewhere in between.
   */
  double calculateEntropy(const std::vector<unsigned char> data,
                          std::map<unsigned char, size_t> frequency) const;

  /**
   * @brief calculate how "random" the data is
   * @return how much information is in the data, in bits
   *
   * the more random the data is, the higher the value. the less random the
   * data is, the lower the value. For example, a string of all the same
   * character will have a value of 0, while a string of random characters will
   * have a value close to 8.
   */
  double calculateChiSquare(const std::vector<unsigned char> data,
                            std::map<unsigned char, size_t> frequency) const;

  // check for ascii text patterns (low values would suggest plaintext)
  double calculateAsciiRatio(const std::vector<unsigned char> data) const;

  /**
   * @brief calculate how much the bytes vary from the mean
   * @return a measure of how spread out the bytes are from the mean
   *
   * the more spread out the bytes are from the mean, the higher the value.
   * the less spread out the bytes are from the mean, the lower the value.
   */
  double calculateVariance(const std::vector<unsigned char> data) const;

  /**
   * @brief calculate how much the bytes repeat patterns
   * @return a measure of how repetitive the bytes are
   *
   * encrypted data should have minimal repetition, so the higher this value,
   * the more likely the data is encrypted.
   */
  double calculateRepetitionScore(const std::vector<unsigned char> data) const;

  /**
   * calculate how random the transitions between bytes are
   * this is a measure of how much the data looks like a random sequence.
   * if the data is truly random, each byte should have an equal chance of
   * being followed by any other byte. if the data is not random, certain
   * byte transitions should be more likely than others.
   */
  double
  calculateTransitionEntropy(const std::vector<unsigned char> data) const;

public:
  EncryptionDetector(std::unique_ptr<AnalysisResult> Result);
  ~EncryptionDetector() = default;
  /**
   * @brief Analyze the data and calculate a score indicating the likelihood it
   * is encrypted
   * @return a struct containing the results of the analysis
   *
   * The analysis score is a measure of how likely the data is to be encrypted.
   * The higher the score, the more likely the data is encrypted.
   *
   * The score is based on the following metrics:
   *   - entropy: How much the data looks like random noise
   *   - chi-square test: How close the data is to a uniform distribution
   *   - ascii ratio: How much of the data is printable ascii characters
   *   - variance: How spread out the byte values are from the mean
   *   - repetition score: How much the data repeats patterns
   *   - transition entropy: How much the data looks like a random sequence
   *
   * The high certainty encrypted flag is set if the score is 70 or higher.
   */
  void analyze() const;
  bool loadFile(const std::string &filename);
  void printDetailedAnalysis() const;
  AnalysisResult getResult() const;
};

#endif
