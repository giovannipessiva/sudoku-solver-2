#ifndef SS_H_
#define SS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define EMPTY "."   //set the char to identify "unknown value" in the file
#define DELIM " "   //set the string used to separate values in the file

#define CHECK_START 4   //cycle when the check for allineation begins

#define LV0 0   //setta numero
#define LV1 1   //unico nella casella
#define LV2 2   //unico nel gruppo
#define LV3 3   //allineamento
#define LV4 4   //coppia unica

#define ERR_FILE 10     //exit value for error with file
#define ERR_ALLOC 11    //exit value for allocation error

#define DEBUG1 0
#define DEBUG2 0  //lev.3
#define DEBUG_LOG 0


/********** prototypes***************/
int prune(int **matrix,int num,int tier,int lastX,int lastY);
int solveRational(int **matrix,int num,int lastX,int lastY);
int solveBruteForce(int **matrix,int num);
int valid();
int validPartially();
void resetPossibilities(int **matrix);
void setLimitations(int pruning);

void printMatrix(FILE *out,int showPossibilities);
void print(const char *text);
void printInt(const char *text,int num);
void printPrune();
void openLogFile();
void closeLogFile();

int solver(int enableBruteForce, int maxCycles);

int checkSquare(int fin);
int getCoord(int *i,int *j);
void warnCol(int i,int j,int k,int val);
void warnRow(int i,int j,int k,int val);

int checkRow(int fin);
void rowWarnSquare(int i,int k,int val);
int checkColumn(int fin);
void columnWarnSquare(int j,int k,int val);

int checkOneValue(int i,int j);
int checkUnicity();
int countIncompleted();
int countValues(int i,int j);
int areEquals(int i,int j);
void areCompleted(int i,int j);
void setNum(int i,int j,int num);
/************************************/


#endif
