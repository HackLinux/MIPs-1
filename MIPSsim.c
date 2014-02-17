/*
 * Creator :: Anirudh Subramanian
 * MIPSsim --- MIPSsimulator
 * 
 */


#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdbool.h>

bool breakValue = false;
int currentAddr   = 128;
//storing breakaddress in a global variable. will be set only once when break is called.
//Will be used to stop the program  and to Find the data section.
int breakAddress = 128;
int* data;
int dataStartAddress = 0;

const char* JUMP  = "J";
const char* BEQ   = "BEQ";
const char* BGTZ  = "BGTZ";
const char* BREAK = "BREAK";
const char* SW    = "SW";
const char* LW    = "LW";
const char* ADD   = "ADD";
const char* SUB   = "SUB";
const char* MUL   = "MUL";
const char* AND   = "AND";
const char* OR    = "OR";
const char* XOR   = "XOR";
const char* NOR   = "NOR";
const char* ADDI  = "ADDI";
const char* ANDI  = "ANDI";
const char* ORI   = "ORI";
const char* XORI  = "XORI";

int registers[32] = {0};
char* instrStr;

void printToOutput(char* instrString, FILE** outputfile, int currentByteAddr, int dataAddress, int dataSize) {
	static int cycleNumber = 1;
	printf("Inside here");
	fprintf(*outputfile, "--------------------\n");
		
	fprintf(*outputfile, "Cycle:%d\t%d\t%s\n", cycleNumber, currentByteAddr, instrString);
	fprintf(*outputfile, "\n");
	int i = 0;
	fprintf(*outputfile,"Registers\n");
	for(i = 0; i < 32; i++) {
		if((i % 8) == 0) {
			if(i > 0) {
				fprintf(*outputfile, "\n");
			}
			if(i < 10) {
				fprintf(*outputfile, "R%d%d", 0, i);

			} else {
				fprintf(*outputfile, "R%d", i);
			}
		} else {
			fprintf(*outputfile, "\t%d",registers[i]);	
		}
	}	
	fprintf(*outputfile, "\n");
	fprintf(*outputfile,"Data\n");
	int j = 0;
	for(j = 0; j < dataSize; j++) {
		if((j % 8) == 0) {
			if(i > 0) {
				fprintf(*outputfile, "\n");
			}
			fprintf(*outputfile, "%d",  dataAddress + j);
		} else  {	
			fprintf(*outputfile, "\t%d", data[j]);
		}
	}
	fprintf(*outputfile, "\n\n");
	cycleNumber++;
}

