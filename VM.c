//Austin Peace
//Sebastian De La Cruz

#include "VM.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

INSTRUCTION instructions[MAX_CODE_LENGTH];
CPU cpu;

int instruction_count;
int ar_count;

int registers[MAX_REGISTERS];
int stack[MAX_STACK_HEIGHT];
int activation_records[MAX_LEXI_LEVELS];

int *output_buffer;
int output_buffer_length;

void get_instructions(FILE *fp);
void init();
int fetch();
int execute();
void print_instructions();
void print_info();
void print_output();
int base(int l, int base);

int run_VM(char *filename, int print_flag) {
	FILE *fp;
	int halt = 0;

	if (filename == NULL) {
		printf("Error no file provided!");
		return -1;
	}

	if ((fp = fopen(filename, "r")) == NULL) {
		printf("Error file does not exist!");
		return -1;
	}

	init();

	get_instructions(fp);

	if (print_flag) {
		printf("\nInstruction    \t\tPC\tBP\tSP\tRegisters\t\tStack\n");
	}

	while (!halt) {
		//print current line to be executed
		if(print_flag)
			printf("%d", cpu.pc);
		fetch();
		halt = execute();
		if(print_flag)
			print_info();
	}

	print_output();

	return 0;
}

void init() {
	cpu.bp = 1;
	cpu.pc = 0;
	cpu.sp = 0;
	cpu.ir = instructions[0];

	ar_count = 0;
	instruction_count = 0;

	memset(registers, 0, sizeof(int) * 16);
	memset(stack, 0, sizeof(int) * MAX_STACK_HEIGHT);
	memset(activation_records, 0, sizeof(int) * MAX_LEXI_LEVELS);

	output_buffer = calloc(MAX_OUTPUT_BUFFER_LENGTH, sizeof(int));
}

//Reads the file given and loads the code to be run as well as the
//number of lines
void get_instructions(FILE *fp) {
	int op, r, l, m;

	while (fscanf(fp,"%d %d %d %d", &op, &r, &l, &m) != EOF){
		instructions[instruction_count].op = op;
		instructions[instruction_count].r = r;
		instructions[instruction_count].l = l;
		instructions[instruction_count].m = m;
		instruction_count++;
	}
}

//Prints the assembly read in by the virtual machine
void print_instructions() {
	int i;

	printf("\nInstructions:\n");

	for (i = 0; i < instruction_count; i++) {
		char op[4];
		int r, l, m;

		strcpy(op, opcodes[instructions[i].op]);
		r = instructions[i].r;
		l = instructions[i].l;
		m = instructions[i].m;
		printf("%d %s %d %d %d\n", i, op, r, l, m);
	}
	printf("\n");
}

//Updates the cpus instruction register and increments the program counter by 1
//return -1 if the pre-fetch program counter is greater than the lines of code read in
//0 otherwise
int fetch() {
	if (cpu.pc < instruction_count) {
		cpu.ir = instructions[cpu.pc];
		cpu.pc += 1;
	} else {
		return -1;
	}
	return 0;
}

