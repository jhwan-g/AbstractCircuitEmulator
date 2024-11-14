#ifndef LC3BINARYCODEGEN_H
#define LC3BINARYCODEGEN_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <bitset>
#include <string>
#pragma once

/* function RunAssemble
* args:		std::string-file_name, std::bitset<16>-memory value
* return:	int (0 if succeded, -1 if error occured), *implicit return: memory value
* output:   print values to io function
*/
int LC3BinaryCodeGen(std::string file_name, std::bitset<16> memory[]);
#endif
