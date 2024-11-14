#include "./LC3/lc3circuit.h"
#include "circuitwrapper.h"
#include "./GlobalDefinitions/circuittype.h"

// private function

template <typename BitsetType>
void circuitWrapper::AssignBitsetFromValue(BitsetType& bitset, const std::string& value) {
    bitset = BitsetType(value);
}

// Function to set the value of a bitset variable by name
void circuitWrapper::SetBitsetValue(const std::string_view& name, const std::string& value) {
    auto it = data_map.find(name);
    if (it != data_map.end()) {
        std::visit([&](auto &&bitset_ptr)
                   {
                       AssignBitsetFromValue(*bitset_ptr, value);

                   }, it->second);
    } else {
        throw std::invalid_argument("Variable name not found.");
    }
}

// public function

int circuitWrapper::InitializeWrapper(std::string_view circuit_type){
    if(circuit_type == CIRCUIT_TYPE_LC3){
        this->data_map = LC3DataMap();
        this->module_map = LC3ModuleMap();
    }
    return 0;
}

Data circuitWrapper::Run(std::string_view func_name){
    // #* Code for Debugging
    // auto temp = module_map[func_name]();
    // std::cout << "circuitwrapper-Run " << temp[0].first << temp[0].second << std::endl;

    return module_map[func_name]();
}

int circuitWrapper::Loader(bitwd new_mem[1<<16]){
    // Load data to the memory
    LC3SetMemory(new_mem);
    return 0;
}

int circuitWrapper::UpdateLine(Data* data){
    for(auto i: (*data)){
        SetBitsetValue(i.first, i.second); // Line name, line value
    }
    return 0;
}

void circuitWrapper::SetPC(int newpc){
    bitwd pcbit{static_cast<unsigned long long>(newpc)};
    LC3SetPC(pcbit);
}

void circuitWrapper::SetClockPositiveEdge(){
    LC3SetClockPositiveEdge();
}

void circuitWrapper::FinishClockPositiveEdge(){
    LC3FinishClockPositiveEdge();
}

bool circuitWrapper::IsTrap(){
    return LC3IsTrap();
}

std::unordered_map<std::string_view, int> circuitWrapper::GetModuleTime(){
    return LC3ModuleTime();
}
std::unordered_map<std::string_view, std::vector<std::string_view>> circuitWrapper::GetNextModule(){
    return LC3NextModule();
}

std::vector<std::string_view> circuitWrapper::GetClockSyncModules(){
    return LC3CLKSyncedModule();
}

int circuitWrapper::GetClockLoopCycle(){
    return LC3CLKLoopCycle();
}

std::string circuitWrapper::GetCircuitStatus(){
    return LC3GetCircuitStatus();
}
