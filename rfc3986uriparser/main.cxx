#include "lib/include/uri_parser.hpp"
#include <fstream>
#include <iostream>
#include <memory>

void print_uri_details(const URI &uri) {
  std::cout << "URI Components:\n";
  std::cout << "Scheme: " << (uri.scheme().empty() ? "(none)" : uri.scheme())
            << "\n";
  std::cout << "Authority: "
            << (uri.authority().empty() ? "(none)" : uri.authority()) << "\n";
  std::cout << "Userinfo: "
            << (uri.userinfo().empty() ? "(none)" : uri.userinfo()) << "\n";
  std::cout << "Host: " << (uri.host().empty() ? "(none)" : uri.host()) << "\n";

  if (uri.port().has_value()) {
    std::cout << "Port: " << uri.port().value() << "\n";
  } else {
    std::cout << "Port: (none)\n";
  }

  std::cout << "Path: " << (uri.path().empty() ? "(none)" : uri.path()) << "\n";
  std::cout << "Query: " << (uri.query().empty() ? "(none)" : uri.query())
            << "\n";
  std::cout << "Fragment: "
            << (uri.fragment().empty() ? "(none)" : uri.fragment()) << "\n";
  std::cout << "Full URI: " << uri.to_string() << "\n";
  std::cout << "----------------------------------------\n";
}

int main(int argc, char *argv[]) {
  URIParser parser;

  try {
    if (argc > 1) {
      for (int i = 1; i < argc; ++i) {
        std::cout << "Parsing: " << argv[i] << "\n";
        auto uri = parser.parse(argv[i]);
        print_uri_details(*uri);
      }
    } else {
      std::cout << "Enter URIs (one per line, Ctrl+D to end)\n";
      std::cout << "URI: ";
      std::string line;

      while (std::getline(std::cin, line)) {
        if (line.empty())
          continue;

        std::cout << "Parsing: " << line << "\n";
        auto uri = parser.parse(line);
        print_uri_details(*uri);
        std::cout << "URI: ";
      }
    }

  } catch (const URIParseException &e) {
    std::cerr << "Error parsing URI: " << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Unexpected error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
