#ifndef CIRCUITWRAPPER_H
#define CIRCUITWRAPPER_H
#include "./GlobalDefinitions/typedef.h"
#include "./LC3/lc3circuit.h"

#include <string_view>
#include <bitset>
#include <string>
#include <iostream>
#include <string_view>
#include <vector>
#include <unordered_map>


// #* Code for Debugging
// #include <typeinfo>

class circuitWrapper{
    dataMapType data_map; // data_name - data variable map
    moduleMapType module_map;
    template <typename BitsetType>
    void AssignBitsetFromValue(BitsetType &bitset, const std::string &value);
    // Function to set the value of a bitset variable by name
    void SetBitsetValue(const std::string_view &name, const std::string &value);

public:
    int InitializeWrapper(std::string_view circuit_type);
    Data Run(std::string_view func_name);
    int Loader(bitwd new_mem[1 << 16]);
    int UpdateLine(Data *data);
    void SetPC(int newpc);
    void SetClockPositiveEdge();
    void FinishClockPositiveEdge();
    bool IsTrap();
    std::unordered_map<std::string_view, int> GetModuleTime();
    std::unordered_map<std::string_view, std::vector<std::string_view>> GetNextModule();
    std::vector<std::string_view> GetClockSyncModules();
    int GetClockLoopCycle();
    std::string GetCircuitStatus();
};
#endif
