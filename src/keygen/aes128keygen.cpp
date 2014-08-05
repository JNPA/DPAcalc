/*
Copyright (C) 2012	Massimo Maggi

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "aes128keygen.hpp"
#include <iostream>
using namespace std;
KeyDataType KeyGenerators::aes128keygen::getKeyFromIndex ( KeyIndexType idx )
{
	assert ( idx < KEYNUM );
	std::bitset<KEY_SIZE_BIT> key = std::bitset<KEY_SIZE_BIT> ( idx );
	key <<= shiftbitsArgs.getValue();
	//This can be useful for debugging of understanding how it works
	//std::cout << "Key after shifting:" << key << std::endl;
	return key;
}

void KeyGenerators::aes128keygen::init()
{
	if ( shiftbitsArgs.getValue() > ( KEY_SIZE_BIT - KEY_HYP_BIT ) || shiftbitsArgs.getValue() < 0 ) {
		std::cerr << "You cannot shift more than " << ( KEY_SIZE_BIT - KEY_HYP_BIT ) << " bits because you're guessing " << KEY_HYP_BIT << "bits out of " << KEY_SIZE_BIT << std::endl;
		exit ( 1 );
	}
}

