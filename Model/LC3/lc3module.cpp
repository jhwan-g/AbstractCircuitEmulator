#include<bitset>
#pragma once

using std::bitset;
using bitwd = bitset<16>;

// 항상 bitset에서 0이 가장 왼쪽
// method인수에서 control signal이 항상 앞에 와야 한다
class pC{
    bitwd val;

public:
    bitwd Run(bool is_positive_edge, bitwd& newval){
        if(!is_positive_edge)
            return val;
        val = newval;
        return val;
    }
    bitwd GetPc(){
        return val;
    }
};

class mEmory{
    bitwd memory[1 << 16];

public:
    int SetMemory(bitwd newmemory[1<<16]){
        for (int i = 0; i < (1<<16); i++)
            memory[i] = newmemory[i];
        return 1;
    }

    bitwd Run(bitwd& address){
        return memory[address.to_ulong()];
    }
};

template <int T>
class mUX2{

public:
    // inp1 if control == 0 inp2 if control != 0
    bitset<T> Run(bitset<1>& control, bitset<T>& inp1, bitset<T>& inp2){
        if(control[0] == 0)
            return inp1;
        return inp2;
    }
};

class rEgFile{
    bitwd registers[8];

public:
    // write -> read 순서로 일어남
    std::pair<bitwd, bitwd> Run(bool is_positive_edge, bitset<1>& regwrite, bitset<3>& dr, bitset<3>& sr1, bitset<3>& sr2, bitwd& data){
        // #*
        // std::cerr << "rEgFile0" << ' ' << is_positive_edge << ' ' << regwrite << std::endl;

        if(regwrite.to_ulong() && is_positive_edge){

            // #*
            // std::cerr << "rEgFile" << BitToInt3(dr) << ' ' << data << std::endl;

            registers[dr.to_ulong()] = data;
        }
        return {registers[sr1.to_ulong()], registers[sr2.to_ulong()]};
    }

    bitwd* GetRegisterVal(){
        return registers;
    }
};

// 순서대로 n, p, z
class statusController{
    bitset<3> status;

public:
    bitset<3>Run(bool is_positive_edge, bitset<2> statauscontrol, bitwd& aluresult, bitwd& memresult){
        if(!is_positive_edge || !statauscontrol.any())
            return status;

        bitwd &result = aluresult;
        if(statauscontrol == bitset<2>{"10"})
            result = memresult;

        if (result[15] == 1)
        {
            status[0] = 1;
            status[1] = status[2] = 0;
        }
        else
        {
            if(result.any()){
                status[1] = 1;
                status[0] = status[2] = 0;
            } else{
                status[2] = 1;
                status[0] = status[1] = 0;
            }
        }
        return status;
    }
};

class bitWDAdder{
public:
    bitwd Run(const bitwd& val1, const bitwd& val2){
        bitset<1> carry{0};
        bitwd ret{
            0,
        };
        for (int i = 0; i < 16; i++)
        {
            ret[i] = (val1[i] + val2[i] + carry[0]) % 2;
            carry[0] = (val1[i] + val2[i] + carry[0]) >= 2;
        }
        return ret;
    }
};

class aLU{
public:
    bitwd Run(bitset<2>& alucontrol, bitwd& val1, bitwd& val2){
        if(alucontrol[1] == 0 && alucontrol[0] == 0){
            return bitWDAdder{}.Run(val1, val2);
        } else if(alucontrol[1] == 1 && alucontrol[0] == 0){
            return val1 | val2;
        } else if(alucontrol[1] == 0 && alucontrol[0] == 0){
            return ~val1;
        } else{
            return bitwd{
                0,
            };
        }
    }
};

class branchController{
public:
    bitset<1> Run(bitset<3>& stat, bitset<3>& brstat){
        return bitset<1>{!(stat & brstat).any()};
    }
};

