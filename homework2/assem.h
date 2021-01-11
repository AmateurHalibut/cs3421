#ifndef ASSEM_H
#define ASSEM_H

// Storage of labels
typedef struct
{
	char label[20];	// Name of the label
	unsigned int address;	// Address of the label
	int isText;		// 1 for .text, 0 for .data

} Label;

// Storage of words
typedef struct
{	
	char label[20];	// Name of a label
	unsigned int word;	// int value of the word
	int isSpace;  // 0 if .word, 1 if .space
} Word;

// Storage for a line of instruction
typedef struct
{
	unsigned int type;  // 0 for R type, 1 for I type, 2 for J type
	unsigned int address;

	// R types
	union
	{
		unsigned int x;
		struct
		{
			unsigned int func : 6;		// function code
			unsigned int shamt : 5;		// shift ammount
			unsigned int rd : 5;
			unsigned int rt : 5;
			unsigned int rs : 5;
			unsigned int op : 6;		// opcode
		} data;
	} RType;

	// I types
	union
	{
		int x;
		struct
		{
			int immed : 16;				// immediate value
			unsigned int rt : 5;		//
			unsigned int rs : 5;
			unsigned int op : 6;
		} data;
	} IType;

	// J types
	union
	{
		unsigned int x;
		struct
		{
			unsigned int immed : 26;  	// immediate value
			unsigned int op : 6;		// opcode
		} data;

	} JType;
} Line;


// Function Prototypes
char *trimComments(char *line);
char *trimLabel(char *line);
void processLine(char *line, unsigned int *lineCount);
unsigned int lookUpReg(char *reg);
unsigned int lookUpOp(char* op);
unsigned int lookUpFunct(char* funct);
void processWord(char *lineCpy); 

#endif