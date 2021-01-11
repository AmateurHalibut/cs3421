/*
 *  CS3421 Assignment 2
 *  Name: Alex Hromada
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assem.h"

#define MAXLINE 80
#define MAXNAME 10
#define MAXREG 5
#define MAXINST 32768
#define MAXLABEL 20
#define MAXIMMJ 26

static Label labels[MAXINST];
static Word words[MAXINST];

static unsigned int instrCount = 0;
static unsigned int dataCount = 0;
static unsigned int numLabels = 0;
static unsigned int numWords = 0;

int main(int argc, char **argv)
{

	static char line[MAXLINE]; // String used when reading each line from infile
	const char *l = ":";	   // delimiter for strtok
	int isInstr = -1;		   // 1 if under .text, 0 if under .data

	FILE *temp = fopen("temp.txt", "w"); // Open temp file for writing

	// Read in .asm file from standard input, find the labels
	// Trim labels and comments and outout to temp file
	while (!feof(stdin))
	{
		if (fgets(line, MAXLINE, stdin) != NULL)
		{
			static char lineCopy[MAXLINE];
			strcpy(&lineCopy[0], &line[0]);

			// Sets flag for the .text directive
			if (strstr(line, ".text") != NULL)
			{
				isInstr = 1;
				fprintf(temp, "%s\n", line);
			}
			// Sets flag for the .data directive
			else if (strstr(line, ".data") != NULL)
			{
				isInstr = 0;
				fprintf(temp, "%s\n", line);
			}
			// Checks for instructions
			else if (isInstr)
			{
				char *lineCpy;
				// Checks for labels in a line, store in a Label struct
				if (strstr(&line[0], l) != NULL)
				{
					Label labelIn;

					lineCpy = strtok(&lineCopy[0], l);
					strcpy(labelIn.label, lineCpy);
					labelIn.address = instrCount;
					labelIn.isText = isInstr;
					labels[numLabels] = labelIn;

					numLabels++;
				}

				lineCpy = trimComments(line); // trim comments
				lineCpy = trimLabel(lineCpy); // trim comments
				fprintf(temp, "%s\n", lineCpy);

				instrCount++;
			}
			// Checks for words
			else if (!isInstr)
			{
				char *lineCpy;
				if (strstr(line, l) != NULL)
				{
					Label labelIn;

					lineCpy = strtok(&lineCopy[0], l);
					strcpy(labelIn.label, lineCpy);
					labelIn.address = -1;
					labelIn.isText = isInstr;
					labels[numLabels] = labelIn;

					numLabels++;
				}

				dataCount++;

				lineCpy = trimComments(line); // trim comments
				processWord(lineCpy);		  // process the words in a line
			}
		}
	}

	fclose(temp);

	printf("%u %u\n", instrCount, numWords);

	temp = fopen("temp.txt", "r"); // Open temp file created by first loop

	unsigned int lineCount = 0;

	// Second loop to process instructions
	while (!feof(temp))
	{
		if (fgets(line, MAXLINE, temp) != NULL)
		{
			// Checks for .text section
			if (strstr(line, ".text") != NULL)
			{
				isInstr = 1;
			}
			// Checks for .data section
			else if (strstr(line, ".data") != NULL)
			{
				isInstr = 0;
			}
			else if (isInstr)
				processLine(line, &lineCount); // Process instructions
											   // encodings, labels, words, &numWords
		}
	}

	fclose(temp);
	remove("temp.txt");

	// Print words in hex
	for (int i = 0; i < numWords; i++)
	{
		if (words[i].isSpace == 1)
			printf("%08x\n", 0);
		else
			printf("%08x\n", words[i].word);
	}

	return 0;
}

/*
	Processes a line of words and stores in a word struct
*/
void processWord(char *line) // , Word *words, unsigned int *numWords
{
	char *lineTrimmed;
	static char numbers[MAXINST];
	static char space[6];
	int wordType = 0;
	const char *delim = ",";

	static char wordLabel[MAXLABEL];
	sscanf(line, "%20[^:]s:", &wordLabel[0]);

	lineTrimmed = trimLabel(line);

	sscanf(lineTrimmed, "%6s  %s", space, numbers);

	// printf("Space and nums:  %s  %s\n", space, numbers);

	// check if .word or .space
	if (strstr(&space[0], ".word"))
		wordType = 0;
	else if (strstr(&space[0], ".space"))
		wordType = 1;

	char *token;

	token = strtok(&numbers[0], delim);

	// Reads in ints from the rest of the line using ',' as a delimiter
	while (token != NULL)
	{
		Word word;

		strcpy(word.label, wordLabel);
		word.word = atoi(token);
		word.isSpace = wordType;
		words[numWords] = word;
		// printf("Label: %s   |  Word:  %d   |   isSpace:   %d   |   numWords: %u\n", words[numWords].label, words[numWords].word, words[numWords].isSpace, numWords);
		token = strtok(NULL, delim);
		numWords++;
	}
}

