#include <iostream>
#include <string>
#include <cstdio>
#include <cstdint>

using namespace std;

typedef uint8_t u8;
typedef uint16_t u16;

void print_binary_8(u8 number) 
{
    for (int i = 7; i >= 0; --i) 
    {
        putchar((number & (1 << i)) ? '1' : '0');
    }

    putchar('\n');
}

void print_binary_16(u16 number) 
{
    for (int i = 15; i >= 0; --i) 
    {
        putchar((number & (1 << i)) ? '1' : '0');
    }

    putchar('\n');
}

string reg_field_encoding[2][8] =
{
	{"al","cl","dl", "bl", "ah", "ch", "dh", "bh"},
	{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
};

struct instruction_t
{
	u8 opcode; // 6 bits
	u8 d; // 1 bit
	u8 w; // 1 bit
	u8 mod; // 2 bit
	u8 reg; // 3 bit
	u8 rm; // 3 bit
};

instruction_t create_instruction(u16 buffer)
{
	// ORDER
	// byte 2 byte 1 
	// byte 1 - OP OP OP OP OP OP D W
	// byte 2 - MOD MOD REG REG REG R/M R/M R/M
	
	instruction_t ret;
	
	u8 byte1 = buffer & 0x00FF;
	
	ret.opcode = byte1 >> 2;
	ret.d = (byte1 & 0b00000010) >> 1;
	ret.w = byte1 & 0b00000001;

	u8 byte2 = buffer >> 8;
	
	ret.mod = (byte2 & 0b11000000) >> 6;
	ret.reg = (byte2 & 0b00111000) >> 3;
	ret.rm = byte2 & 0b00000111;

	return ret;
}

void decode_and_print(instruction_t instruction)
{
	string inst_type = "mov";

	u8 D;
	u8 S;

	if(instruction.d == 0)
	{
		S = instruction.reg;
		D = instruction.rm;
	}
	else
	{
		S = instruction.rm;
		D = instruction.reg;
	}

	cout << inst_type << " " 
		<< reg_field_encoding[instruction.w][D]
		<< ", " << reg_field_encoding[instruction.w][S]
		<< endl;
}

int main(int argc, char *argv[]) {
	FILE* file = fopen(argv[1], "rb");
    
	if (file == NULL) 
    	{
		perror("Cannot open file");
		return 1;
    	}

	printf("bits 16\n\n");

    	u16 buffer;

    	while (fread(&buffer, sizeof(buffer), 1, file) == 1) 
    	{
		instruction_t instruction = create_instruction(buffer);
		decode_and_print(instruction);
    	}

    	fclose(file);

    	return 0;
}

