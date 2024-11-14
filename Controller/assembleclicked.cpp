#include "assembleclicked.h"
#include "../Model/LC3/lc3assembler.h"
#include "globalfilereader.h"
#include "../ModelView/consoleglobal.h"

void assembleClicked::Run(){

    debug_console.ClearConsole();

    if(file_reader.IsFilePathValid() == false){
        debug_console<<"File Path Isn't Valid\n";
    } else{
        file_reader.PrintFilePath();
    }

    if(RunAssemble(file_reader.GetFilePath().c_str()) == 0){
        debug_console<<"Assemble Finished\n";
    }
}

assembleClicked::assembleClicked() {
    // Constructor implementation
}

assembleClicked::~assembleClicked() {
    // Destructor implementation
}

void assembleClicked::someFunction() {
    // Function implementation
}
