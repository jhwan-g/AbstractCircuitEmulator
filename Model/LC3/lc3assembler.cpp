// This is for assembler of LC3 visual simulator
#include<iostream>
#include<fstream>
#include<string.h>
#include<vector>
#include<utility>
#include<cctype>
#include<map>

#include "lc3assembler.h"
#include "../../ModelView/consoleglobal.h"

using std::vector;

using pii = std::pair<int, int>;

using token = std::pair<int, std::pair<int, int> >; // (토큰 주소, (토큰 종류(keyword: 0, immediate: 1, label: 2, semicolon: 3, register: 4, 값))
    // 아래는 값에 들어가는 내용
    // keyword의 경우에는 각 keyword를 숫자에 대응 (keyword 나타내는 번호 * 10 + form 나타내는 번호)
    // immediate의 경우에는 실제 값을 넣음
    // label의 경우에는 주소를 넣음(parsing 단계에서, 이전에는 그냥 0)

// Token type definitions
inline const int TOKEN_TYPE_KEYWORD = 0;
inline const int TOKEN_TYPE_IMMEDIATE = 1;
inline const int TOKEN_TYPE_LABEL = 2;
inline const int TOKEN_TYPE_REGISTER = 3;

// Keyword definitions
inline const char *KEYWORDS[] = {"ADD", "AND", "NOT", "LD", "LDR", "LDI", "LEA", "BR", "BRn", "BRz", "BRp", "BRnz", "BRnp", "BRzp", "BRnzp" , "JMP", "HALT", ".ORIG"};
inline const int INSTRUCTION_LENGTH[] = {4, 4, 3,     3,     4,     3,     3,     2,    2,     2,     2,     2,       2,    2,       2,       2,      1};
inline int AVAL_FORM[][2] = {{7, 5}, {7, 5}, {4, -1}, {3, -1}, {6, -1}, {3, -1}, {3, -1}, {1, -1}, {1, -1}, {1, -1}, {1, -1}, {1, -1}, {1, -1}, {1, -1}, {1, -1},
                             {2, -1}, {0, -1}};
inline int NUM_KEYWORDS = 18;

// Form structure to define each form's expected token types and immediate sizes
struct Form {
    std::vector<int> token_types;                // Expected sequence of token types
    std::map<int, int> immediate_sizes;          // Map of token index to immediate size (in bits)
};

// Initialize forms with their expected token types and immediate sizes
inline void InitializeForms(std::map<int, Form> &forms) {
    forms[0] = { {TOKEN_TYPE_KEYWORD}, {} }; // Form0: (opcode)
    forms[1] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_IMMEDIATE}, { {1, 9} } }; // Form1: (opcode) (offset9)
    forms[2] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_REGISTER}, {} }; // Form2: (opcode) (register)
    forms[3] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}, { {2, 9} } }; // Form3: (opcode) (register, offset9)
    forms[4] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER}, {} }; // Form4: (opcode) (register1, register2)
    forms[5] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}, { {3, 5} } }; // Form5: (opcode) (register1, register2, offset5)
    forms[6] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_IMMEDIATE}, { {3, 6} } }; // Form6: (opcode) (register1, register2, offset6)
    forms[7] = { {TOKEN_TYPE_KEYWORD, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER, TOKEN_TYPE_REGISTER}, {} }; // Form7: (opcode) (register1, register2, register3)
}

// (Label, 주소) 순서쌍으로 저징
class symbolTable{
    std::vector<std::pair<std::string, int> > table;
public:
    // search from name of lable
    int GetAddress(const std::string& label_name){
        // return -1 if doesn't exist
        // return address of label if exist
        for(auto i: table){
            if(i.first != label_name) continue;
            return i.second;
        }
        return -1;
    }

    // add new value to symbolTable
    int AddLabel(std::string& label_name, int address){
        // label이 모호할 경우 -1 return
        if(GetAddress(label_name) != -1){
            return -1;
        }
        table.push_back(std::make_pair(label_name, address));
        return 0;
    }
};

