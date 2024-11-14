/* Based of GPT Generated Code */
#include "LC3binarycodegen.h"
#include "ModelView/consoleglobal.h"

// Token types
const int TOKEN_TYPE_KEYWORD   = 0;
const int TOKEN_TYPE_REGISTER  = 1;
const int TOKEN_TYPE_IMMEDIATE = 2;

// Keyword definitions
const char* KEYWORDS[] = {
    "ADD", "AND", "NOT", "LD", "LDR", "LDI", "LEA", "BR", "BRn",
    "BRz", "BRp", "BRnz", "BRnp", "BRzp", "BRnzp", "JMP", "HALT", ".ORIG"
};
const int NUM_KEYWORDS = 18;

// Map keyword & form number to instruction structure
std::map<std::pair<std::string, int>, int> keywordform_to_structure = {
    {{"ADD", 7}, 1},
    {{"ADD", 5}, 2},   // Both register and immediate forms
    {{"AND", 7}, 3},
    {{"AND", 5}, 4},    // Both register and immediate forms
    {{"BR", 1}, 5},
    {{"BRn", 1}, 5},
    {{"BRz", 1}, 5},
    {{"BRp", 1}, 5},
    {{"BRnz", 1}, 5},
    {{"BRnp", 1}, 5},
    {{"BRzp", 1}, 5},
    {{"BRnzp", 1}, 5},
    {{"JMP", 2}, 6},
    {{"LD", 3}, 7},
    {{"LDR", 6}, 8},
    {{"LEA", 3}, 9},
    {{"NOT", 3}, 10},
    {{"HALT", 0}, 11},
    };

// Instruction structures
struct InstructionStructure {
    int opcode;                  // Opcode bits (4 bits)
    std::vector<int> fields;     // Expected sequence of token types
};

// Define instruction structures
std::map<int, InstructionStructure> instruction_structures = {
    {1,  {0b0001, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER}}},       // ADD (register)
    {2,  {0b0001, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}}},      // ADD (immediate)
    {3,  {0b0101, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER}}},       // AND (register)
    {4,  {0b0101, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}}},      // AND (immediate)
    {5,  {0b0000, {TOKEN_TYPE_IMMEDIATE}}},                                               // BR
    {6,  {0b1100, {TOKEN_TYPE_REGISTER}}},                                                // JMP
    {7,  {0b0010, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}}},                          // LD
    {8,  {0b0110, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}}},     // LDR
    {9,  {0b1110, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}}},                          // LEA
    {10, {0b1001, {TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER}}},                           // NOT
    {11, {0b1111, {TOKEN_TYPE_IMMEDIATE}}},                                               // HALT
};

// Token structure
struct Token {
    int address;
    int type;
    int value;
};

// Function to read and parse the tokens from the file
std::vector<Token> ReadTokens(const std::string& filename) {
    std::vector<Token> tokens;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        // Remove any whitespace
        line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        // Parse the line into address, type, and value
        // Assuming address is up to 5 digits, type is 1 digit, value is the rest
        if (line.length() < 6) {
            debug_console << "[Error-Loading Stage] Invalid token format: " << line << "\n";
            continue;
        }
        int address = std::stoi(line.substr(0, 5));
        int type = std::stoi(line.substr(5, 1));
        int value = std::stoi(line.substr(6));

        tokens.push_back({address, type, value});
    }
    return tokens;
}

