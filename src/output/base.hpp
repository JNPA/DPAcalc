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
#include "keygen/base.hpp"
using namespace std;
namespace Output
{
	class base
	{
		public:
			base ( TCLAP::CmdLine& cmd, shared_ptr<KeyGenerators::base> _keygen ) : keygen ( _keygen ) {};
			virtual void init() {};
			virtual void WriteBatch ( unsigned long long id, shared_ptr<StatisticIndexMatrix>& s ) = 0;
			virtual void end() {};

		protected:
			shared_ptr<KeyGenerators::base> keygen;
	};

}
