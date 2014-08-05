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
#include "base.hpp"
#include <fstream>
#include <map>
#include <mutex>
using namespace std;
namespace Output
{
	class coarseresult: public base
	{
		public:
			coarseresult ( TCLAP::CmdLine& cmd, shared_ptr<KeyGenerators::base> _keygen ) : base ( cmd, _keygen ), checkingMutex() {
				best = 0;
				bestval = -1;
			};
			virtual void end();
			virtual void WriteBatch ( unsigned long long id, shared_ptr<StatisticIndexMatrix>& s );
		protected:
			KeyIndexType best;
			StatisticValueType bestval;
			std::mutex checkingMutex;
	};

}
