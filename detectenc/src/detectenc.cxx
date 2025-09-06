#include "detectenc.hpp"

EncryptionDetector::EncryptionDetector(std::unique_ptr<AnalysisResult> Result)
    : result(std::move(Result)) {}

double EncryptionDetector::calculateEntropy(
    const std::vector<unsigned char> data,
    std::map<unsigned char, size_t> frequency) const {
  if (data.empty())
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  double entropy = 0.0;
  size_t totalBytes = data.size();

  for (const auto &pair : frequency) {
    double probability = (double)(pair.second) / totalBytes;
    if (probability > 0) {
      entropy -= probability * log2(probability);
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Inner Entropy calculation took " << duration.count() << "us\n";

  return entropy;
}

double EncryptionDetector::calculateChiSquare(
    const std::vector<unsigned char> data,
    std::map<unsigned char, size_t> frequency) const {

  if (data.empty())
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  double expected = (double)(data.size()) / 256.0;
  double chiSquare = 0.0;

  for (int i = 0; i < 256; i++) {
    unsigned char byte = (unsigned char)(i);
    size_t observed = frequency.count(byte) ? frequency.at(byte) : 0;
    double diff = observed - expected;
    chiSquare += (diff * diff) / expected;
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Inner Chi-Square calculation took " << duration.count()
            << "us\n";

  return chiSquare;
}

double EncryptionDetector::calculateAsciiRatio(
    const std::vector<unsigned char> data) const {

  if (data.empty())
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  size_t printableCount = 0;
  size_t controlCount = 0;

  for (unsigned char byte : data) {
    if (byte >= 32 && byte <= 126) { // Printable ASCII
      printableCount++;
    } else if (byte < 32 || byte == 127) { // Control characters
      controlCount++;
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Inner ASCII Ratio calculation took " << duration.count()
            << "us\n";

  return (double)printableCount / data.size();
}

double EncryptionDetector::calculateVariance(
    const std::vector<unsigned char> data) const {

  if (data.empty())
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  // Single-pass variance calculation using Welford's algorithm
  double mean = 0.0;
  double m2 = 0.0;
  size_t count = 0;

  for (unsigned char byte : data) {
    count++;
    double delta = byte - mean;
    mean += delta / count;
    double delta2 = byte - mean;
    m2 += delta * delta2;
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Inner Variance calculation took " << duration.count() << "us\n";

  return count > 1 ? m2 / count : 0.0;
}
// double EncryptionDetector::calculateVariance(
//     const std::vector<unsigned char> data) const {
//
//   if (data.empty())
//     return 0.0;
//
//   auto start = std::chrono::high_resolution_clock::now();
//
//   double mean = 0.0;
//   for (unsigned char byte : data) {
//     mean += byte;
//   }
//   mean /= data.size();
//
//   double variance = 0.0;
//   for (unsigned char byte : data) {
//     double diff = byte - mean;
//     variance += diff * diff;
//   }
//
//   auto stop = std::chrono::high_resolution_clock::now();
//   auto duration =
//       std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//   std::cout << "Inner Variance calculation took " << duration.count() <<
//   "us\n";
//
//   return variance / data.size();
// }

double EncryptionDetector::calculateRepetitionScore(
    const std::vector<unsigned char> data) const {

  if (data.size() < 4)
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  // For large files, sample instead of checking every pattern
  const size_t MAX_PATTERNS_TO_CHECK = 100000;
  const size_t patternLength = 4;

  std::unordered_map<uint32_t, size_t>
      patterns; // Use hash instead of full pattern
  size_t step = 1;

  // If file is large, sample every nth pattern
  if (data.size() > MAX_PATTERNS_TO_CHECK) {
    step = (data.size() - patternLength + 1) / MAX_PATTERNS_TO_CHECK;
  }

  size_t totalPatterns = 0;

  for (size_t i = 0; i <= data.size() - patternLength; i += step) {
    // Create a hash of the 4-byte pattern instead of storing full pattern
    uint32_t hash = 0;
    hash = (hash << 8) | data[i];
    hash = (hash << 8) | data[i + 1];
    hash = (hash << 8) | data[i + 2];
    hash = (hash << 8) | data[i + 3];

    patterns[hash]++;
    totalPatterns++;
  }

  size_t repeatedPatterns = 0;
  for (const auto &pair : patterns) {
    if (pair.second > 1) {
      repeatedPatterns += pair.second - 1;
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Inner Repetition score took " << duration.count() << "us\n";

  return (double)repeatedPatterns / totalPatterns;
}

// double EncryptionDetector::calculateRepetitionScore(
//     const std::vector<unsigned char> data) const {
//
//   if (data.size() < 4)
//     return 0.0;
//
//   auto start = std::chrono::high_resolution_clock::now();
//
//   std::map<std::vector<unsigned char>, size_t> patterns;
//   size_t patternLength = 4; // Check for 4-byte patterns
//
//   for (size_t i = 0; i <= data.size() - patternLength; i++) {
//     std::vector<unsigned char> pattern(data.begin() + i,
//                                        data.begin() + i + patternLength);
//     patterns[pattern]++;
//   }
//   size_t repeatedPatterns = 0;
//   for (const auto &pair : patterns) {
//     if (pair.second > 1) {
//       repeatedPatterns += pair.second - 1;
//     }
//   }
//
//   auto stop = std::chrono::high_resolution_clock::now();
//   auto duration =
//       std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//   std::cout << "Inner Repetition score took " << duration.count() << "us\n";
//
//   return (double)repeatedPatterns / (data.size() - patternLength + 1);
// }

// double EncryptionDetector::calculateTransitionEntropy(
//     const std::vector<unsigned char> data) const {
//
//   if (data.size() < 2)
//     return 0.0;
//
//   auto start = std::chrono::high_resolution_clock::now();
//
//   std::map<std::pair<unsigned char, unsigned char>, size_t> transitions;
//
//   for (size_t i = 0; i < data.size() - 1; i++) {
//     std::pair<unsigned char, unsigned char> transition = {data[i], data[i +
//     1]}; transitions[transition]++;
//   }
//
//   double entropy = 0.0;
//   size_t totalTransitions = data.size() - 1;
//
//   for (const auto &pair : transitions) {
//     double probability = (double)pair.second / totalTransitions;
//     if (probability > 0) {
//       entropy -= probability * log2(probability);
//     }
//   }
//
//   auto stop = std::chrono::high_resolution_clock::now();
//   auto duration =
//       std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//   std::cout << "Inner Transition entropy took " << duration.count() <<
//   "us\n";
//
//   return entropy;
// }

// Optimized Transition Entropy - Use sampling for large files
double EncryptionDetector::calculateTransitionEntropy(
    const std::vector<unsigned char> data) const {

  if (data.size() < 2)
    return 0.0;

  auto start = std::chrono::high_resolution_clock::now();

  // For large files, sample transitions instead of checking every one
  const size_t MAX_TRANSITIONS = 100000;
  size_t step = 1;

  if (data.size() > MAX_TRANSITIONS) {
    step = data.size() / MAX_TRANSITIONS;
  }

  // Use array instead of map for better cache performance
  // transitions[from][to] = count
  std::array<std::array<size_t, 256>, 256> transitions{};

  size_t totalTransitions = 0;

  for (size_t i = 0; i < data.size() - 1; i += step) {
    transitions[data[i]][data[i + 1]]++;
    totalTransitions++;
  }

  double entropy = 0.0;

  // Calculate entropy from the transition matrix
  for (int from = 0; from < 256; from++) {
    for (int to = 0; to < 256; to++) {
      if (transitions[from][to] > 0) {
        double probability = (double)transitions[from][to] / totalTransitions;
        entropy -= probability * log2(probability);
      }
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Inner Transition entropy took " << duration.count() << "us\n";

  return entropy;
}

bool EncryptionDetector::loadFile(const std::string &filename) {

  if (!std::filesystem::exists(filename)) {
    std::cerr << "Error: File does not exist :'" << filename << "'\n";
    return false;
  }
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Error: Cannot open file '" << filename << "'\n";
    return false;
  }
  auto start = std::chrono::high_resolution_clock::now();

  // Read file into vector
  data.assign(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>());

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  std::cout << "Loaded file in " << duration.count() << "ms\n";
  file.close();

  if (data.empty()) {
    std::cerr << "Error: File is empty\n";
    return false;
  }

  frequency.clear();
  for (unsigned char byte : data) {
    frequency[byte]++;
  }

  return true;
}

void EncryptionDetector::analyze() const {

  // std::vector<std::future<double>> future_vec;
  // std::vector<std::function<double()>> member_funcs;
  // std::vector<std::thread> threads;
  // member_funcs.push_back([this]() { return this->calculateEntropy(); });
  // member_funcs.push_back([this]() { return this->calculateChiSquare(); });
  // member_funcs.push_back([this]() { return this->calculateAsciiRatio(); });
  // member_funcs.push_back([this]() { return this->calculateVariance(); });
  // member_funcs.push_back([this]() { return this->calculateRepetitionScore();
  // }); member_funcs.push_back(
  //     [this]() { return this->calculateTransitionEntropy(); });
  //
  // for (int i = 0; i < member_funcs.size(); ++i) {
  //   std::promise<double> p;
  //   future_vec.push_back(p.get_future());
  //   threads.emplace_back(
  //       [this, p = std::move(p), j = i, &member_funcs]() mutable {
  //         try {
  //           double res = member_funcs[j]();
  //           p.set_value(res);
  //         } catch (...) {
  //           p.set_exception(std::current_exception());
  //         }
  //       });
  // }
  //
  // for (size_t i = 0; i < threads.size(); ++i) {
  //   threads[i].join();
  //   std::cout << "Task " << i << " result: " << future_vec[i].get()
  //             << std::endl;
  // }

  // result->entropy = calculateEntropy();
  // result->chiSquare = calculateChiSquare();
  // result->asciiRatio = calculateAsciiRatio();
  // result->variance = calculateVariance();
  // result->repetitionScore = calculateRepetitionScore();
  // result->transitionEntropy = calculateTransitionEntropy();

  /*
  std::promise<double> promise_entropy;
  std::promise<double> promise_chi_square;
  std::promise<double> promise_ascii_ratio;
  std::promise<double> promise_variance;
  std::promise<double> promise_repetition_score;
  std::promise<double> promise_transition_entropy;

  std::future<double> future_entropy = promise_entropy.get_future();
  std::future<double> future_chi_square = promise_chi_square.get_future();
  std::future<double> future_ascii_ratio = promise_ascii_ratio.get_future();
  std::future<double> future_variance = promise_variance.get_future();
  std::future<double> future_repetition_score =
      promise_repetition_score.get_future();
  std::future<double> future_transition_entropy =
      promise_transition_entropy.get_future();

  std::thread thread_entropy([this, p = std::move(promise_entropy), data = data,
                              frequency = frequency]() mutable {
    p.set_value(this->calculateEntropy(data, frequency));
  });

  std::thread thread_chi_square([this, p = std::move(promise_chi_square),
                                 data = this->data,
                                 frequency = this->frequency]() mutable {
    p.set_value(this->calculateChiSquare(data, frequency));
  });

  std::thread thread_ascii_ratio(
      [this, p = std::move(promise_ascii_ratio), data = this->data]() mutable {
        p.set_value(this->calculateAsciiRatio(data));
      });

  std::thread thread_variance(
      [this, p = std::move(promise_variance), data = this->data]() mutable {
        p.set_value(this->calculateVariance(data));
      });

  std::thread thread_repetition_score([this,
                                       p = std::move(promise_repetition_score),
                                       data = this->data]() mutable {
    p.set_value(this->calculateRepetitionScore(data));
  });

  std::thread thread_transition_entropy(
      [this, p = std::move(promise_transition_entropy),
       data = this->data]() mutable {
        p.set_value(this->calculateTransitionEntropy(data));
      });

  thread_entropy.join();
  thread_chi_square.join();
  thread_ascii_ratio.join();
  thread_variance.join();
  thread_repetition_score.join();
  thread_transition_entropy.join();

  result->entropy = future_entropy.get();
  result->chiSquare = future_chi_square.get();
  result->asciiRatio = future_ascii_ratio.get();
  result->variance = future_variance.get();
  result->repetitionScore = future_repetition_score.get();
  result->transitionEntropy = future_transition_entropy.get();
  */

  auto future_entropy = std::async(std::launch::async, [this]() {
    return this->calculateEntropy(this->data, this->frequency);
  });
  auto future_chi_square = std::async(std::launch::async, [this]() {
    return this->calculateChiSquare(this->data, this->frequency);
  });

  auto future_ascii_ratio = std::async(std::launch::async, [this]() {
    return this->calculateAsciiRatio(this->data);
  });

  auto future_variance = std::async(std::launch::async, [this]() {
    return this->calculateVariance(this->data);
  });

  auto future_repetition_score = std::async(std::launch::async, [this]() {
    return this->calculateRepetitionScore(this->data);
  });

  auto future_transition_entropy = std::async(std::launch::async, [this]() {
    return this->calculateTransitionEntropy(this->data);
  });

  result->entropy = future_entropy.get();
  result->chiSquare = future_chi_square.get();
  result->asciiRatio = future_ascii_ratio.get();
  result->variance = future_variance.get();
  result->repetitionScore = future_repetition_score.get();
  result->transitionEntropy = future_transition_entropy.get();

  double score = 0.0;

  // High entropy (close to 8.0 bits) suggests encryption
  if (result->entropy > 7.5)
    score += 30;
  else if (result->entropy > 7.0)
    score += 20;
  else if (result->entropy > 6.0)
    score += 10;

  // Chi-square test: encrypted data should be close to uniform distribution
  // Lower chi-square values indicate more uniform distribution
  if (result->chiSquare < 300)
    score += 25;
  else if (result->chiSquare < 500)
    score += 15;
  else if (result->chiSquare < 1000)
    score += 5;

  // Low ASCII ratio suggests binary/encrypted data
  if (result->asciiRatio < 0.1)
    score += 15;
  else if (result->asciiRatio < 0.3)
    score += 10;
  else if (result->asciiRatio < 0.5)
    score += 5;

  // High variance suggests good distribution of byte values
  if (result->variance > 5000)
    score += 10;
  else if (result->variance > 3000)
    score += 5;

  // Low repetition score suggests encrypted data
  if (result->repetitionScore < 0.01)
    score += 10;
  else if (result->repetitionScore < 0.05)
    score += 5;

  // High transition entropy suggests randomness
  if (result->transitionEntropy > 10)
    score += 10;
  else if (result->transitionEntropy > 8)
    score += 5;

  result->confidenceScore = score;
  result->highCertaintyEncrypted = (score >= 70);
}

void EncryptionDetector::printDetailedAnalysis() const {
  std::cout << std::fixed << std::setprecision(4);

  std::cout << "\n=== Encryption Detection Analysis ===\n";
  std::cout << "File size: " << data.size() << " bytes\n";
  std::cout << "Unique bytes: " << frequency.size() << "/256\n";
  std::cout << "\nStatistical Metrics:\n";
  std::cout << "  Shannon Entropy: " << result->entropy << "/8.0\n";
  std::cout << "  Chi-Square: " << result->chiSquare << '\n';
  std::cout << "  ASCII Ratio: " << result->asciiRatio * 100 << "%\n";
  std::cout << "  Byte Variance: " << result->variance << '\n';
  std::cout << "  Repetition Score: " << result->repetitionScore * 100 << "%\n";
  std::cout << "  Transition Entropy: " << result->transitionEntropy << '\n';
  std::cout << "\nAnalysis Score: " << result->confidenceScore << "/100\n";
  std::flush(std::cout);

  if (result->highCertaintyEncrypted) {
    std::cout << "*** HIGH CERTAINTY: File appears to be ENCRYPTED ***\n";
    std::cout << "Confidence: " << result->confidenceScore << "%\n";
  } else {
    std::cout << "\n*** File does NOT appear to be encrypted ***\n";
    if (result->confidenceScore > 40) {
      std::cout
          << "Note: Some encryption indicators present but below threshold\n";
    }
  }
}

AnalysisResult EncryptionDetector::getResult() const {
  return AnalysisResult(*result);
}