/*
	Processes an instruction read in
*/
void processLine(char *line, unsigned int *lineCount) // Label *labels, Word *words, unsigned int *numWords, Line *encodings,
{
	static char oper[MAXNAME], rs[MAXREG], rt[MAXREG], rd[MAXREG], immedI[MAXLABEL], immedJ[MAXIMMJ];
	Line instruction;

	// sw, lw
	if (sscanf(line, "%10s $%5[^,],%5[^(]($%5[^)]s", &oper[0], &rt[0], &immedI[0], &rs[0]) == 4)
	{
		// printf("%s %s %s %s |  I Type\n", &oper[0], &rt[0], &immedI[0], &rs[0]);
		instruction.type = 1;
		instruction.IType.data.op = lookUpOp(&oper[0]);
		instruction.IType.data.rt = lookUpReg(&rt[0]);
		instruction.IType.data.rs = lookUpReg(&rs[0]);

		// Search for the label in immedI in Word array
		for (unsigned int i = 0; i < numWords; i++)
		{
			// Null pointer check
			if (!words[i].label)
			{
				break;
			}

			// Label check
			if (!strcmp(&immedI[0], words[i].label))
			{
				instruction.IType.data.immed = words[i].word;
				break;
			}
		}

		// printf("%u\n", instruction.IType.data.op);
		// printf("%u\n", instruction.IType.data.rt);
		// printf("%u\n", instruction.IType.data.rs);
		// printf("%u\n", instruction.IType.data.immed);
		printf("%08x\n", instruction.IType.x);

		*lineCount += 1;
	}

	// addu, and, or, slt, subu
	else if (sscanf(line, "%10s $%5[^,],$%5[^,],$%5s", &oper[0], &rd[0], &rs[0], &rt[0]) == 4)
	{
		// printf("%s %s %s %s |  R Type\n", oper, rd, rs, rt);

		instruction.type = 0;
		instruction.RType.data.op = lookUpOp(&oper[0]);
		instruction.RType.data.rt = lookUpReg(&rt[0]);
		instruction.RType.data.rs = lookUpReg(&rs[0]);
		instruction.RType.data.rd = lookUpReg(&rd[0]);
		instruction.RType.data.shamt = 0;
		instruction.RType.data.func = lookUpFunct(&oper[0]);

		// printf("%u\n", instruction.RType.data.op);
		// printf("%u\n", instruction.RType.data.rs);
		// printf("%u\n", instruction.RType.data.rt);
		// printf("%u\n", instruction.RType.data.rd);
		// printf("%u\n", instruction.RType.data.shamt);
		// printf("%u\n", instruction.RType.data.func);

		printf("%08x\n", instruction.RType.x);

		*lineCount += 1;
	}

	// addiu
	else if (sscanf(line, "%10s $%5[^,],$%5[^,],%20s", &oper[0], &rt[0], &rs[0], &immedI[0]) == 4)
	{
		// printf("%s %s %s %s |  I Type\n", oper, rs, rt, immedI);

		instruction.type = 1;
		instruction.IType.data.op = lookUpOp(&oper[0]);

		// addiu encoding
		if (!strcmp("addiu", &oper[0]))
		{
			instruction.IType.data.rt = lookUpReg(&rt[0]);
			instruction.IType.data.rs = lookUpReg(&rs[0]);
			instruction.IType.data.immed = atoi(&immedI[0]);
		}
		// Branch instruction encoding
		else if (!strcmp("beq", &oper[0]) || !strcmp("bne", &oper[0]))
		{
			instruction.IType.data.rt = lookUpReg(&rs[0]);
			instruction.IType.data.rs = lookUpReg(&rt[0]);
			// Search for label in Label array
			for (unsigned int i = 0; i < numLabels; i++)
			{
				// Null pointer check
				if (!labels[i].label)
				{
					break;
				}

				// Label check
				if (!strcmp(&immedI[0], labels[i].label))
				{
					instruction.IType.data.immed = labels[i].address - *lineCount;
					break;
				}
			}
		}

		// printf("%u\n", instruction.IType.data.op);
		// printf("%u\n", instruction.IType.data.rt);
		// printf("%u\n", instruction.IType.data.rs);
		// printf("%d\n", instruction.IType.data.immed);

		printf("%08x\n", instruction.IType.x);

		*lineCount += 1;
	}

	// mult, div
	else if (sscanf(line, "%10s $%5[^,],$%5s", &oper[0], &rs[0], &rt[0]) == 3)
	{
		instruction.type = 0;
		instruction.RType.data.op = lookUpOp(&oper[0]);
		instruction.RType.data.rt = lookUpReg(&rt[0]);
		instruction.RType.data.rs = lookUpReg(&rs[0]);
		instruction.RType.data.rd = 0;
		instruction.RType.data.shamt = 0;
		instruction.RType.data.func = lookUpFunct(&oper[0]);

		// printf("%s %s %s |  R Type\n", oper, rs, rt);
		// printf("%u\n", instruction.RType.data.op);
		// printf("%u\n", instruction.RType.data.rs);
		// printf("%u\n", instruction.RType.data.rt);
		// printf("%u\n", instruction.RType.data.rd);
		// printf("%u\n", instruction.RType.data.shamt);
		// printf("%u\n", instruction.RType.data.func);

		printf("%08x\n", instruction.RType.x);

		*lineCount += 1;
	}

	// mfhi, mflo
	else if (sscanf(line, "%10s $%5s", &oper[0], &rd[0]) == 2)
	{
		// printf("%s %s |  R Type\n", oper, rd);
		instruction.type = 0;
		instruction.RType.data.op = lookUpOp(&oper[0]);
		instruction.RType.data.rt = 0;
		instruction.RType.data.rs = 0;
		instruction.RType.data.rd = lookUpReg(&rd[0]);
		instruction.RType.data.shamt = 0;
		instruction.RType.data.func = lookUpFunct(&oper[0]);

		// printf("%u\n", instruction.RType.data.op);
		// printf("%u\n", instruction.RType.data.rs);
		// printf("%u\n", instruction.RType.data.rt);
		// printf("%u\n", instruction.RType.data.rd);
		// printf("%u\n", instruction.RType.data.shamt);
		// printf("%u\n", instruction.RType.data.func);

		printf("%08x\n", instruction.RType.x);

		*lineCount += 1;
	}

	// j
	else if (sscanf(line, "%10s %20s", &oper[0], &immedJ[0]) == 2)
	{

		// printf("%s %s |  J Type\n", oper, immedJ);
		instruction.JType.data.op = lookUpOp(&oper[0]);

		// Search for label in Label array
		for (unsigned int i = 0; i < numLabels; i++)
		{
			// Null pointer check
			if (!labels[i].label)
			{
				break;
			}

			// Label check
			if (!strcmp(&immedJ[0], labels[i].label))
			{
				instruction.JType.data.immed = labels[i].address;
				break;
			}
		}

		printf("%08x\n", instruction.JType.x);
	}

	// syscall
	else if (sscanf(line, "%10s", &oper[0]) == 1)
	{
		instruction.type = 0;
		instruction.RType.data.op = 0;
		instruction.RType.data.rt = 0;
		instruction.RType.data.rs = 0;
		instruction.RType.data.rd = 0;
		instruction.RType.data.shamt = 0;
		instruction.RType.data.func = 12;
		// printf("%s |  R Type\n", oper);

		printf("%08x\n", instruction.RType.x);

		*lineCount += 1;
	}
}