void static execute(FILE** temp, FILE** fpSimOutput) {
	//Go to the line in the code after break
	int dataAddress = (breakAddress + 4 -128)/4;
	//Assign dataAddress to dataStartAddress
	dataStartAddress = dataAddress;
	
	//fseek(*temp, dataAddress, SEEK_SET);
	int dataSize = 0;
	char binaryString[100];
	int i = 0;
	fseek(*temp, 0L, SEEK_SET);
	while(fgets(binaryString, 100, *temp) != NULL) {
		i++;	
	}
	printf("value for i is %d", i);
	printf("value for dataAddress is %d", dataAddress);
	printf("size is %d", (i - dataAddress));	
	dataSize = i - dataAddress;
	printf("data size is %d\n", dataSize);

	data = (int*)malloc(sizeof(int)*dataSize);

	fseek(*temp, dataAddress, SEEK_SET);
	int j = 0;
	char* end;
	while(fgets(binaryString, 100, *temp) != NULL) {
		if(j >= dataStartAddress) {
			int t1 = strtol(binaryString, &end, 10);
			printf("%d\n", t1);
			int t2 = strtol(end, &end, 10);
			printf("%d\n", t2);
			data[j-dataStartAddress] = t2;
		} 
		j++;
	}
	fseek(*temp, 0, SEEK_SET);
				
	char* end2;
	int m = 0;
	char* instrString;
	int currentByteAddr = 0;
	int rs              = -1;
	int rt              = -1;
	int rd              = -1;
	int imm             = -1;
	int offset          = -1;
		
	while(fgets(binaryString, 100, *temp) != NULL) {
		printf("%s",binaryString);
		instrString      = strtok(binaryString, "\t");
		printf("instrString is %s", instrString);
		char* finalInstrString = instrString;
		currentByteAddr  = atoi(instrString);
		instrString = strtok(NULL, "\t");		
		printf("After second tokenizing %s\n", instrString);	
		printf("currentAddr in bytes = %d\n", currentByteAddr);
		if(strcmp(instrString, JUMP) == 0) {
			printf("Inside JUMP");
			instrString = strtok(NULL,"\t");
			int jumpAddress = (atoi(instrString) - 128)/4;
			printf("Jumpaddress is %d", jumpAddress);
			//fseek(*temp, 0, jumpAddress);
			int j = 0;
			fseek(*temp,0,SEEK_SET);
			while(j < jumpAddress) {
				fgets(binaryString, 100,*temp);
				j++;
			}
			printf(" address jumped to %ld", ftell(*temp));
						
			printToOutput(finalInstrString, fpSimOutput, currentByteAddr, dataAddress, dataSize);
			continue;
		} 
		
		if(strcmp(instrString, BEQ) == 0) {
			printf("Inside BEQ");
			instrString = strtok(NULL,"\t");
		        int registerA = atoi(instrString);
			instrString = strtok(NULL,"\t");
			int registerB = atoi(instrString);
			instrString = strtok(NULL,"\t");
			int jumpAddress = (atoi(instrString) + currentByteAddr + 4 - 128)/4;
			printf("Inside BEQ jump address is %d", jumpAddress);	
			if(registers[registerA] == registers[registerB]) {
				printf("BEQing");
				int j = 0;
				fseek(*temp,0,SEEK_SET);
				while(j < jumpAddress) {
					fgets(binaryString, 100,*temp);
					j++;
				}
				printf(" address jumped to %ld\n", ftell(*temp));
			}
			//print()
			continue;
		} 
		
		if(strcmp(instrString, BGTZ) == 0) {
			printf("Inside BGTZ");
			
			instrString = strtok(NULL,"\t");
		        rs          = atoi(instrString);
			instrString = strtok(NULL,"\t");
			offset      = atoi(instrString);
			int jumpAddress = (offset + currentByteAddr + 4 - 128)/4 + 1;
		
			if(registers[rs] > 0) {	
				printf("BGTZing");
				int j = 0;
				fseek(*temp,0,SEEK_SET);
				while(j < jumpAddress) {
					fgets(binaryString, 100,*temp);
					j++;
				}
				printf("This is final line :: %s\n", binaryString);
				printf(" address jumped to %ld", ftell(*temp));

			}
				
			//print()
			continue;
		} 
		
		if(strcmp(instrString, BREAK) == 0) {
			printf("Inside BREAK");
			while(instrString != NULL) {
				instrString = strtok(NULL, "\t");			
			}
			//print()
			break;
		} 
		
		if(strcmp(instrString, SW) == 0) {
			printf("Inside SW");
			instrString = strtok(NULL, "\t");
			rs          = atoi(instrString);
			instrString = strtok(NULL, "\t");
			offset      = atoi(instrString);
			instrString = strtok(NULL, "\t");
			rt          = atoi(instrString);
		     	data[(registers[rt] + offset - dataStartAddress)/4] = registers[rt];
			printf("%d\n", data[(registers[rt] + offset - dataStartAddress)/4]);		
			//print()
			continue;
		} 
		
		if(strcmp(instrString, LW) == 0) {
			printf("Inside SW");
			instrString   = strtok(NULL, "\t");
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			offset        = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt           = atoi(instrString);
		     	registers[rd] = data[(registers[rt] + offset - dataStartAddress)/4];		
			printf("%d\n", registers[rd]);		
			continue;
		} 
		
		if(strcmp(instrString, ADD) == 0) {
			printf("Inside ADD");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = registers[rs] + registers[rt];
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, SUB) == 0) {
			printf("Inside SUB");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = registers[rs] - registers[rt];
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, MUL) == 0) {
			printf("Inside MUL");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = registers[rs] * registers[rt];
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, AND) == 0) {
			printf("Inside AND");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = registers[rs] & registers[rt];
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, OR) == 0) {
			printf("Inside OR");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = registers[rs] | registers[rt];
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, XOR) == 0) {
			printf("Inside XOR");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = registers[rs] ^ registers[rt];
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, NOR) == 0) {
			printf("Inside NOR");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rt            = atoi(instrString);
			registers[rd] = ~(registers[rs] | registers[rt]);
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, ADDI) == 0) {
			printf("Inside ADDI");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			int immediate     = atoi(instrString);
			registers[rd] = registers[rs] + immediate;
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, ANDI) == 0) {
			printf("Inside ANDI");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			int immediate     = atoi(instrString);
			registers[rd] = registers[rs] & immediate;
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, ORI) == 0) {
			printf("Inside ORI");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			int immediate     = atoi(instrString);
			registers[rd] = registers[rs] | immediate;
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		if(strcmp(instrString, XORI) == 0) {
			printf("Inside XORI");
			instrString   = strtok(NULL, "\t");		
			rd            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			rs            = atoi(instrString);
			instrString   = strtok(NULL, "\t");
			int immediate     = atoi(instrString);
			registers[rd] = registers[rs] ^ immediate;
			printf("%d\n", registers[rd]);
			continue;
		} 
		
		 
	}
	return;
}

