#include "lc3circuit.h"
#include "lc3module.cpp"

using std::bitset;

namespace LC3controlLine{
bool is_clock_positive_edge;
bitset<1> regwrite, mux1, mux2, mux4, mux5, istrap;
bitset<2> alucontrol, signextendcontrol, statuscontrol;
bitset<3> branch;
};

namespace LC3localLine{
bitset<1> bcval;
bitset<3> statval;
bitset<16> addr, imemval, sr1, sr2, sxval, aluval, dmemval, addval, newaddr;
bitset<16> mux1val, mux2val, mux3val, mux5val;
};

pC pc{};
mEmory inst_memory{}, data_memory{};
mUX2<16> mux1{}, mux2{}, mux3{}, mux4{}, mux5{};
bitWDAdder add{};
branchController branch_controller{};
aLU alu{};
dEcoder decoder{};
rEgFile register_file{};
signExtend signextend{};
statusController status_controller{};
trapDetector trap_detector{};

/* Functions needed to make code shorter */

/* Slice string in [start_ind, end_ind)
*/
std::string SliceString(const std::string& str, int start_ind, int end_ind){
    std::string ret{""};
    for (int i = start_ind; i < end_ind; i++){
        // #*
        // std::cerr << str << ' ' << i << ' ' << str[i] << std::endl;

        ret += str[i];
    }
    return ret;
}

/* Connection wire and modules */
Data RunPC(){
    bitwd pcval = pc.Run(LC3controlLine::is_clock_positive_edge, LC3localLine::newaddr);

    // #* Code for Debugging
    // std::cout << "newpcaddress" << pcval << std::endl;

    return Data({{"addr", pcval.to_string()}});
}

Data RunInstMemory(){
    // #* Code for Debugging
    // std::cout << "pcaddress" << LC3localLine::addr << std::endl;

    bitwd imemval = inst_memory.Run(LC3localLine::addr);

    // #*
    // std::cout << "instruction" << imemval << ' ' << LC3localLine::addr << std::endl;

    return Data({{"imemval", imemval.to_string()}});
}

Data RunDataMemory(){
    bitwd dmemval = data_memory.Run(LC3localLine::aluval);
    return Data({{"dmemval", dmemval.to_string()}});
}

Data RunMux1(){
    bitwd mux1val = mux1.Run(LC3controlLine::mux1, LC3localLine::sxval, LC3localLine::sr2);
    return Data({{"mux1val", mux1val.to_string()}});
}

Data RunMux2(){
    bitwd mux2val = mux2.Run(LC3controlLine::mux2, LC3localLine::aluval, LC3localLine::dmemval);
    return Data({{"mux2val", mux2val.to_string()}});
}

Data RunMux3(){
    bitwd mux3val = mux3.Run(LC3localLine::bcval, LC3localLine::aluval, LC3localLine::addval);
    return Data({{"mux3val", mux3val.to_string()}});
}

Data RunMux4(){
    bitwd newaddr = mux4.Run(LC3controlLine::mux4, LC3localLine::mux3val, LC3localLine::sr1);

    // #*
    // std::cerr << newaddr << "mux4" << std::endl;

    return Data({{"newaddr", newaddr.to_string()}});
}

Data RunMux5(){
    bitwd mux5val = mux5.Run(LC3controlLine::mux5, LC3localLine::sr1, LC3localLine::addr);
    return Data({{"mux5val", mux5val.to_string()}});
}

Data RunAdd(){
    bitwd addval = add.Run(bitwd{1}, LC3localLine::addr);

    // #* Code for Debugging
    // std::cout << "ADDVAL " << addval.to_string() << std::endl;

    return Data({{"addval", addval.to_string()}});
}

Data RunBranchController(){
    bitset<1> bcval = branch_controller.Run(LC3localLine::statval, LC3controlLine::branch);

    // #* Code for Debugging
    // std::cout << "bc " << bcval.to_string() << std::endl;

    return Data({{"bcval", bcval.to_string()}});
}

Data RunALU(){
    bitwd aluval = alu.Run(LC3controlLine::alucontrol, LC3localLine::mux5val, LC3localLine::mux1val);
    return Data({{"aluval", aluval.to_string()}});
}

