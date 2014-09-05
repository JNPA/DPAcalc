#pragma once
#include "aes_primitives.h"

/**
* Runs a full AES 128 encryption, requires an unrolled key
*/

void AES_128 ( uint8_t InValue[], uint8_t UnrolledRoundKey[AES_128_ROUND_KEY_NO][AES_STATE_BYTES_NO], uint8_t OutValue[] );

/**
* Runs a full AES 192 encryption, requires an unrolled key
*/

void AES_192 ( uint8_t InValue[], uint8_t UnrolledRoundKey[AES_128_ROUND_KEY_NO][AES_STATE_BYTES_NO], uint8_t OutValue[] );

/**
* Runs a full AES 256 encryption, requires an unrolled key
*/

void AES_256 ( uint8_t InValue[], uint8_t UnrolledRoundKey[AES_256_ROUND_KEY_NO][AES_STATE_BYTES_NO], uint8_t OutValue[] );
