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
#include "base.hpp"
#include "dpacalc.h"
#include <bitset>
#include <iostream>
namespace KeyGenerators
{
	class aes128keygen: public base
	{
		public:
			aes128keygen ( TCLAP::CmdLine& cmd ) : base ( cmd ), shiftbitsArgs ( "k", "shiftkey", "Shift key hypotesis bits on the left by X bits", false, 0, "int" ) {
				cmd.add ( shiftbitsArgs );
			};
			virtual void init();
			virtual KeyDataType getKeyFromIndex ( KeyIndexType idx ) ;

		protected:
			TCLAP::ValueArg<int> shiftbitsArgs;
	};
}