/*
	Trims the comment from a line
*/
char *trimComments(char *line)
{
	static char lineCopy[MAXLINE];
	strcpy(lineCopy, line);
	char *trimmed;
	const char h = '#';		 // strchr delimiter
	const char *token = "#"; // strtok delimiter

	// Check for comment in line
	if (strchr(lineCopy, h))
	{
		trimmed = strtok(lineCopy, token);
		return trimmed;
	}
	return line;
}

/*
	Trims the label from a line
*/
char *trimLabel(char *line)
{
	static char lineCopy[MAXLINE];
	strcpy(lineCopy, line);
	char *trimmed;
	const char *l = ":"; // strtok delimiter

	// Check if ':' is in the string
	trimmed = strchr(lineCopy, ':');
	if (trimmed != NULL)
	{
		for (int i = 0; i < 2; i++)
		{
			trimmed = strtok(trimmed, l);
		}
		return trimmed;
	}

	else // Return untrimmed line
	{
		return line;
	}
}

/*
	Returns unsigned int value of a given register
*/
unsigned int lookUpReg(char *reg)
{
	if (strstr(reg, "zero") != NULL)
		return 0;
	else if (strstr(reg, "at") != NULL)
		return 1;
	else if (strstr(reg, "v0") != NULL)
		return 2;
	else if (strstr(reg, "v1") != NULL)
		return 3;
	else if (strstr(reg, "a0") != NULL)
		return 4;
	else if (strstr(reg, "a1") != NULL)
		return 5;
	else if (strstr(reg, "a2") != NULL)
		return 6;
	else if (strstr(reg, "a3") != NULL)
		return 7;
	else if (strstr(reg, "t0") != NULL)
		return 8;
	else if (strstr(reg, "t1") != NULL)
		return 9;
	else if (strstr(reg, "t2") != NULL)
		return 10;
	else if (strstr(reg, "t3") != NULL)
		return 11;
	else if (strstr(reg, "t4") != NULL)
		return 12;
	else if (strstr(reg, "t5") != NULL)
		return 13;
	else if (strstr(reg, "t6") != NULL)
		return 14;
	else if (strstr(reg, "t7") != NULL)
		return 15;
	else if (strstr(reg, "t8") != NULL)
		return 24;
	else if (strstr(reg, "t9") != NULL)
		return 25;
	else if (strstr(reg, "s0") != NULL)
		return 16;
	else if (strstr(reg, "s1") != NULL)
		return 17;
	else if (strstr(reg, "s2") != NULL)
		return 18;
	else if (strstr(reg, "s3") != NULL)
		return 19;
	else if (strstr(reg, "s4") != NULL)
		return 20;
	else if (strstr(reg, "s5") != NULL)
		return 21;
	else if (strstr(reg, "s6") != NULL)
		return 22;
	else if (strstr(reg, "s7") != NULL)
		return 23;
	else if (strstr(reg, "k0") != NULL)
		return 26;
	else if (strstr(reg, "k1") != NULL)
		return 27;
	else if (strstr(reg, "gp") != NULL)
		return 28;
	else if (strstr(reg, "sp") != NULL)
		return 29;
	else if (strstr(reg, "fp") != NULL)
		return 30;
	else if (strstr(reg, "ra") != NULL)
		return 31;
	return 0;
}