// 토큰 종류: keyword: 0, immediate: 1, label,register: 2, 줄바꿈: 3
class lexToken{
    int type;
    std::string str;
    int value;
    int address;

public:
    lexToken(int addr, int tp, const std::string& inp){
        type = tp;
        str = inp;
        address = addr;
    }
    lexToken(int addr, int tp, int v){
        type = tp;
        value = v;
        address = addr;
    }
    int typ(){
        return type;
    }
    std::string& st(){
        return str;
    }
    int val(){
        return value;
    }
    int addr(){
        return address;
    }
    void print(){
        debug_console<<"("<<address<<") "<<type<<' ';
        if(type == 3) debug_console<<"\\n"<<"\n";
        if(type == 0 || type == 1 || type == 1) debug_console<<value<<"\n";
        else debug_console<<str<<"\n";
    }
    lexToken(const lexToken& other) = default;
    lexToken& operator=(const lexToken& other) = default;
    /*
        lexToken(lexToken&& other) noexcept = default;
        lexToken& operator=(lexToken&& other) noexcept = default;
        */
    ~lexToken() = default;
};

// tokenized_program, lex_tokens, sem_table, KEYWORDS, NUM_KEYWORDS, END_SYMBOLS
namespace assembler{
// global variables
std::vector<token>tokenized_program;
std::vector<lexToken>lex_tokens;
symbolTable sem_table;
std::string END_SYMBOLS = " \n\t,";
// Map to store all forms
std::map<int, Form> forms;

}

// ******* between these are gpt generated code - need to review
// Function to check if tokens match a specific form
bool CheckForm(int start_ind, int end_ind, int form_number) {
    Form& form = assembler::forms[form_number];
    int num_tokens = end_ind - start_ind + 1;

    if (num_tokens != form.token_types.size()) {
        return false;
    }

    for (int i = 0; i < num_tokens; ++i) {
        int token_index = start_ind + i;
        int expected_type = form.token_types[i];
        int actual_type = assembler::tokenized_program[token_index].second.first;

        if (expected_type != actual_type) {
            return false;
        }

        // If the token is an immediate, check its size
        if (expected_type == TOKEN_TYPE_IMMEDIATE) {
            auto it = form.immediate_sizes.find(i);
            if (it != form.immediate_sizes.end()) {
                int immediate_size = it->second;
                int value = assembler::tokenized_program[token_index].second.second;

                // Calculate the minimum and maximum values for the given immediate size
                int min_value = -(1 << (immediate_size - 1));
                int max_value = (1 << (immediate_size - 1)) - 1;

                if (value < min_value || value > max_value) {
                    return false; // Immediate value out of range
                }
            } else {
                return false; // Immediate size not specified
            }
        }
    }
    return true; // All checks passed
}

/* function GetFormNumber
 * Return form number if valid; otherwise, return -1
 * args: 	int start_ind, int end_ind
 * return: 	int
 */
int GetFormNumber(int start_ind, int end_ind) {
    // Initialize forms only once
    static bool forms_initialized = false;
    if (!forms_initialized) {
        InitializeForms(assembler::forms);
        forms_initialized = true;
    }

    // Validate indices
    if (start_ind < 0 || end_ind >= assembler::tokenized_program.size() || start_ind > end_ind) {
        return -1;
    }

    // Check if the first token is a keyword
    if (assembler::tokenized_program[start_ind].second.first != TOKEN_TYPE_KEYWORD) {
        return -1; // First token is not a keyword
    }
    // Get the keyword number from the first token
    int keyword_number = assembler::tokenized_program[start_ind].second.second;

    // Get the instruction length for the keyword
    int inst_length = INSTRUCTION_LENGTH[keyword_number];

    // Check if the number of tokens matches the instruction length
    if (end_ind - start_ind + 1 != inst_length) {
        return -1; // Token count does not match
    }

    // Retrieve possible forms for the keyword
    std::vector<int> possible_forms;
    for (int i = 0; i < 2; ++i) {
        int form = AVAL_FORM[keyword_number][i];
        if (form >= 0) { // Assuming -1 indicates an invalid form
            possible_forms.push_back(form);
        }
    }

    // Check each possible form
    for (int form_number : possible_forms) {
        if (CheckForm(start_ind, end_ind, form_number)) {
            return form_number; // Valid form found
        }
    }

    return -1; // No valid form matched
}
// ********

