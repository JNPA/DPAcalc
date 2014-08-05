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
#include "dpacalc.h"
#include <bitset>
#include <iostream>
namespace KeyGenerators
{
	class base
	{
		public:
			base ( TCLAP::CmdLine& cmd ) {};
			virtual void init() {};
			virtual  KeyDataType getKeyFromIndex ( KeyIndexType idx ) = 0;
			virtual std::string getKeyAsString ( KeyIndexType idx ) {
				std::stringstream s;
				s << std::hex;
				s << idx;
				return s.str();
			};
	};
}
