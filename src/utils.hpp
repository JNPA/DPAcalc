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
#pragma once
#if defined(CONFIG_NO_TRICKS)
template <int Nbytes> void BufferToBitset ( char* buffer, std::bitset<Nbytes* 8>& bitset )
{
	for ( unsigned long dim = 0; dim < Nbytes; dim++ ) {
		if ( dim > 0 ) {
			bitset <<= 8;
		}
		bitset |= *buffer;
		buffer++;
	}
}
template <int Nbytes> void BitsetToBuffer ( std::bitset<Nbytes* 8>& bitset, char* buffer )
{
	for ( unsigned long dim = 0; dim < Nbytes; dim++ ) {
		if ( dim > 0 ) {
			bitset <<= 8;
		}
		*buffer = 0;
		if ( bitset.test ( 0 ) ) { ( *buffer ) = ( *buffer ) | 1; }
		if ( bitset.test ( 1 ) ) { ( *buffer ) = ( *buffer ) | 2; }
		if ( bitset.test ( 2 ) ) { ( *buffer ) = ( *buffer ) | 4; }
		if ( bitset.test ( 3 ) ) { ( *buffer ) = ( *buffer ) | 8; }
		if ( bitset.test ( 4 ) ) { ( *buffer ) = ( *buffer ) | 16; }
		if ( bitset.test ( 5 ) ) { ( *buffer ) = ( *buffer ) | 32; }
		if ( bitset.test ( 6 ) ) { ( *buffer ) = ( *buffer ) | 64; }
		if ( bitset.test ( 7 ) ) { ( *buffer ) = ( *buffer ) | 128; }
		buffer++;
	}
}
#else
#include <string.h>
template <int Nbytes> inline void BufferToBitset ( char* buffer, std::bitset<Nbytes* 8>& bitset )
{
	memcpy ( &bitset, buffer, Nbytes );
}
template <int Nbytes> inline void BitsetToBuffer ( std::bitset<Nbytes* 8>& bitset, char* buffer )
{
	memcpy ( buffer, &bitset, Nbytes );
}
#endif

long timevaldiff ( struct timeval* starttime, struct timeval* finishtime );