/* function ToUpper
*  string내의 모든 문자열을 upper case로 변환
*  args: 	std::string
*  return: 	std::string
*/
std::string ToUpper(const std::string& str){
    std::string result = str;
    for(size_t i = 0; i < result.length(); i++){
        result[i] = std::toupper((unsigned char)result[i]);
    }
    return result;
}

/* function IsVectorStingSame
*  args: 	vector<char>&, char*
*  return: 	bool
*/
bool IsVectorStringSame(vector<char>& input1, char* input2){
    for(int i = 0 ; i < input1.size() ; i++){
        if(input2[i] == 0) return false;
        if(input2[i] != input1[i]) return false;
    }
    if(input2[input1.size()] != 0) return false;
    return true;
}

/* function VectorToString
*  arg: 	vector<char>&
*  return: 	std::string
*/
std::string VectorToString(const std::vector<char>& str) {
    return std::string(str.begin(), str.end());
}

/* function ConvertToNumber
* arg: 		vector<char>&
* return: 	pair<int, int>
*     <-2, 0> if nan
*     <-2, -1> if wrong number expression
*     <sign(-1 or 1), absolute value> if number
*/
pii ConvertToNumber(vector<char>& str) {
    int pow = 0; // Base (10 or 16)
    int value = 0;
    int sign = 1;
    int ind = 0;
    int n = str.size();

    if (n == 0) {
        // Empty string represents 'nan'
        return std::make_pair(-2, 0);
    }

    // Handle sign
    if (str[ind] == '-') {
        sign = -1;
        ind++;
        if (ind >= n) {
            // Only a sign without number
            return std::make_pair(-2, -1);
        }
    }
    // Determine base
    if (std::isdigit(str[ind]) || str[ind] == '#') {
        if(str[ind] == '#') ind++;
        pow = 10;
    } else if (str[ind] == 'x') {
        pow = 16;
        ind++;
        if (ind >= n) {
            // 'x' without digits
            return std::make_pair(-2, -1);
        }
    } else {
        // Not starting with digit or 'x' => 'nan'
        return std::make_pair(-2, 0);
    }

    // Parse the number
    for (; ind < n; ind++) {
        char current = str[ind];
        if (pow == 10) {
            if (std::isdigit(current)) {
                value = value * pow + (current - '0');
            } else {
                // Invalid character in number
                return std::make_pair(-2, -1);
            }
        } else if (pow == 16) {
            if (std::isdigit(current)) {
                value = value * pow + (current - '0');
            } else if ('a' <= current && current <= 'f') {
                value = value * pow + (current - 'a' + 10);
            } else if ('A' <= current && current <= 'F') {
                value = value * pow + (current - 'A' + 10);
            } else {
                // Invalid character in number
                return std::make_pair(-2, -1);
            }
        }
    }

    return std::make_pair(sign, value);
}

/* function ConvertStringToPNumber
* if string is positive decimal expressed number convert to int else -1
* arg: 		std::string&
* return: 	int
*/
int ConvertStringToPNumber(const std::string& str) {
    // Check if the string is empty
    if (str.empty()) {
        return -1;
    }

    // Iterate through the string to ensure all characters are digits
    for (char c : str) {
        if (!std::isdigit(c)) {
            return -1;  // Return -1 if a non-digit character is found
        }
    }

    // Convert the string to an integer using stoi
    try {
        int result = std::stoi(str);  // stoi will throw an exception if conversion fails
        if (result < 0) {
            return -1;  // Return -1 if the number is negative
        }
        return result;  // Return the positive integer
    } catch (std::invalid_argument&) {
        return -1;  // Return -1 if the conversion fails (invalid string)
    } catch (std::out_of_range&) {
        return -1;  // Return -1 if the number is out of range for an int
    }
}

