#pragma once

#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>
#include "tuple-helpers.hpp"

template<typename... Args>
class Csv {
private:
    std::ifstream &inputFile;

    char columnDelimiter;
    char rowDelimiter;
    char escapeSym;

    int currentLine = 0;

    std::string getRow() {
        std::string row;
        std::getline(inputFile, row, rowDelimiter);

        ++currentLine;

        return row;
    }

    std::vector<std::string> getRowCells(const std::string &row) {
        std::vector<std::string> rowCells(1);
        std::string cell;
        bool isEscapeSym = false;

        for (const char &sym: row) {
            if (sym == escapeSym) {
                isEscapeSym = true;
                continue;
            }

            if (sym == columnDelimiter) {
                if (isEscapeSym) {
                    cell += sym;
                    continue;
                }
                rowCells.back() = cell;
                rowCells.emplace_back();
                cell.clear();
                continue;
            }

            cell += sym;
        }

        if (!cell.empty()) {
            rowCells.back() = cell;
        }

        return rowCells;
    };

public:
    class InputIterator;

    explicit Csv(
            std::ifstream &inputFile,
            unsigned int lineOffset = 0,
            char columnDelimiter = ',',
            char lineDelimiter = '\n',
            char escapeSym = '\"') : inputFile(inputFile),
                                     columnDelimiter(columnDelimiter),
                                     rowDelimiter(lineDelimiter),
                                     escapeSym(escapeSym) {
        if (!inputFile.is_open()) {
            throw std::ifstream::failure("File is not open");
        }

        for (; currentLine < lineOffset; ++currentLine) {
            if (!inputFile.ignore(std::numeric_limits<std::streamsize>::max(), inputFile.widen(lineDelimiter))) {
                throw std::ifstream::failure("File does not contain " + std::to_string(lineOffset) + " lines");
            }
        }
    }

    InputIterator begin() {
        return InputIterator(*this, InputIterator::Mode::begin);
    }

    InputIterator end() {
        return InputIterator(*this, InputIterator::Mode::end);
    }
};

template<typename... Args>
class Csv<Args...>::InputIterator {
public:
    using valueType = std::tuple<Args...>;
    using reference = std::tuple<Args...> &;
    using pointer = std::tuple<Args...> *;

    enum class Mode {
        begin,
        end
    };

    InputIterator(Csv &parent, Mode mode) : parent(parent) {
        switch (mode) {
            case Mode::begin:
                updatePtr();
                break;
            case Mode::end:
                ptr = nullptr;
        }
    }

    reference operator*() const {
        return *ptr;
    }

    pointer operator->() {
        return ptr;
    }

    InputIterator &operator++() {
        updatePtr();
        return *this;
    }

    InputIterator operator++(int) {
        InputIterator temp = *this;
        updatePtr();
        return temp;
    }

    friend bool operator==(const InputIterator &a, const InputIterator &b) {
        return a.ptr == b.ptr;
    }

    friend bool operator!=(const InputIterator &a, const InputIterator &b) {
        return a.ptr != b.ptr;
    }

private:
    Csv<Args...> &parent;
    valueType tuple;
    pointer ptr = &tuple;

    void updatePtr() {
        std::string row = parent.getRow();

        if (row.empty() && !parent.inputFile.eof()) {
            throw std::invalid_argument(std::to_string(parent.currentLine) + " row is empty");
        }
        if (row.empty()) {
            ptr = nullptr;
            return;
        }

        std::vector<std::string> rowCells = parent.getRowCells(row);

        if (rowCells.size() > sizeof...(Args)) {
            throw std::invalid_argument("Too many arguments on " + std::to_string(parent.currentLine) + " line");
        }
        if (rowCells.size() < sizeof...(Args)) {
            throw std::invalid_argument("Too few arguments on " + std::to_string(parent.currentLine) + " line");
        }

        try {
            makeTuple<Args...>(ptr, rowCells);
        } catch (const std::invalid_argument &ex) {
            std::string msg = ex.what();
            msg += std::to_string(parent.currentLine) + " row";
            throw std::invalid_argument(msg);
        }
    }
};
