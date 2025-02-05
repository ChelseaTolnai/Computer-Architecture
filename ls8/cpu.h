#ifndef _CPU_H_
#define _CPU_H_

// Holds all information about the CPU
struct cpu {
  // PC
  unsigned int pc;
  // registers (array) -- R0->R7 (8 items)
  unsigned char reg[8];
  // ram (array) -- With 8 bits, our CPU has a total of 256 bytes of memory
  unsigned char ram[256];
};

// ALU operations
// enum alu_op {
// 	ALU_MUL,
//   ALU_ADD
// };

// Instructions

// These use binary literals. If these aren't available with your compiler, hex
// literals should be used.

#define GEN  0b0000
#define HLT  0b00000001
#define LDI  0b10000010
#define PUSH 0b01000101
#define POP  0b01000110 
#define PRN  0b01000111

#define PC   0b0001
#define CALL 0b01010000
#define RET  0b00010001
#define JMP  0b01010100

#define ALU  0b0010
#define ADD  0b10100000
#define SUB  0b10100001
#define MUL  0b10100010
#define DIV  0b10100011


// Function declarations

extern void cpu_load(struct cpu *cpu, char *file);
extern void cpu_init(struct cpu *cpu);
extern void cpu_run(struct cpu *cpu);

#endif