Data RunDecoder(){
    auto decoderval = decoder.Run(LC3localLine::imemval).to_string();

    // #* Code for Debugging
    // std::cerr << 'x'<< SliceString(decoderval, 0, 7) << ' '<< decoderval << std::endl;

    return Data({{"regwrite", SliceString(decoderval, 0, 1)},
                 {"branch", SliceString(decoderval, 1, 4)},
                 {"mux1", SliceString(decoderval, 4, 5)},
                 {"mux2", SliceString(decoderval, 5, 6)},
                 {"alucontrol", SliceString(decoderval, 6, 8)},
                 {"mux4", SliceString(decoderval, 8, 9)},
                 {"mux5", SliceString(decoderval, 9, 10)},
                 {"signextendcontrol", SliceString(decoderval, 10, 12)},
                 {"statuscontrol", SliceString(decoderval, 12, 14)}});
}

Data RunRegFile(){
    bitset<3> drnum{SliceString(LC3localLine::imemval.to_string(), 4, 7)},
        sr1num{SliceString(LC3localLine::imemval.to_string(), 7, 10)},
        sr2num{SliceString(LC3localLine::imemval.to_string(), 13, 16)};

    // #*
    // std::cerr << "*RunRegFile" << drnum << ' ' << sr1num << ' ' << sr2num << std::endl;

    auto regfileval = register_file.Run(LC3controlLine::is_clock_positive_edge, LC3controlLine::regwrite,
                                        drnum, sr1num, sr2num, LC3localLine::mux2val);
    // 이부분 drnum, sr1num, sr2num 수정 필요하다
    return Data({{"sr1", regfileval.first.to_string()},
                 {"sr2", regfileval.second.to_string()}});
}

Data RunSignextend(){
    bitwd sxval = signextend.Run(LC3controlLine::signextendcontrol, LC3localLine::imemval);
    return Data({{"sxval", sxval.to_string()}});
}

Data RunStatusController(){
    bitset<3> statval = status_controller.Run(LC3controlLine::is_clock_positive_edge, LC3controlLine::statuscontrol,
                                              LC3localLine::aluval, LC3localLine::dmemval);
    return Data({{"statval", statval.to_string()}});
}

Data RunTrapDetector(){
    bitset<1> istrapval = trap_detector.Run(LC3localLine::imemval);
    return Data({{"istrap", istrapval.to_string()}});
}
/*
part needed for circuit wrapper
*/

void LC3SetClockPositiveEdge(){
    LC3controlLine::is_clock_positive_edge = true;
}

void LC3FinishClockPositiveEdge(){
    LC3controlLine::is_clock_positive_edge = false;
}

bool LC3IsTrap(){
    return bool(LC3controlLine::istrap.to_ulong());
}

void LC3SetMemory(bitwd new_mem[1<<16]){

    // #* Code for Debugging
    // std::cout << new_mem[12289];
    auto temp = bitwd(12289);

    inst_memory.SetMemory(new_mem);
    data_memory.SetMemory(new_mem);

    // #* Code for Debugging
    // std::cout << 'x' << inst_memory.Run(temp) << std::endl;;
}

int LC3CLKLoopCycle(){
    return 7;
}

void LC3SetPC(bitwd &newpc){
    pc.Run(true, newpc);
}

// function that print current status of circuit
std::string LC3GetCircuitStatus(){
    auto registers = register_file.GetRegisterVal();
    char ret[111];
    std::sprintf(ret, "pc[%d]\n", pc.GetPc());
    for (int i = 0; i < 8; i++){
        sprintf(ret + sizeof(char) * strlen(ret), "R%d[%d] ", i, registers[i]);
    }
    return std::string(ret);
}

