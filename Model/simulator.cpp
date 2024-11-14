#include "simulator.h"
#include "../ModelView/consoleglobal.h"

// private function

bool sImulator::IsClockSyncModule(std::string_view module_name){
    bool ret = false;
    for(auto i: clock_sync_modules)
        if(i == module_name)
            ret = true;

    return ret;
}

// public function

int sImulator::InitializeSimulator(std::string file_name, std::string_view circuit_type){   // Need Modification if not LC3
    this->circuit_type = circuit_type;

    if(this->circuit_type == CIRCUIT_TYPE_LC3){
        this->circuit.InitializeWrapper(circuit_type);

        // set circuit cycle to 0
        this->cycle = 0;

        // Load ByteCode File To LC3
        std::bitset<16> memory[1 << 16];
        int pc0 = LC3BinaryCodeGen(file_name, memory);
        this->circuit.Loader(memory);

        this->circuit.SetPC(pc0);

        // #* code for debugging
        // for (int i = 0; i < 10; i++)
        //     std::cout << memory[i + pc0].to_string() << std::endl;

        this->module_time = this->circuit.GetModuleTime();
        this->next_module = this->circuit.GetNextModule();
        while(!this->data_buffer.empty())
            this->data_buffer.pop();
        this->CLKLoopLength = this->circuit.GetClockLoopCycle();
        this->clock_sync_modules = this->circuit.GetClockSyncModules();
    }
    return 0;
}

// Return value 1 means program ended with halt instruction
// run one standard time (cycle - !different from clock cycle!)
int sImulator::RunSimulator(){
    if (!this->circuit.IsTrap()){

        // #* Code for Debugging
        // std::cerr << data_buffer.size() << std::endl;

        /* Update Register Values If Clock Is Positive */
        if(cycle % CLKLoopLength == 0){
            // Clock cycle >= critical path 이므로 clock cycle에 도달했을 때는 모든 모듈에서 추가적으로 업데이트가 발생하지 않음
            // 따라서 buffer를 비워도 된다
            while(!data_buffer.empty())
                data_buffer.pop();

            if(cycle != 0)
                this->circuit.SetClockPositiveEdge(); // should not update if first cycle
            for (auto i : clock_sync_modules){
                // std::cout << i << ' ';
                data_buffer.push(
                    std::make_pair(-cycle - module_time[i], std::make_pair(i, this->circuit.Run(i))));
            }
            if(cycle != 0)
                this->circuit.FinishClockPositiveEdge();
        }

        // #* Code for Debugging
        // std::cout << "a" << std::endl;

        /* Store Following Module and Update Line Data */
        std::vector<std::string_view> temp_new_modules;

        // Print Circuit Status;
        stat_console << "\n===Time" << cycle << "===" << "\n";
        stat_console << "Register Values: ";
        stat_console << this->circuit.GetCircuitStatus();
        stat_console << "\nModule Finished Running:";

        while(!data_buffer.empty() && data_buffer.top().first == -cycle){
            // #* Code for Debugging
            // std::cout << "b" << data_buffer.size() << std::endl;

            auto module_run = data_buffer.top();
            data_buffer.pop();

            stat_console << std::string(module_run.second.first) << " ";    // Data.first

            this->circuit.UpdateLine(&module_run.second.second);   // Update Line Data (Data.second)

            for(auto i : next_module[module_run.second.first]){
                if(IsClockSyncModule(i) && i != "RunRegFile") // Don't add if it is a clock sync module (output line of clock sync module doesn't change before clock cycle)
                    continue;
                temp_new_modules.push_back(i);
            }
        }

        // #* Code for Debugging
        // std::cout << "c" << std::endl;

        /* Delete Components That Appear Twice */
        std::sort(temp_new_modules.begin(), temp_new_modules.end());
        temp_new_modules.erase(std::unique(temp_new_modules.begin(), temp_new_modules.end()), temp_new_modules.end());


        /* Run New Modules In Current Line Data */
        for (auto i : temp_new_modules){
            // #* Code for Debugging
            // std::cout << "i" << i << std::endl;

            Data runned = this->circuit.Run(i);

            // #* Code for Debugging
            // // std::cout << "e" << i << runned[0].second << std::endl;
            // for(auto j: runned){
            //     std::cout << j.first << j.second << ' ';
            // }
            // std::cout << "f" << std::endl;

            data_buffer.push(
                std::make_pair(-cycle - module_time[i], std::make_pair(i, runned)));

            // #* Code for Debugging
            // std::cout << 'g' << std::endl;
        }

        // #* Code for Debugging
        // std::cout << "d" << std::endl;

        this->cycle++;
        return 0;
    }
    return 1;
}
