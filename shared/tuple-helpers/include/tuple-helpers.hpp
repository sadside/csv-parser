#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace {
    template<std::size_t>
    struct index_t {};

    template<typename CharType, typename Traits, class Tuple>
    std::basic_ostream<CharType, Traits> &printTuple(std::basic_ostream<CharType, Traits> &os, const Tuple &tuple, index_t<1>) {
        return os << std::get<std::tuple_size<Tuple>::value - 1>(tuple);
    }

    template<typename CharType, typename Traits, class Tuple>
    std::basic_ostream<CharType, Traits> &printTuple(std::basic_ostream<CharType, Traits> &os, const Tuple &tuple, index_t<0>) {
        return os;
    }

    template<typename CharType, typename Traits, class Tuple, std::size_t Pos>
    std::basic_ostream<CharType, Traits> &printTuple(std::basic_ostream<CharType, Traits> &os, const Tuple &tuple, index_t<Pos>) {
        os << std::get<std::tuple_size<Tuple>::value - Pos>(tuple) << ", ";
        return printTuple(os, tuple, index_t<Pos - 1>());
    }

    template<typename ValueType>
    ValueType readValue(const std::string &cell) {
        std::istringstream stream(cell);
        ValueType value;

        if (!(stream >> value)) {
            throw std::invalid_argument("bad reading type of \"" + cell + "\" at ");
        }

        return value;
    }

    template<>
    std::string readValue<std::string>(const std::string &cell) {
        return cell;
    }

    template<typename... Types>
    void fillTuple(std::tuple<Types...> *tuplePtr, const std::vector<std::string> &rowCells, index_t<1>) {
        try {
            std::get<0>(*tuplePtr) = readValue<typename std::tuple_element<0, std::tuple<Types...>>::type>(rowCells[0]);
        } catch (const std::exception &ex) {
            std::string msg = ex.what();
            msg += std::to_string(1) + " column, ";

            throw std::invalid_argument(msg);
        }
    }

    template<typename... Types, std::size_t N>
    void fillTuple(std::tuple<Types...> *tuplePtr, const std::vector<std::string> &rowCells, index_t<N>) {
        try {
            std::get<N - 1>(*tuplePtr) = readValue<typename std::tuple_element<N - 1, std::tuple<Types...>>::type>(
                    rowCells[N - 1]);
        } catch (const std::invalid_argument &ex) {
            std::string msg = ex.what();
            msg += std::to_string(N) + " column, ";

            throw std::invalid_argument(msg);
        }

        fillTuple<Types...>(tuplePtr, rowCells, index_t<N - 1>());
    }
}

template<typename CharType, typename Traits, typename... Types>
std::basic_ostream<CharType, Traits> &operator<<(std::basic_ostream<CharType, Traits> &os, const std::tuple<Types...> &tuple) {
    return printTuple(os, tuple, index_t<sizeof...(Types)>());
}

template<typename... Types>
void makeTuple(std::tuple<Types...> *tuplePtr, const std::vector<std::string> &rowCells) {
    fillTuple(tuplePtr, rowCells, index_t<sizeof...(Types)>());
}
