#ifndef LC3ASSEMBLER_H
#define LC3ASSEMBLER_H

/* function RunAssemble
* args:		char* (file_name(path included))
* return:	int (0 if succeded, -1 if error occured)
* output: 	bytecode that simulator can understand(file name a.o) + print values to io function
*/
int RunAssemble(const char* file_name);
#endif