/* function GetKeywordNumber
* 키워드에 대응하는 숫자를 return하는 함수
* arg: 		std::string
* return: 	int (-1 if not a keyword)
*/
int GetKeywordNumber(const std::string word) {
    // KEYWORDS 배열을 순회하며 비교
    for (int i = 0; i < NUM_KEYWORDS; ++i) {
        if (word == KEYWORDS[i]) {
            return i; // 키워드에 해당하는 인덱스 반환
        }
    }
    return -1; // 키워드가 아닌 경우 -1 반환
}

// functions that runs code
/* function Lexer
* arg:		std::ifstream& (LC3 source_file)
* return: 	성공적으로 진행되었는지
*/
int Lexer(std::ifstream& source_file){
    vector<char>buffer;
    int mem_addr = 0x3000;

    while(!source_file.eof()){
        char current = 0;
        source_file.get(current);

        if(current == ' ' || current == '\n' || current == '\t' || current == ','){ // 공백 문자나 콤마를 만난 경우 -> 표현이 끝난 경우
            if(buffer.size() == 0) continue;

            // 주석인 경우 -> '\n' 이면 버퍼 비우고 아니면 그냥 계속 읽어들임
            if(buffer.size() >= 1 && buffer[0] == ';') {
                if(current == '\n') {
                    buffer.clear();
                    assembler::lex_tokens.push_back(lexToken(mem_addr, 3, 0));
                }
                continue;
            }

            // 주석이 아닌 경우
            int keyword_number = GetKeywordNumber(ToUpper(VectorToString(buffer)));

            if(keyword_number != -1){ // keyword인 경우
                if(keyword_number == 17){ // 주소 지정 keyword의 경우
                    buffer.clear();
                    while(!source_file.eof()){
                        source_file.get(current);
                        if(assembler::END_SYMBOLS.find(current) != std::string::npos) break;
                        buffer.push_back(current);
                    }
                    pii new_addr = ConvertToNumber(buffer);
                    if(new_addr.first == -2){
                        debug_console <<"[Error] Wrong number expression"<<"\n";
                        return -1;
                    }
                    mem_addr = new_addr.first * new_addr.second;
                    if(mem_addr < 0x3000 || mem_addr > INT16_MAX){
                        debug_console<<"[Error] Memory address should be in [0x3000, 0x9999]"<<"\n";
                        return -1;
                    }
                } else{ // 주소 지정 keyword가 아닌 경우

                    // #*
                    // std::cerr << mem_addr << std::endl;

                    assembler::lex_tokens.push_back(lexToken(mem_addr++, 0, keyword_number));
                }
            }
            else{ // keyword 아닌 경우
                pii number = ConvertToNumber(buffer);
                if(number.second == -1){ // 잘못된 표현인 경우
                    debug_console<<"[Error] Invalid number expression: ";
                    for(auto i:buffer) debug_console<<i;
                    debug_console<<"\n";
                    return -1;
                }
                if(number.first == -2){ // 숫자 표현이 아닌 경우
                    if(buffer[buffer.size() - 1] == ':'){ // label 지정 표현인 경우 -> semantic table에 추가
                        std::string str = VectorToString(buffer);
                        if(str[0] == 'R'){
                            debug_console<<"[Error] Label should not start with character 'R'"<<"\n";
                            return -1;
                        }

                        int is_succeded = assembler::sem_table.AddLabel(str, mem_addr);
                        if(is_succeded == -1){
                            debug_console<<"[Error] Label \""<<str<<"\" is ambiguous"<<"\n";
                            return -1;
                        }
                    }
                    // 그 외의 경우 -> token에 추가
                    else{
                        std::string str = VectorToString(buffer);
                        assembler::lex_tokens.push_back(lexToken(mem_addr, 2, str));
                    }
                }
                else{ // 숫자 표현인 경우
                    assembler::lex_tokens.push_back(lexToken(mem_addr, 1, number.second * number.first));
                }
            }

            // 끝이 줄바꿈인 경우 추가 처리
            if(current == '\n'){
                assembler::lex_tokens.push_back(lexToken(mem_addr, 3, 0));
            }

            buffer.clear(); // 버퍼 비우기
        }
        else{
            buffer.push_back(current);
        }
    }
    return 0;
}