/*
	Returns unsigned int value of a given opcode
*/
unsigned int lookUpOp(char *op)
{
	if (strstr(op, "addiu") != NULL)
		return 9;
	else if (strstr(op, "addu") != NULL)
		return 0;
	else if (strstr(op, "and") != NULL)
		return 0;
	else if (strstr(op, "beq") != NULL)
		return 4;
	else if (strstr(op, "bne") != NULL)
		return 5;
	else if (strstr(op, "div") != NULL)
		return 0;
	else if (strstr(op, "j") != NULL)
		return 2;
	else if (strstr(op, "lw") != NULL)
		return 35;
	else if (strstr(op, "mfhi") != NULL)
		return 0;
	else if (strstr(op, "mflo") != NULL)
		return 0;
	else if (strstr(op, "mult") != NULL)
		return 0;
	else if (strstr(op, "or") != NULL)
		return 0;
	else if (strstr(op, "slt") != NULL)
		return 0;
	else if (strstr(op, "subu") != NULL)
		return 0;
	else if (strstr(op, "sw") != NULL)
		return 43;
	else if (strstr(op, "syscall") != NULL)
		return 0;
	return 0;
}

/*
	Returns unsigned int value of a given fucntion code
*/
unsigned int lookUpFunct(char *funct)
{
	if (strstr(funct, "addu") != NULL)
		return 33;
	else if (strstr(funct, "and") != NULL)
		return 36;
	else if (strstr(funct, "div") != NULL)
		return 26;
	else if (strstr(funct, "mfhi") != NULL)
		return 16;
	else if (strstr(funct, "mflo") != NULL)
		return 18;
	else if (strstr(funct, "mult") != NULL)
		return 24;
	else if (strstr(funct, "or") != NULL)
		return 37;
	else if (strstr(funct, "slt") != NULL)
		return 42;
	else if (strstr(funct, "subu") != NULL)
		return 35;
	return 0;
}
