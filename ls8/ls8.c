#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

/**
 * Main
 */
int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Error - Invalid input; Expected two arguments. (Ex. `./ls8 examples/mult.ls8`)\n");
    exit(1);
  }
  struct cpu cpu;

  cpu_init(&cpu);
  cpu_load(&cpu, argv[1]);
  cpu_run(&cpu);

  return 0;
}