#include "runclicked.h"
#include "Model/GlobalDefinitions/typedef.h"
#include "ModelView/consoleglobal.h"

runClicked::runClicked(): is_simulator_made(false){

}

void runClicked::Run(){

    // Make simulator if there is no running simulator
    if(is_simulator_made == false){
        this->simulator.InitializeSimulator("./a.o", CIRCUIT_TYPE_LC3);
        debug_console<<"Simulator Initialized\n";
        is_simulator_made = true;
    }

    stat_console.ClearConsole();
    bool is_simulation_finished = simulator.RunSimulator();

    if(is_simulation_finished == true){
        debug_console<<"Simulator Ended\n";
    }

}
