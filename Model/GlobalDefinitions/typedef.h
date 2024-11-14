#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <variant>
#include <bitset>
#include <vector>
#include <unordered_map>
#pragma once

using bitwd = std::bitset<16>;
using Data = std::vector<std::pair<std::string_view, std::string>>;

using BitsetVariant = std::variant<
    std::bitset<1>*,
    std::bitset<2>*,
    std::bitset<3>*,
    std::bitset<16>*
    >;

using dataMapType = std::unordered_map < std::string_view, BitsetVariant >;
using moduleMapType = std::unordered_map<std::string_view, Data (*)()>;
#endif
