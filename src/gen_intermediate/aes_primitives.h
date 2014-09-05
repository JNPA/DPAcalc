#pragma once
#include <stdint.h>
#include "aes_constants.h"
#include "aes_helpers.h"


void MixColumns ( uint8_t InValue[] );

void AddRoundKey ( uint8_t InValue[], uint8_t RoundKey[] );

void SubBytes ( uint8_t InValue[] );

void ShiftRows ( uint8_t InValue[] );

void InShiftRows ( uint8_t InValue[] );

void InSubBytes ( uint8_t InValue[] );

void InMixColumns ( uint8_t InValue[] );
//#pragma GCC diagnostic warning "-Warray-bounds"
void KeyExpansion ( uint8_t Key[], uint8_t RoundKey[11][16] );
