#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 /* to read from file */

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for this project */
#define HALT 6
#define NOOP 7
#define NOOPINSTRUCTION 0x1c00000

// Basic struct
typedef struct IFIDStruct {
 int instr;
 int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
 int instr;
 int pcPlus1;
 int readRegA;
 int readRegB;
 int offset;
} IDEXType;

typedef struct EXMEMStruct {
 int instr;
 int branchTarget;
 int aluResult;
 int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
 int instr;
 int writeData;
} MEMWBType;

typedef struct WBENDStruct {
 int instr;
 int writeData;
} WBENDType;

typedef struct stateStruct {
 int pc;
 int instrMem[NUMMEMORY];
 int dataMem[NUMMEMORY];
 int reg[NUMREGS];
 int numMemory;
 IFIDType IFID;
 IDEXType IDEX;
 EXMEMType EXMEM;
 MEMWBType MEMWB;
 WBENDType WBEND;
 int cycles; /* number of cycles run so far */
} stateType;

// Declare functions
void printState(stateType *);
int field0(int);
int field1(int);
int field2(int);
int opcode(int);
void printInstruction(int);
int convertNum(int);
int destReg(int);

// main function
int main(int argc, char *argv[])
{
 // var
 int stall = 0; // for stall cycles
 int instruction;
 char line[MAXLINELENGTH];
 stateType state, newState;
 FILE *filePtr;

 // incorrect type
 if(argc != 2){
  printf("error: usage: %s <machine-code file>\n", argv[0]);
  exit(1);
 }

 // open file to read
 filePtr = fopen(argv[1], "r");
 if(filePtr == NULL){
  printf("error: can't open file %s", argv[1]);
  perror("fopen");
  exit(1);
 }

 // BOOTING
 // 32-bit compute-r
 state.pc = 0;
 state.cycles = 0;
 state.IFID.instr  = NOOPINSTRUCTION;
 state.IDEX.instr  = NOOPINSTRUCTION;
 state.EXMEM.instr = NOOPINSTRUCTION;
 state.MEMWB.instr = NOOPINSTRUCTION;
 state.WBEND.instr = NOOPINSTRUCTION;
 for(int i = 0; i < NUMREGS; i++) state.reg[i] = 0;
 newState.pc = 0;
 newState.cycles = 0;
 newState.IFID.instr  = NOOPINSTRUCTION;
 newState.IDEX.instr  = NOOPINSTRUCTION;
 newState.EXMEM.instr = NOOPINSTRUCTION;
 newState.MEMWB.instr = NOOPINSTRUCTION;
 newState.WBEND.instr = NOOPINSTRUCTION;
 for(int i = 0; i < NUMREGS; i++) newState.reg[i] = 0;

 /* read into instruction memory */
 for(state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
  if(sscanf(line, "%d", state.instrMem+state.numMemory) != 1) {
   printf("error in reading address %d\n", state.numMemory);
   exit(1);
  }
  printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
 }
 printf("%d memory words\n", state.numMemory);
 printf("\tinstruction memory:\n");
 for(int i = 0; i < state.numMemory; i++) {
  printf("\t\tinstrMem[%d] ", i);
  printInstruction(state.instrMem[i]);
 }

 /* copy it to data memory */
 for(int i = 0; i < state.numMemory; i++)
  state.dataMem[i] = state.instrMem[i];

 // RUN
 while (1) {
  printState(&state);
  
  /* check for halt */
  if (opcode(state.MEMWB.instr) == HALT) {
   printf("machine halted\n");
   printf("total of %d cycles executed\n", state.cycles);
   exit(0);
  }

  newState = state;
  newState.cycles++;
  
  /* --------------------- IF stage --------------------- */
  if(stall == 0) {
   if(state.pc < state.numMemory)
    newState.IFID.instr = state.instrMem[state.pc];
   else
    newState.IFID.instr = NOOPINSTRUCTION;
   newState.IFID.pcPlus1 = state.pc + 1;
   newState.pc = state.pc + 1;
  } else {
   newState.IFID.instr = NOOPINSTRUCTION;
   newState.IFID.pcPlus1 = state.pc;
   newState.pc = state.pc;
   stall--;
   if(stall == 0) newState.pc--; // exiting out of stall cycle require this procedure
  }

  /* --------------------- ID stage --------------------- */
  newState.IDEX.instr = state.IFID.instr;
  newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
  newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
  newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
  newState.IDEX.offset = convertNum(field2(state.IFID.instr));

  /* DATA HAZARD */
  // LOAD USE HAZARD
  if((opcode(state.IDEX.instr) == LW) && ((field1(state.IDEX.instr) == field0(state.IFID.instr)) || (field1(state.IDEX.instr) == field1(state.IFID.instr)))) {
   stall = 1;
   newState.pc = state.pc; // freeze
   newState.IDEX.instr = NOOPINSTRUCTION;
   newState.IFID.instr = NOOPINSTRUCTION;
  }
  if((opcode(state.EXMEM.instr) == LW) && ((field1(state.EXMEM.instr) == field0(state.IFID.instr)) || (field1(state.EXMEM.instr) == field1(state.IFID.instr)))) {
   stall = 1;
   newState.pc = state.pc;
   newState.IDEX.instr = NOOPINSTRUCTION;
   newState.IFID.instr = NOOPINSTRUCTION;
  }
  if((opcode(state.WBEND.instr) == LW) && field1(state.WBEND.instr) && (field1(state.WBEND.instr) == field0(state.IFID.instr)))
   newState.IDEX.readRegA = state.WBEND.writeData;
  if((opcode(state.WBEND.instr) == LW) && field1(state.WBEND.instr) && (field1(state.WBEND.instr) == field1(state.IFID.instr)))
   newState.IDEX.readRegB = state.WBEND.writeData;
  if((opcode(state.MEMWB.instr) == LW) && field1(state.MEMWB.instr) && (field1(state.MEMWB.instr) == field0(state.IFID.instr)))
   newState.IDEX.readRegA = state.MEMWB.writeData;
  if((opcode(state.MEMWB.instr) == LW) && field1(state.MEMWB.instr) && (field1(state.MEMWB.instr) == field1(state.IFID.instr)))
   newState.IDEX.readRegB = state.MEMWB.writeData;

  /* --------------------- EX stage --------------------- */
  newState.EXMEM.instr = state.IDEX.instr;
  newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;

  /* caching */
  int tempRegA = state.IDEX.readRegA;
  int tempRegB = state.IDEX.readRegB;
  
  /* DATA HAZARD */
  // FOR regA
  if((opcode(state.EXMEM.instr) < 2) && destReg(state.EXMEM.instr) && (destReg(state.EXMEM.instr) == field0(state.IDEX.instr))) {
   tempRegA = state.EXMEM.aluResult;
  } else {
   if((opcode(state.MEMWB.instr) < 2) && destReg(state.MEMWB.instr) && (destReg(state.MEMWB.instr) == field0(state.IDEX.instr)))
    tempRegA = state.MEMWB.writeData;
  }

  // FOR regB
  if((opcode(state.EXMEM.instr) < 2) && destReg(state.EXMEM.instr) && (destReg(state.EXMEM.instr) == field1(state.IDEX.instr))) {
   tempRegB = state.EXMEM.aluResult;
  } else {
   if((opcode(state.MEMWB.instr) < 2) && destReg(state.MEMWB.instr) && (destReg(state.MEMWB.instr) == field1(state.IDEX.instr)))
    tempRegB = state.MEMWB.writeData;
  }

  switch(opcode(state.IDEX.instr)) {
   case ADD:
    newState.EXMEM.aluResult = tempRegA + tempRegB;
    break;
   case NOR:
    newState.EXMEM.aluResult = ~(tempRegA | tempRegB);
    break;
   case LW:
   case SW:
    newState.EXMEM.aluResult = tempRegA + state.IDEX.offset;
    break;
   case BEQ:
    // use the aluResult as zEro
    newState.EXMEM.aluResult = (tempRegA == tempRegB)? 1 : 0;
    break;
   default:
    break;
  }
  newState.EXMEM.readRegB = tempRegB;

  /* --------------------- MEM stage --------------------- */
  newState.MEMWB.instr = state.EXMEM.instr;
  switch(opcode(state.EXMEM.instr)) {
   case ADD:
   case NOR:
    newState.MEMWB.writeData = state.EXMEM.aluResult;
    break;
   case LW:
    newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
    break;
   case SW:
    newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
    break;
   case BEQ:

    /* BRANCH HAZARD */
    if(state.EXMEM.aluResult) {
     newState.pc = state.EXMEM.branchTarget;
     newState.IFID.instr = NOOPINSTRUCTION;
     newState.IDEX.instr = NOOPINSTRUCTION;
     newState.EXMEM.instr = NOOPINSTRUCTION;
    }
    break;
   default:
    break;
  }

  /* --------------------- WB stage --------------------- */
  newState.WBEND.instr = state.MEMWB.instr;
  newState.WBEND.writeData = state.MEMWB.writeData;
  switch(opcode(state.MEMWB.instr)) {
   case ADD:
   case NOR:
    newState.reg[destReg(state.MEMWB.instr)] = state.MEMWB.writeData;
    break;
   case LW:
    newState.reg[field1(state.MEMWB.instr)] = state.MEMWB.writeData;
    break;
   default:
    break;
  }

  state = newState;
  /* this is the last statement before end of the loop.
  It marks the end of the cycle and updates the
  current state with the values calculated in this
  cycle */
 }
}

