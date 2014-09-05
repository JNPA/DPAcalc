/*
This file includes helper functions and the full polynomial multiplication table
*/
#ifndef __AES_HELPERS_H__
#define __AES_HELPERS_H__

#include <stdint.h>
#include <stdio.h>
#include "aes_structs.h"

extern uint8_t Mult2[256];
extern uint8_t Mult3[256];
extern uint8_t Multe[256];
extern uint8_t Mult9[256];
extern uint8_t Multd[256];
extern uint8_t Multb[256];

void PrintVectorFile ( uint8_t* DaStampare, FILE* fp );

int checkAllZero ( uint8_t diff[16] );

void printStateToFile ( unsigned char state[16], FILE* fp );

uint8_t returnDecimalFromString ( char hex );

void PrintVector ( uint8_t* DaStampare );

ColumnStructure_t OneMixColumn ( ColumnStructure_t InValue );

ColumnStructure_t OneInMixColumn ( ColumnStructure_t InValue );

uint8_t PolyMultiply ( uint8_t FirstPoly, uint8_t SecondPoly );

void FillHelperData();

#endif // __AES_HELPERS_H__
