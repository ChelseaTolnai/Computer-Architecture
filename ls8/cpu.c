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
 * GEN handler functions 
 */
void gen(struct cpu *cpu, unsigned int op, unsigned int index, unsigned char value)
{
  switch (op) {
    case HLT:
      break;
    case LDI:
      cpu->reg[index] = value;
      break;
    case PRN:
      printf("%d\n", cpu->reg[index]);
      break;
    case PUSH:
      cpu->reg[7]--;
      cpu_ram_write(cpu, cpu->reg[7], cpu->reg[index]);
      break;
    case POP:
      cpu->reg[index] = cpu_ram_read(cpu, cpu->reg[7]);
      cpu->reg[7]++;
      break;
    default:
      fprintf(stderr, "Error - GEN Instruction Unknown {%d}\n", op);
      exit(1);
      break;
  }
}

/**
 * PC handler functions 
 */
void pc(struct cpu *cpu, unsigned int op, unsigned int index)
{
  switch (op) {
    case CALL:
      cpu->reg[7]--;
      cpu_ram_write(cpu, cpu->reg[7], cpu->pc+2);
      cpu->pc = cpu->reg[index];
      break;
    case RET:
      cpu->pc = cpu_ram_read(cpu, cpu->reg[7]);
      cpu->reg[7]++;
      break;
    case JMP:
      cpu->pc = cpu->reg[index];
      break;
    default:
      fprintf(stderr, "Error - PC Instruction Unknown {%d}\n", op);
      exit(1);
      break;
  }
}

/**
 * ALU
 */
void alu(struct cpu *cpu, unsigned int op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ADD:
      cpu->reg[regA] = cpu->reg[regA] + cpu->reg[regB];
      break;
    case SUB:
      cpu->reg[regA] = cpu->reg[regA] - cpu->reg[regB];
      break;
    case MUL:
      cpu->reg[regA] = cpu->reg[regA] * cpu->reg[regB];
      break;
    case DIV:
      cpu->reg[regA] = cpu->reg[regA] / cpu->reg[regB];
      break;
    default:
      fprintf(stderr, "Error - ALU Instruction Unknown {%d}\n", op);
      exit(1);
      break;
  }
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
    unsigned char operandA = operands > 0 ? cpu_ram_read(cpu, (cpu->pc+1) & 0xff) : 0;
    unsigned char operandB = operands > 1 ? cpu_ram_read(cpu, (cpu->pc+2) & 0xff) : 0;

    // 0bAABCDDDD where AA is operands, B is if alu, C if PC, D is instruction identifier
    int instruction_type = (IR >> 4) & 0b0011; // 0b0000->GEN; 0b0001->PC; 0b0010->ALU

    // 4. switch() over it to decide on a course of action.
    switch (instruction_type) {
      // 5. Do whatever the instruction should do according to the spec.
      case GEN:
        if (IR == HLT) {
          running = 0;
          break;
        }
        gen(cpu, IR, operandA, operandB);
        cpu->pc += operands + 1;
        break;
      case PC:
        pc(cpu, IR, operandA);
        break;
      case ALU:
        if (IR == DIV && operandB == 0) {
          fprintf(stderr, "Error - Cannot divide by 0\n");
          running = 0;
          break;
        }
        alu(cpu, IR, operandA, operandB);
        cpu->pc += operands + 1;
        break;
    }
    // 6. Move the PC to the next instruction.
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
