/* Assembler code fragment for LC-2K */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXWORDLENGTH 1000

// for convenience
#define MAXLINELENGTH 100
#define OPCODE 4194304
#define REGA 524288
#define REGB 65536

// Declare functions
int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int getReg(char *);

// main function
int main(int argc, char *argv[])
{

 // nessesary variables
 int i, insert;
 int icecube, penguin1, penguin2, igloo; // Penguins live in igloo!
 char *inFileString, *outFileString;
 FILE *inFilePtr, *outFilePtr;
 char label[MAXWORDLENGTH], opcode[MAXWORDLENGTH], arg0[MAXWORDLENGTH],
 arg1[MAXWORDLENGTH], arg2[MAXWORDLENGTH];
 char label_info[MAXLINELENGTH][MAXWORDLENGTH]; // label recorder

 // incorrect type
 if (argc != 3) {
 printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
 argv[0]);
 exit(1);
 }

 // open file to read & write
 inFileString = argv[1];
 outFileString = argv[2];
 inFilePtr = fopen(inFileString, "r");
 if (inFilePtr == NULL) {
 printf("error in opening %s\n", inFileString);
 exit(1);
 }
 outFilePtr = fopen(outFileString, "w");
 if (outFilePtr == NULL) {
 printf("error in opening %s\n", outFileString);
 exit(1);
 }

 /* here is an example for how to use readAndParse to read a line from inFilePtr */

 // penguin1 is a temporary penguin to set label's info
 penguin1 = 0;
 while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
  strcpy(label_info[penguin1], label);
  penguin1++;
 }
 igloo = penguin1;

 rewind(inFilePtr);

 // penguin2 keep track of current location
 penguin2 = 0;
 while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
  if(!strcmp(opcode, "add")) {

   insert = 0 * OPCODE + getReg(arg0) * REGA + getReg(arg1) * REGB + getReg(arg2);
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "nor")) {

   insert = 1 * OPCODE + getReg(arg0) * REGA + getReg(arg1) * REGB + getReg(arg2);
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "lw")) {

   insert = 2 * OPCODE + getReg(arg0) * REGA + getReg(arg1) * REGB;
   if(isNumber(arg2)) { // NUMERIC
    sscanf(arg2, "%d", &icecube);
    insert += icecube;

    // PROFESSOR HINT
    // 2's compliment
    if(icecube < 0) insert += REGB - 1;
   } else { // SYMBOLIC
    for(i = 0; i < igloo; i++) {
     if(!strcmp(label_info[i], arg2)) {
      insert += i; // for lw, absolute

      // PROFESSOR HINT
      if(i < 0) insert += REGB - 1;
      break;
     }
    }
    if(i == igloo) {
     printf("error: label not detected\n%s\n", arg2);
     exit(1);
    }
   }
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "sw")) {

   insert = 3 * OPCODE + getReg(arg0) * REGA + getReg(arg1) * REGB;
   if(isNumber(arg2)) { // NUMERIC
    sscanf(arg2, "%d", &icecube);
    insert += icecube;

    // PROFESSOR HINT
    if(icecube < 0) insert += REGB - 1;
   } else { // SYMBOLIC
    for(i = 0; i < igloo; i++) {
     if(!strcmp(label_info[i], arg2)) {
      insert += i; // for sw, absolute

      // PROFESSOR HINT
      if(i < 0) insert += REGB - 1;
      break;
     }
    }
    if(i == igloo) {
     printf("error: label not detected\n%s\n", arg2);
     exit(1);
    }
   }
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "beq")) {

   insert = 4 * OPCODE + getReg(arg0) * REGA + getReg(arg1) * REGB;
   if(isNumber(arg2)) { // NUMBERIC
    sscanf(arg2, "%d", &icecube);
    insert += icecube;

    // PROFESSOR HINT
    if(icecube < 0) insert += REGB;
   } else { // SYMBOLIC
    for(i = 0; i < igloo; i++) {
     if(!strcmp(label_info[i], arg2)) {
      insert += i - penguin2 - 1; // for beq, relative

      // PROFESSOR HINT
      if(i < penguin2 + 1) insert += REGB;
      break;
     }
    }
    if(i == igloo) {
     printf("error: label not detected\n%s\n", arg2);
     exit(1);
    }
   }
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "jalr")) {

   insert = 5 * OPCODE + getReg(arg0) * REGA + getReg(arg1) * REGB;
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "halt")) {

   insert = 6 * OPCODE;
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, "noop")) {

   insert = 7 * OPCODE;
   fprintf(outFilePtr, "%d\n", insert);

  } else if(!strcmp(opcode, ".fill")) {

   if(isNumber(arg0)) {
    fprintf(outFilePtr, "%s\n", arg0);
   } else {
    for(i = 0; i < igloo; i++) {
     if(!strcmp(label_info[i], arg0)) {
      fprintf(outFilePtr, "%d\n", i);
      break;
     }
    }
    if(i == igloo) {
     printf("error: label not detected\n%s\n", arg0);
     exit(1);
    }
   }

  } else {
   printf("error: unrecognized opcode\n%s\n", opcode);
   exit(1);
  }
  penguin2++;
 }
 fclose(outFilePtr);

 /* this is how to rewind the file ptr so that you start reading from the
 beginning of the file */
 //rewind(inFilePtr);
 /* after doing a readAndParse, you may want to do the following to test the
 opcode */
 //if (!strcmp(opcode, "add")) {
 /* do whatever you need to do for opcode "add" */
 //}
 
 return(0);
}
/*
* Read and parse a line of the assembly-language file. Fields are returned
* in label, opcode, arg0, arg1, arg2 (these strings must have memory already
* allocated to them). *
* Return values:
* 0 if reached end of file
* 1 if all went well *
* exit(1) if line is too long.
*/

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
 char *arg1, char *arg2)
{
 char line[MAXLINELENGTH];
 char *ptr = line;
 /* delete prior values */
 label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
 /* read the line from the assembly-language file */
 if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
 /* reached end of file */
 return(0);
 }
 /* check for line too long (by looking for a \n) */
 if (strchr(line, '\n') == NULL) {
 /* line too long */
 exit(1);
 }
 /* is there a label? */
 ptr = line;
 if (sscanf(ptr, "%[^\t\n\r ]", label)) {
 /* successfully read label; advance pointer over the label */
 ptr += strlen(label);
 }
 /*
 * Parse the rest of the line. Would be nice to have real regular
 * expressions, but scanf will suffice.
 */
 sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
 opcode, arg0, arg1, arg2);
 return(1);
}

// implemented functions
int isNumber(char *string)
{ /* return 1 if string is a number */
 int i;
 return( (sscanf(string, "%d", &i)) == 1);
}

// return register number if it is a number 0 ~ 7
int getReg(char *string)
{
 int a;
 if(!isNumber(string)) {
  printf("error: not a number\n%s\n", string);
  exit(1);
 }
 sscanf(string, "%d", &a);
 if(a > 7) {
  printf("error: invalid register number\n%d\n", a);
  exit(1);
 }
 if(a < 0) {
  printf("error: invalid register number\n%d\n", a);
  exit(1);
 }
 return a;
}