template<int T>
class signExtendToWD {
public:
    bitwd Run(const std::bitset<T>& val) {
        bitwd ret{0};
        // The sign bit is the leftmost bit (most significant bit) in 'val'
        bool sign_bit = val[T - 1];
        // Fill the upper bits of 'ret' with the sign bit
        for (int i = 15; i >= T; i--) {
            ret[i] = sign_bit;
        }
        // Copy the bits from 'val' into the lower bits of 'ret'
        for (int i = 0; i < T; ++i) {
            ret[i] = val[i];
        }
        return ret;
    }
};

class signExtend {
public:
    bitwd Run(std::bitset<2> signextendcontrol, const bitwd& val) {
        unsigned int sec = signextendcontrol.to_ulong();
        if (sec == 0) {
            // Sign-extend rightmost 5 bits
            const int T = 5;
            std::bitset<T> valT;
            for (int i = 0; i < T; ++i) {
                valT[i] = val[i];
            }
            signExtendToWD<T> ext;
            return ext.Run(valT);
        } else if (sec == 1) {
            // Sign-extend rightmost 6 bits
            const int T = 6;
            std::bitset<T> valT;
            for (int i = 0; i < T; ++i) {
                valT[i] = val[i];
            }
            signExtendToWD<T> ext;
            return ext.Run(valT);
        } else if (sec == 2) {
            // Sign-extend rightmost 9 bits
            const int T = 9;
            std::bitset<T> valT;
            for (int i = 0; i < T; ++i) {
                valT[i] = val[i];
            }
            signExtendToWD<T> ext;
            return ext.Run(valT);
        } else {
            // Handle invalid control signal
            return bitwd{0};
        }
    }
};

class trapDetector{
public:
    bitset<1> Run(bitwd& instruction){
        if(instruction[15] == 1 && instruction[14] == 1 && instruction[13] == 1 && instruction[12] == 1)
            return bitset<1>{1};
        return bitset<1>{0};
    }
};

class dEcoder {
public:
    /*
    0   Regwrite
    1   Branch
    2   Branch
    3   Branch
    4   MUX1
    5   MUX2
    6   ALUControl
    7   ALUControl
    8   MUX4
    9   MUX5
    10  SignExtendControl
    11	SignExtendControl
    12  StatusControll
    13  StatusControll
    */
    bitset<14> Run(bitwd& instruction) {
        bitset<14> ret{0};

        // Extract opcode from bits[0:3] (leftmost bits)
        unsigned int opcode = 0;
        for (int i = 0; i < 4; ++i) {
            opcode = (opcode << 1) | instruction[15 - i];
        }

        if (opcode == 0x1) { // ADD
            if (instruction[5] == 0) {
                // Structure [1]
                ret = bitset<14>("10001000000001"); // 가장 왼쪽이 regwrite
            } else {
                // Structure [2]
                ret = bitset<14>("10000000000001");
            }
        } else if (opcode == 0x5) { // AND
            if (instruction[5] == 0) {
                // Structure [3]
                ret = bitset<14>("10001001000001");
            } else {
                // Structure [4]
                ret = bitset<14>("10000001000001");
            }
        } else if (opcode == 0x0) { // BR
            // Structure [5]
            ret.reset();
            ret[0] = 0; // RegWrite
            ret[1] = instruction[4]; // n
            ret[2] = instruction[5]; // z
            ret[3] = instruction[6]; // p
            ret[9] = 1; // MUX5
            ret[10] = 1; // MUX6
        } else if (opcode == 0xC) { // JMP
            // Structure [6]
            ret = bitset<14>("00000000100000");
        } else if (opcode == 0xA) { // LD
            // Structure [7]
            ret = bitset<14>("10000100011010");
        } else if (opcode == 0x6) { // LDR
            // Structure [8]
            ret = bitset<14>("10000100000110");
        } else if (opcode == 0xE) { // LEA
            // Structure [9]
            ret = bitset<14>("10000000011000");
        } else if (opcode == 0x9) { // NOT
            // Structure [10]
            ret = bitset<14>("10000010000001");
        } else if (opcode == 0xF) { // TRAP
            // Structure [11]
            ret.reset();
        } else {
            // Unrecognized instruction
            ret.reset();
        }
        return ret;
    }
};
