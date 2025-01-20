#include <fstream>
#include <iostream>
#include <stdexcept>
#include "csv-parser.hpp"
#include "tuple-helpers.hpp"

namespace {
    void printHelp() {
        std::cout << "\n\t\t\tCSV Parser" << std::endl
                  << "\n\t1 argument \t--\t input file path" << std::endl
                  << "\t2 argument \t--\t skip first lines count (default = 0)" << std::endl
                  << "\t3 argument \t--\t custom column delimiter (default = ',')" << std::endl
                  << "\t4 argument \t--\t custom row delimiter (default = '\\n')" << std::endl
                  << "\t5 argument \t--\t custom escape symbol (default = '\"')" << std::endl
                  << std::endl;
    }

    template<typename... Args>
    void processCsv(std::ifstream &inputFile, int lineOffset, char columnDelimiter, char rowDelimiter, char escapeSymbol) {
        Csv<Args...> parser(inputFile, lineOffset, columnDelimiter, rowDelimiter, escapeSymbol);
        for (const auto &row: parser) {
            std::cout << row << std::endl;
        }
    }

    void handleArguments(int argc, const char *argv[], std::ifstream &inputFile, int &lineOffset, char &columnDelimiter, char &rowDelimiter, char &escapeSymbol) {
        if (argc < 2) {
            throw std::invalid_argument("Input file path is required.");
        }
        if (std::string_view(argv[1]) == "--help") {
            printHelp();
            exit(EXIT_SUCCESS);
        }
        if (argc > 6) {
            throw std::invalid_argument("Wrong number of parameters. Use -h or --help for help");
        }

        inputFile.open(argv[1]);
        if (!inputFile.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(argv[1]));
        }

        lineOffset = (argc > 2) ? std::stoi(argv[2]) : 0;
        columnDelimiter = (argc > 3) ? argv[3][0] : ',';
        rowDelimiter = (argc > 4) ? argv[4][0] : '\n';
        escapeSymbol = (argc > 5) ? argv[5][0] : '\"';
    }
}

int main(int argc, char const *argv[]) {
    try {
        std::ifstream inputFile;
        int lineOffset;
        char columnDelimiter, rowDelimiter, escapeSymbol;

        handleArguments(argc, argv, inputFile, lineOffset, columnDelimiter, rowDelimiter, escapeSymbol);
        processCsv<std::string, int, std::string>(inputFile, lineOffset, columnDelimiter, rowDelimiter, escapeSymbol);

    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
