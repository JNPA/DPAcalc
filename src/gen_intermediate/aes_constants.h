#pragma once
#include <stdint.h>

#define AES_128_ROUND_KEY_NO 11
#define AES_192_ROUND_KEY_NO 13
#define AES_256_ROUND_KEY_NO 15
#define AES_STATE_BYTES_NO 16
/* Forward Substitution Box*/

extern uint8_t SBOX[256]; //F

/*Inverse substitution box */
extern uint8_t InSBOX[256];

// The round constant word array, Rcon[i], contains the values given by
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(28)
// Note that i starts at 1, not 0).
extern uint8_t RCon[255];


