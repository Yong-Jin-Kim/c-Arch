/* LC-2K Instruction-level simulator */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

// Basic struct
typedef struct stateStruct {
 int pc;
 int mem[NUMMEMORY];
 int reg[NUMREGS];
 int numMemory;
} stateType;

// Declare functions
void printState(stateType *);
int convertNum(int);

// main function
int main(int argc, char *argv[])
{
 // necessary variables
 int total_instruction = 0;
 int alive = 1;
 int instruction;
 char line[MAXLINELENGTH];
 stateType state;
 FILE *filePtr;

 // incorrect type
 if(argc != 2){
  printf("error: usage: %s <machine-code file>\n", argv[0]);
  exit(1);
 }

 // open file to read
 filePtr = fopen(argv[1], "r");
 if(filePtr == NULL) {
  printf("error: can't open file %s", argv[1]);
  perror("fopen");
  exit(1);
 }

 // CPU BOOTING
 // a 32-bit computer
 state.pc = 0;
 for(int i = 0; i < NUMREGS; i++) state.reg[i] = 0;

 /* read in the entire machine-code file into memory */
 for(state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
  if(sscanf(line, "%d", state.mem+state.numMemory) != 1) {
   printf("error in reading address %d\n", state.numMemory);
   exit(1);
  }
  printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
 }

 // RUN CPU
 // opcode is (state.mem[state.pc] >> 22)
 while(alive) {
  //while((state.mem[state.pc] >> 22) != 6) {
  /*
   * KEEP in mind
   * printState(&state); 
   * state.pc++; if add, nor, lw, sw, noop
   */
  total_instruction++;
  instruction = state.mem[state.pc];
  printState(&state); // instruction >> 22 = opcode
  switch(instruction >> 22) {
   case 0: // add
    // instruction % 8 = destReg
    // (instruction >> 19) % 8 = regA
    // (instruction >> 16) % 8 = regB
    state.reg[instruction % 8] = state.reg[(instruction >> 19) % 8] + state.reg[(instruction >> 16) % 8];
    state.pc++;
    break;
   case 1: // nor
    state.reg[instruction % 8] = ~( state.reg[(instruction >> 19) % 8] | state.reg[(instruction >> 16) % 8]);
    state.pc++;
    break;
   case 2: // lw
    // instruction % 65536 = offset (16-bit)
    state.reg[(instruction >> 16) % 8] = state.mem[state.reg[(instruction >> 19) % 8] + convertNum(instruction % 65536)];
    state.pc++;
    break;
   case 3: // sw
    state.mem[state.reg[(instruction >> 19) % 8] + convertNum(instruction % 65536)] = state.reg[(instruction >> 16) % 8];
    state.pc++;
    break;
   case 4: // beq
    if(state.reg[(instruction >> 19) % 8] == state.reg[(instruction >> 16) % 8])
     state.pc += convertNum(instruction % 65536);
    state.pc++;
    break;
   case 5: // jalr
    state.reg[(instruction >> 16) % 8] = state.pc + 1;
    state.pc = state.reg[(instruction >> 19) % 8];
    break;
   case 6: // halt
    // ERROR SITUATION
    alive = 0; // dead potato
    state.pc++;
    break;
   case 7: // noop
    state.pc++;
    break;
   default:
    // ERROR SITUATION
    exit(1);
    break;
  }
 }

 // state before halt is executed

 printf("machine halted\n");
 printf("total of %d instructions executed\n", total_instruction);
 printf("final state of machine:\n");

 // print the final halt state
 printState(&state);

 return(0);
}

// print STATE
void printState(stateType *statePtr)
{
 int i;
 printf("\n@@@\nstate:\n");
 printf("\tpc %d\n", statePtr->pc);
 printf("\tmemory:\n");
 for(i=0; i<statePtr->numMemory; i++) {
  printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
 }
 printf("\tregisters:\n");
 for(i=0; i<NUMREGS; i++) {
  printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
 }
 printf("end state\n");
}

// PROFESSOR HINT
int convertNum(int num)
{
 /* convert a 16-bit number into a 32-bit Linux integer */
 if(num & (1<<15)) {
  num -= (1<<16);
 }
 return(num);
}