// Function to encode an instruction based on its structure and tokens
std::bitset<16> EncodeInstruction(int structure_id, const std::vector<Token>& instruction_tokens, const std::string& mnemonic) {
    // If instruction is HALT
    if(structure_id == 0){
        return std::bitset<16>{
            0,
        };
    }

    InstructionStructure instr_struct = instruction_structures[structure_id];
    std::bitset<16> machine_code(0);

    // Set the opcode
    machine_code |= (instr_struct.opcode << 12);

    // Implement encoding logic based on the structure_id
    switch (structure_id) {
    case 1: // ADD (register)
    case 3: // AND (register)
    {
        // instruction_tokens[1] is DR
        machine_code |= (instruction_tokens[1].value & 0x7) << 9;
        // instruction_tokens[2] is SR1
        machine_code |= (instruction_tokens[2].value & 0x7) << 6;
        // instruction_tokens[3] is SR2
        machine_code |= (instruction_tokens[3].value & 0x7);
        break;
    }
    case 2: // ADD (immediate)
    case 4: // AND (immediate)
    {
        // instruction_tokens[1] is DR
        machine_code |= (instruction_tokens[1].value & 0x7) << 9;
        // instruction_tokens[2] is SR1
        machine_code |= (instruction_tokens[2].value & 0x7) << 6;
        // Set immediate flag
        machine_code |= 0x20;
        // instruction_tokens[3] is immediate value
        machine_code |= (instruction_tokens[3].value & 0x1F);
        break;
    }
    case 5: // BR
    {
        // Set condition codes based on mnemonic
        int cc = 0;
        if (mnemonic.find('n') != std::string::npos) cc |= 0x0800;
        if (mnemonic.find('z') != std::string::npos) cc |= 0x0400;
        if (mnemonic.find('p') != std::string::npos) cc |= 0x0200;
        // If no specific condition codes, default to BRnzp
        if (cc == 0) cc = 0x0E00;
        machine_code |= cc;
        // instruction_tokens[1] is immediate value
        machine_code |= (instruction_tokens[1].value & 0x1FF);
        break;
    }
    case 6: // JMP
    {
        // instruction_tokens[1] is BaseR
        machine_code |= (instruction_tokens[1].value & 0x7) << 6;
        break;
    }
    case 7: // LD
    case 9: // LEA
    {
        // instruction_tokens[1] is DR
        machine_code |= (instruction_tokens[1].value & 0x7) << 9;
        // instruction_tokens[2] is PCoffset9
        machine_code |= (instruction_tokens[2].value & 0x1FF);
        break;
    }
    case 8: // LDR
    {
        // instruction_tokens[1] is DR
        machine_code |= (instruction_tokens[1].value & 0x7) << 9;
        // instruction_tokens[2] is BaseR
        machine_code |= (instruction_tokens[2].value & 0x7) << 6;
        // instruction_tokens[3] is offset6
        machine_code |= (instruction_tokens[3].value & 0x3F);
        break;
    }
    case 10: // NOT
    {
        // instruction_tokens[1] is DR
        machine_code |= (instruction_tokens[1].value & 0x7) << 9;
        // instruction_tokens[2] is SR
        machine_code |= (instruction_tokens[2].value & 0x7) << 6;
        // Set bits [5:0] to 1
        machine_code |= 0x003F;
        break;
    }
    case 11: // TRAP / HALT
    {
        // instruction_tokens[0] is the mnemonic
        // For HALT, the trap vector is x25 (0x25)
        int trap_vector = 0x25;
        if (mnemonic == "TRAP") {
            // instruction_tokens[1] is trap vector
            trap_vector = instruction_tokens[1].value & 0xFF;
        }
        machine_code |= trap_vector;
        break;
    }
    default:
        debug_console << "[Error-Loading Stage] Unknown instruction structure: " << structure_id << "\n";
        break;
    }

    return machine_code;
}

int LC3BinaryCodeGen(std::string file_name, std::bitset<16> machine_codes[]) {
    // Read tokens from the file
    std::vector<Token> tokens = ReadTokens(file_name);
    int pc0 = 0; // Address where pc is first set

    // Process tokens to form instructions
    for (size_t i = 0; i < tokens.size();) {
        Token token = tokens[i];

        // Set PC start address to front of the file
        if(i == 0)
            pc0 = token.address;

        if (token.type != TOKEN_TYPE_KEYWORD) {
            debug_console << "[Error-Loading Stage] Expected a keyword token at position " << i << "\n";
            return 0;
        }

        // Get the mnemonic
        std::string mnemonic = KEYWORDS[token.value / 10];

        // Get token form number
        int structure_id = keywordform_to_structure[{mnemonic, token.value % 10}];

        std::vector<Token> instruction_tokens; // Tokens that will change to the machine code
        instruction_tokens.push_back(token); // Include the keyword token

        bool match = true;
        // Check if the following tokens match the expected types
        for (size_t j = 0; tokens[i +1 + j].type != TOKEN_TYPE_KEYWORD; ++j) {
            Token next_token = tokens[i + 1 + j];
            instruction_tokens.push_back(next_token);
        }

        // #*Code for debugging
        // std::cerr << token.address << std::endl;

        // Encode the instruction and add to machine code
        machine_codes[token.address] = EncodeInstruction(structure_id, instruction_tokens, mnemonic);

        // Move to the next instruction
        i += instruction_tokens.size();
    }

    return pc0;
}