// Given function
void
printState(stateType *statePtr)
{
 int i;
 printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
 printf("\tpc %d\n", statePtr->pc);
 printf("\tdata memory:\n");
 
 for (i=0; i<statePtr->numMemory; i++) {
  printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
 }
 printf("\tregisters:\n");
 
 for (i=0; i<NUMREGS; i++) {
  printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
 }
 
 printf("\tIFID:\n");
 printf("\t\tinstruction ");
 printInstruction(statePtr->IFID.instr);
 printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
 printf("\tIDEX:\n");
 printf("\t\tinstruction ");
 printInstruction(statePtr->IDEX.instr);
 printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
 printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
 printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
 printf("\t\toffset %d\n", statePtr->IDEX.offset);
 printf("\tEXMEM:\n");
 printf("\t\tinstruction ");
 printInstruction(statePtr->EXMEM.instr);
 printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
 printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
 printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
 printf("\tMEMWB:\n");
 printf("\t\tinstruction ");
 printInstruction(statePtr->MEMWB.instr);
 printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
 printf("\tWBEND:\n");
 printf("\t\tinstruction ");
 printInstruction(statePtr->WBEND.instr);
 printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
 return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
 return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
 return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
 return(instruction>>22);
}

void
printInstruction(int instr)
{
 char opcodeString[10];
 if (opcode(instr) == ADD) {
  strcpy(opcodeString, "add");
 } else if (opcode(instr) == NOR) {
  strcpy(opcodeString, "nor");
 } else if (opcode(instr) == LW) {
  strcpy(opcodeString, "lw");
 } else if (opcode(instr) == SW) {
  strcpy(opcodeString, "sw");
 } else if (opcode(instr) == BEQ) {
  strcpy(opcodeString, "beq");
 } else if (opcode(instr) == JALR) {
  strcpy(opcodeString, "jalr");
 } else if (opcode(instr) == HALT) {
  strcpy(opcodeString, "halt");
 } else if (opcode(instr) == NOOP) {
  strcpy(opcodeString, "noop");
 } else {
  strcpy(opcodeString, "data");
 }
 printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr), field2(instr));
}

// some additional functions
int
convertNum(int num)
{
 if(num & (1<<15)) {
  num -= (1<<16);
 }
 return(num);
}

int
destReg(int instruction)
{
 return(instruction & 0x7);
}
