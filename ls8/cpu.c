#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *file)
{
  FILE *fp;
  char line[128];
  int address = 0;

  fp = fopen(file, "r");
  if (fp == NULL) { 
    fprintf(stderr, "Error - Opening File Failed\n"); 
    exit (1); 
  } 
  while (fgets(line, 128, fp) != NULL) {
    char *endptr;
    unsigned char value = strtoul(line, &endptr, 2);

    if (line == endptr) {
      continue;
    }

    cpu->ram[address++] = value;
    
  }

  fclose(fp);

}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      cpu->reg[regA] = cpu->reg[regA] * cpu->reg[regB];
      break;
    case ALU_ADD:
      cpu->reg[regA] = cpu->reg[regA] + cpu->reg[regB];
      break;
  }
}

/**
 * RAM 
 */
unsigned char cpu_ram_read(struct cpu *cpu, unsigned int index)
{
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, unsigned int index, unsigned char value)
{
  cpu->ram[index] = value;
}

/**
 * REG handler functions 
 */
void handle_LDI(struct cpu *cpu, unsigned int index, unsigned char value)
{
  cpu->reg[index] = value;
}
void handle_PRN(struct cpu *cpu, unsigned int index)
{
  printf("%d\n", cpu->reg[index]);
}
void handle_PUSH(struct cpu *cpu, unsigned int index)
{
  cpu->reg[7]--;
  cpu_ram_write(cpu, cpu->reg[7], cpu->reg[index]);
}
void handle_POP(struct cpu *cpu, unsigned int index)
{
  cpu->reg[index] = cpu_ram_read(cpu, cpu->reg[7]);
  cpu->reg[7]++;
}
void handle_CALL(struct cpu *cpu, unsigned int index)
{
  cpu->reg[7]--;
  cpu_ram_write(cpu, cpu->reg[7], cpu->pc+2);
  cpu->pc = cpu->reg[index];
}
void handle_RET(struct cpu *cpu)
{
  cpu->pc = cpu_ram_read(cpu, cpu->reg[7]);
  cpu->reg[7]++;
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running) {
    // 1. Get the value of the current instruction (in address PC).
    unsigned char IR = cpu_ram_read(cpu, cpu->pc);

    // 2. Figure out how many operands this next instruction requires
    int operands = IR >> 6;

    // 3. Get the appropriate value(s) of the operands following this instruction
    unsigned char operandA = operands > 0 ? cpu_ram_read(cpu, cpu->pc+1): 0;
    unsigned char operandB = operands > 1 ? cpu_ram_read(cpu, cpu->pc+2): 0;

    // 4. switch() over it to decide on a course of action.
    switch (IR) {
      // 5. Do whatever the instruction should do according to the spec.
      case LDI:
        handle_LDI(cpu, operandA, operandB);
        break;
      case PRN:
        handle_PRN(cpu, operandA);
        break;
      case MUL:
        alu(cpu, ALU_MUL, operandA, operandB);
        break;
      case PUSH:
        handle_PUSH(cpu, operandA);
        break;
      case POP:
        handle_POP(cpu, operandA);
        break;
      case CALL:
        handle_CALL(cpu, operandA);
        break;
      case RET:
        handle_RET(cpu);
        break;
      case ADD:
        alu(cpu, ALU_ADD, operandA, operandB);
        break;
      case HLT:
        running = 0;
        break;
      default:
        printf("Error - Instruction Register Unknown at register PC index {%d}\n", cpu->pc);
        running = 0;
        break;
    }
    // 6. Move the PC to the next instruction.
    // 0bAABCDDDD where B is if alu and C if PC
    int is_pc = (IR >> 4) & 0b0001;
    if (is_pc != 1) {
      cpu->pc += operands+1;
    }

  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  // When the LS-8 is booted, the following steps occur:

  // * `R0`-`R6` are cleared to `0`.
  memset(cpu->reg, 0, sizeof(cpu->reg));

  // * `R7` is set to `0xF4`.
  cpu->reg[7] = 0xF4;

  // * `PC` and `FL` registers are cleared to `0`.
  cpu->pc = 0;
  
  // * RAM is cleared to `0`.
  memset(cpu->ram, 0, sizeof(cpu->ram));
}