// Define a variant type to hold pointers to different bitset sizes
std::unordered_map<std::string_view, BitsetVariant> LC3DataMap(){

    // Map variable names to their corresponding bitset pointers
    std::unordered_map<std::string_view, BitsetVariant> data_map = {
        // controlLine namespace
        {"regwrite", &LC3controlLine::regwrite},
        {"mux1", &LC3controlLine::mux1},
        {"mux2", &LC3controlLine::mux2},
        {"mux4", &LC3controlLine::mux4},
        {"mux5", &LC3controlLine::mux5},
        {"alucontrol", &LC3controlLine::alucontrol},
        {"signextendcontrol", &LC3controlLine::signextendcontrol},
        {"statuscontrol", &LC3controlLine::statuscontrol},
        {"branch", &LC3controlLine::branch},
        {"istrap", &LC3controlLine::istrap},

        // localLine namespace
        {"bcval", &LC3localLine::bcval},
        {"statval", &LC3localLine::statval},
        {"addr", &LC3localLine::addr},
        {"imemval", &LC3localLine::imemval},
        {"sr1", &LC3localLine::sr1},
        {"sr2", &LC3localLine::sr2},
        {"sxval", &LC3localLine::sxval},
        {"aluval", &LC3localLine::aluval},
        {"dmemval", &LC3localLine::dmemval},
        {"addval", &LC3localLine::addval},
        {"newaddr", &LC3localLine::newaddr},
        {"mux1val", &LC3localLine::mux1val},
        {"mux2val", &LC3localLine::mux2val},
        {"mux3val", &LC3localLine::mux3val},
        {"newaddr", &LC3localLine::newaddr},
        {"mux5val", &LC3localLine::mux5val}
    };
    return data_map;
}

std::unordered_map<std::string_view, Data (*)()> LC3ModuleMap(){
    std::unordered_map<std::string_view, Data (*)()> module_map = {
        {"RunPC", &RunPC},
        {"RunInstMemory", &RunInstMemory},
        {"RunDataMemory", &RunDataMemory},
        {"RunMux1", &RunMux1},
        {"RunMux2", &RunMux2},
        {"RunMux3", &RunMux3},
        {"RunMux4", &RunMux4},
        {"RunMux5", &RunMux5},
        {"RunAdd", &RunAdd},
        {"RunBranchController", &RunBranchController},
        {"RunALU", &RunALU},
        {"RunDecoder", &RunDecoder},
        {"RunRegFile", &RunRegFile},
        {"RunSignextend", &RunSignextend},
        {"RunStatusController", &RunStatusController},
        {"RunTrapDetector", &RunTrapDetector}
    };
    return module_map;
}

std::unordered_map<std::string_view, int> LC3ModuleTime(){
    std::unordered_map<std::string_view, int> module_time{
        {"RunPC", 1},
        {"RunInstMemory", 1},
        {"RunDataMemory", 1},
        {"RunMux1", 1},
        {"RunMux2", 1},
        {"RunMux3", 1},
        {"RunMux4", 1},
        {"RunMux5", 1},
        {"RunAdd", 1},
        {"RunBranchController", 1},
        {"RunALU", 1},
        {"RunDecoder", 1},
        {"RunRegFile", 1},
        {"RunSignextend", 1},
        {"RunStatusController", 1},
        {"RunTrapDetector", 1}
    };
    return module_time;
}

std::unordered_map<std::string_view, std::vector<std::string_view>> LC3NextModule(){
    std::unordered_map<std::string_view, std::vector<std::string_view>> next_module{
        {"RunPC", {"RunInstMemory", "RunAdd"}},
        {"RunInstMemory", {"RunDecoder", "RunRegFile", "RunSignextend", "RunTrapDetector"}},
        {"RunDataMemory", {"RunStatusController", "RunMux2"}},
        {"RunMux1", {"RunALU"}},
        {"RunMux2", {"RunRegFile"}},
        {"RunMux3", {"RunMux4"}},
        {"RunMux4", {"RunPC"}},
        {"RunMux5", {"RunALU"}},
        {"RunAdd", {"RunMux3"}},
        {"RunBranchController", {"RunMux3"}},
        {"RunALU", {"RunDataMemory", "RunStatusController", "RunMux2", "RunMux3"}},
        {"RunDecoder", {"RunSignextend", "RunStatusController", "RunBranchController",
                        "RunALU", "RunMux1", "RunMux2", "RunMux4", "RunMux5", "RunRegFile"}},
        {"RunRegFile", {"RunMux1", "RunMux5"}},
        {"RunSignextend", {"RunMux1"}},
        {"RunStatusController", {"RunBranchController"}},
        {"RunTrapDetector", {}}
    };
    return next_module;
}

std::vector<std::string_view> LC3CLKSyncedModule(){
    std::vector<std::string_view> clock_synced_module{
        "RunPC", "RunRegFile", "RunStatusController"
    };
    return clock_synced_module;
}