//Executes the instruction in the cpus instruction register
//Returns 1 on halt 0 otherwise
int execute() {
	switch (cpu.ir.op) {
		case LIT:
			registers[cpu.ir.r] = cpu.ir.m;
			break;
		case RTN:
			cpu.sp = cpu.bp - 1;
			cpu.bp = stack[cpu.sp + 3];
			cpu.pc = stack[cpu.sp + 4];
			ar_count--;
			break;
		case LOD:
			registers[cpu.ir.r] = stack[base(cpu.ir.l, cpu.bp) + cpu.ir.m];
			break;
		case STO:
			stack[base(cpu.ir.l, cpu.bp) + cpu.ir.m] = registers[cpu.ir.r];
			break;
		case CAL:
			stack[cpu.sp + 1] = 0;						//Space for rtn val
			stack[cpu.sp + 2] = base(cpu.ir.l, cpu.bp); //Static Link
			stack[cpu.sp + 3] = cpu.bp;					//Dynamic Link
			stack[cpu.sp + 4] = cpu.pc;					//Return Address
			cpu.bp = cpu.sp + 1;
			activation_records[ar_count] = cpu.bp;
			ar_count++;
			cpu.pc = cpu.ir.m;
			break;
		case INC:
			cpu.sp += cpu.ir.m;
			break;
		case JMP:
			cpu.pc = cpu.ir.m;
			break;
		case JPC:
			if (registers[cpu.ir.r] == 0)
				cpu.pc = cpu.ir.m;
			break;
		case SIO:
			if (cpu.ir.m == 1){
					if(output_buffer_length + 1 < MAX_OUTPUT_BUFFER_LENGTH)
						output_buffer[output_buffer_length++] = registers[cpu.ir.r];
			} else if (cpu.ir.m == 2) {
				scanf("%d", &registers[cpu.ir.r]);
			} else if (cpu.ir.m == 3){
				return 1; //halt
			}
			break;
		case NEG:
			registers[cpu.ir.r] = -1 * registers[cpu.ir.l];
			break;
		case ADD:
			registers[cpu.ir.r] = registers[cpu.ir.l] + registers[cpu.ir.m];
			break;
		case SUB:
			registers[cpu.ir.r] = registers[cpu.ir.l] - registers[cpu.ir.m];
			break;
		case MUL:
			registers[cpu.ir.r] = registers[cpu.ir.l] * registers[cpu.ir.m];
			break;
		case DIV:
			registers[cpu.ir.r] = registers[cpu.ir.l] / registers[cpu.ir.m];
			break;
		case ODD:
			registers[cpu.ir.r] = registers[cpu.ir.r] % 2;
			break;
		case MOD:
			registers[cpu.ir.r] = registers[cpu.ir.l] % registers[cpu.ir.m];
			break;
		case EQL:
			registers[cpu.ir.r] = registers[cpu.ir.l] == registers[cpu.ir.m];
			break;
		case NEQ:
			registers[cpu.ir.r] = registers[cpu.ir.l] != registers[cpu.ir.m];
			break;
		case LSS:
			registers[cpu.ir.r] = registers[cpu.ir.l] < registers[cpu.ir.m];
			break;
		case LEQ:
			registers[cpu.ir.r] = registers[cpu.ir.l] <= registers[cpu.ir.m];
			break;
		case GTR:
			registers[cpu.ir.r] = registers[cpu.ir.l] > registers[cpu.ir.m];
			break;
		case GEQ:
			registers[cpu.ir.r] = registers[cpu.ir.l] >= registers[cpu.ir.m];
			break;
	}

	return 0;
}

void print_info() {
	int i, ar_printed = 0;

	printf("	%s %d %d %d\t", opcodes[cpu.ir.op], cpu.ir.r, cpu.ir.l, cpu.ir.m);
	printf("%d\t%d\t%d\t", cpu.pc, cpu.bp, cpu.sp);

	for (i = 0; i < MAX_REGISTERS; i++) {
		printf("%d ", registers[i]);
	}
	printf("\t");
	for (i = 1; i <= cpu.sp; i++) {
		if (ar_printed < ar_count && i == activation_records[ar_printed]) {
			printf("|");
			ar_printed++;
		}

		printf("%d ", stack[i]);
	}
	printf("\n");
}

void print_output() {
	int i;
	printf("Program Output: ");
	for(i = 0; i < output_buffer_length; i++)
		printf("%d", output_buffer[i]);

		printf("\n");
}

//Moves down the static chain l times
//Returns the base pointer of the AR at the lexicographical level: l
int base(int l, int base) {
	int b1;
	b1 = base;
	while (l > 0) {
		b1 = stack[b1 + 1];
		l--;
	}
	return b1;
}