void static disassemble(char* binaryString, FILE** fpDisAssemblyOutput, FILE** temp, FILE** fpSimOutput) {
	/*Changing the \n to terminating character because fgets copies \n*/
	
	binaryString[32]='\0';	
	int category;
	char* end;
	char* end2;
	/*Print the initial binary
	 * Check the category , Check the opcode 
	 * and write the disassembly accordingly
	 * Write only disassembly to the temp file*/
	
	char *instrStr;
	fputs(binaryString, *fpDisAssemblyOutput);
	printf("breakValue is %d", breakValue);
	if(!breakValue) {
	/*
	 * Convert the 32 bit instruction to binary and 
	 * right shift by 29 to retrieve first three bits which is category
	 * */

        	unsigned long int j = strtoul(binaryString, &end, 2);
		//Just and with a number with the first three bits one 
		//this gets the first six bits of the instruction
		//this will help deciding the instruction
		int instr           = (j & 0x1C000000) >> 26;
		//Category will be the first three digits 
		//this will help deciding the category
		category            = (j & 0xE0000000) >> 29;
		unsigned int jump = 0;
		int sourceRegister  = 0;
		int rtRegister      = 0;
		int destRegister    = 0;
		switch(category) {
			case 0:
				jump       = (j & 0x03FFFFFF) << 2;
				
				destRegister   = (j & 0x0000FA00) >> 11;
				sourceRegister = (j & 0x03E00000) >> 21;
				rtRegister     = (j & 0x001F0000) >> 16;
				int immediate      = (j & 0x0000FFFF) << 2;
				int offset         = (j & 0x0000FFFF);	
				printf("Value of instr is %d", instr);
				switch(instr) {
					//Jump
					case 0:
						fprintf(*fpDisAssemblyOutput, "\t%d\tJ #%d\n", currentAddr, jump);
						fprintf(*temp, "%d\tJ\t%d\n", currentAddr, jump);
						break;
					//BEQ
					case 2:
						fprintf(*fpDisAssemblyOutput, "\t%d\tBEQ R%d, R%d, #%d\n", currentAddr, sourceRegister, rtRegister, immediate);		
						fprintf(*temp, "%d\tBEQ\t%d\t%d\t%d\n", currentAddr, sourceRegister, rtRegister, immediate);		
						break;
					//BGTZ
					case 4:
						fprintf(*fpDisAssemblyOutput, "\t%d\tBGTZ R%d, #%d\n", currentAddr, sourceRegister, immediate);		
						fprintf(*temp, "%d\tBGTZ\t%d\t%d\n", currentAddr, sourceRegister, immediate);		
						break;
					//BREAK
					case 5:
						breakValue = true;
						printf("Inside break");
						breakAddress = currentAddr;
						
						fprintf(*fpDisAssemblyOutput, "\t%d\tBREAK\n", currentAddr, jump);		
						fprintf(*temp,"%d\tBREAK\n" , currentAddr, jump);		
						break;
					//SW
					case 6:

						fprintf(*fpDisAssemblyOutput, "\t%d\tSW R%d, %d(R%d)\n", currentAddr, rtRegister, offset, sourceRegister);		
						fprintf(*temp, "%d\tSW\t%d\t%d\t%d\n", currentAddr, rtRegister, offset, sourceRegister);		
						break;
					//LW
					case 7:
						fprintf(*fpDisAssemblyOutput, "\t%d\tLW R%d, %d(R%d)\n", currentAddr, rtRegister, offset, sourceRegister);		
						fprintf(*temp, "%d\tLW\t%d\t%d\t%d\n", currentAddr, rtRegister, offset, sourceRegister);		
						break;
				}
				break;
			case 6:
				sourceRegister = (j & 0x1F000000) >> 24;
				rtRegister     = (j & 0x00F80000) >> 19;
				destRegister   = (j & 0x0000FA00) >> 11;
				instr =          (j & 0x00070000) >> 16;
				switch (instr) {
					//ADD
					case 0:
						fprintf(*fpDisAssemblyOutput, "\t%d\tADD R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);	
						fprintf(*temp, "%d\tADD\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);	
						break;
					//SUB
					case 1:
						fprintf(*fpDisAssemblyOutput, "\t%d\tSUB R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						fprintf(*temp, "%d\tSUB\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						break;
					//MUL
					case 2:
						fprintf(*fpDisAssemblyOutput, "\t%d\tMUL R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						fprintf(*temp, "%d\tMUL\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						break;
					//AND
					case 3:
						fprintf(*fpDisAssemblyOutput, "\t%d\tAND R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						fprintf(*temp, "%d\tAND\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						break;
					//OR
					case 4:
						fprintf(*fpDisAssemblyOutput, "\t%d\tOR R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						fprintf(*temp, "%d\tOR\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						break;
					//XOR
					case 5:
						fprintf(*fpDisAssemblyOutput, "\t%d\tXOR R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						fprintf(*temp, "%d\tXOR\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						break;
					//NOR
					case 6:
						fprintf(*fpDisAssemblyOutput, "\t%d\tNOR R%d, R%d, R%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						fprintf(*temp, "%d\tNOR\t%d\t%d\t%d\n", currentAddr, destRegister, sourceRegister, rtRegister);		
						break;
				}
				break;
			case 7:
				
				sourceRegister = (j & 0x1F000000) >> 24;
				rtRegister     = (j & 0x00F80000) >> 19;
				immediate      = (j & 0x0000FFFF);
				instr =          (j & 0x00070000) >> 16;
				switch(instr) {
					//ADDI
					case 0:
						fprintf(*fpDisAssemblyOutput, "\t%d\tADDI R%d, R%d, #%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
						fprintf(*temp, "%d\tADDI\t%d\t%d\t%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
					break;
					//ANDI
					case 1:
						fprintf(*fpDisAssemblyOutput, "\t%d\tANDI R%d, R%d, #%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
						fprintf(*temp, "%d\tANDI\t%d\t%d\t%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
						break;
					//ORI
					case 2:
						fprintf(*fpDisAssemblyOutput, "\t%d\tORI R%d, R%d, #%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
						fprintf(*temp, "%d\tORI\t%d\t%d\t%d\n",  currentAddr, rtRegister, sourceRegister, immediate);		
						break;
					//XORI
					case 3:
						fprintf(*fpDisAssemblyOutput, "\t%d\tXORI R%d, R%d, #%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
						fprintf(*temp, "%d\tXORI\t%d\t%d\t%d\n", currentAddr, rtRegister, sourceRegister, immediate);		
						break;
				}
					break;
		}

	
	} else {
		printf("Inside else");
		long int tp         = strtoul(binaryString, &end2, 2);
		//Just print the data
		fprintf(*fpDisAssemblyOutput, "\t%d\t%ld\n", currentAddr, tp);		
		fprintf(*temp, "%d\t%ld\n", currentAddr, tp);		
	}

}

void static disAssembly(FILE** temp, FILE** inputFile, FILE** fpDisAssemblyOutput, FILE** fpSimOutput) {

	if((*temp) == NULL || (*inputFile) == NULL || (*fpDisAssemblyOutput == NULL) ) {
		printf("Returning in 1");
		return;
	}	
	if((temp == NULL) || inputFile == NULL || fpDisAssemblyOutput == NULL) {
		printf("Returning in 2");
		return;
	}

	char binaryString[100];
	//printf("Inside disassembly");
	/*Keep reading each line of the file and write output 
	 * keep calling disassemble for every string
	 * write the output to disassembly output along with the input
	 * also write only the disassembled output in a seperate
	 * file , to be used later for simulation
	 */
	while(!feof(*inputFile)) {
		if(fgets(binaryString, 100, *inputFile) != NULL) {
			printf("%s", binaryString);	
			disassemble(binaryString, fpDisAssemblyOutput, temp, fpSimOutput);
			currentAddr = currentAddr + 4;		
		}
	}
	execute(temp, fpSimOutput);
} 

int main(int argc, char *argv[]) {

	/*Take the input file name and store it in filename*/
	char *inputFile;
	const char* outputFile = "disassembly.txt";
	const char* temp       = "temp.txt";
	const char* simOutput  = "simulation.txt";
	
	if(argc > 1) {
		inputFile = argv[1];
	}
	else {
		inputFile = "sample.txt";
	}
	/*Initialize a filepointer to read the contents of the file
	 * Create and Open a new file disassembly.txt to write the contents to a new file*/
	
	FILE* fpInput             = fopen(inputFile, "r");
	FILE* fpDisAssemblyOutput = fopen(outputFile, "w+");	
	FILE* fpDisAssemblyTemp   = fopen(temp, "w+");
	FILE* fpSimulationOutput  = fopen(simOutput, "w+");	
	/*Calls the disassemble method to read from the file and to write the output file*/
	disAssembly(&fpDisAssemblyTemp, &fpInput, &fpDisAssemblyOutput, &fpSimulationOutput);	
	
	fclose(fpSimulationOutput);
}