/* function Parser
* arg:		none
* return:	성공적으로 진행되었는지
*/
int Parser(){
    int cnt = 0;
    for(auto& current_token:assembler::lex_tokens){
        if(current_token.typ() == 3){ // 줄바꿈일 경우(instruction들은 줄바꿈을 기준으로 구분됨)
            if(cnt == 0) continue; // 아무것도 없이 줄바꿈만 있는 경우 -> 줄바꿈 날림
            else{ // 문법 오류 검사
                int n = assembler::tokenized_program.size();
                int form_number = GetFormNumber(n - cnt, n - 1);
                if(form_number == -1){
                    debug_console<<"[Error] Invalid instruction format of mnemonic \""<<KEYWORDS[assembler::tokenized_program[n - cnt].second.second]<<"\" "<<
                        "loc:"<<assembler::tokenized_program[n - cnt].first<<"\n";
                    return -1;
                }
                assembler::tokenized_program[n - cnt].second.second = assembler::tokenized_program[n - cnt].second.second * 10 + form_number; // 저장하는 값 = 토큰 번호 * 10 + 형식 숫자

                cnt = 0;
            }
        } else{ // 줄바꿈이 아닐 경우 -> 일단 tokenized program에 넣음(문법 오류 검사는 나중에)
            cnt++;
            int addr = current_token.addr(), type = current_token.typ();

            // #*
            // std::cerr << "Parser" << addr << std::endl;

            if(type == 2 && current_token.st()[0] != 'R'){ // label일 경우
                int label_loc = assembler::sem_table.GetAddress(current_token.st() + ":");
                if(label_loc == -1){
                    debug_console<<"[Error] Label \""<<current_token.st()<<"\" does not exist"<<"\n";
                    return -1;
                }
                int diff_addr = current_token.addr() - label_loc - 1;
                assembler::tokenized_program.push_back(std::make_pair(addr, std::make_pair(type, diff_addr)));
            } else if(type == 2){ //Register일 경우
                std::string register_number;
                for(int i = 1 ; i < current_token.st().length() ; i++) register_number.push_back(current_token.st()[i]);
                int int_register_number = ConvertStringToPNumber(register_number);
                if(int_register_number == -1){
                    debug_console<<"[Error] Invalid register number expression"<<"\n";
                    return -1;
                }
                assembler::tokenized_program.push_back(std::make_pair(addr, std::make_pair(TOKEN_TYPE_REGISTER, int_register_number)));
            } else{
                assembler::tokenized_program.push_back(std::make_pair(addr, std::make_pair(type, current_token.val())));
            }
        }
    }
    return 0;
}

/* function RunAssemble
* args:		char* (file_name(path included))
* return:	int 성공적으로 진행되었는지
* output: 	bytecode that simulator can understand(file name a.o)
*/
int RunAssemble(const char* file_name){
    std::ifstream source_file;

    // File extension check
    if(strcmp(file_name + (strlen(file_name) - 4) * sizeof(char), ".asm") != 0){
        debug_console<<"[Error] Unsupported file extension. Only supports .asm files"<<"\n";
        return -1;
    }

    source_file.open(file_name);

    // When file does not exist
    if(!source_file.is_open()){
        debug_console<<"[Error] File does not exist"<<"\n";
        return -1;
    }

    if(Lexer(source_file) == -1) {
        debug_console<<"[Error] Assemble error during lexical analysis"<<"\n";
        return -1;
    }

    source_file.close();

    if(Parser() == -1) {
        debug_console<<"[Error] Assemble error during parsing"<<"\n";
        return -1;
    }

    std::ofstream bytecode_file;
    bytecode_file.open("./a.o");
    for (auto i : assembler::tokenized_program)
    {
        bytecode_file<<i.first<<i.second.first<<i.second.second<<'\n';
    }
    bytecode_file.close();
    return 0;
}
