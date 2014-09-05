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
#include <bbque/bbque_exc.h>

using bbque::rtlib::BbqueEXC;

namespace ExecMethod
{
	class bbque: public base
	{
		public:
			bbque ( TCLAP::CmdLine& cmd ) : base ( cmd ) {}
			virtual void RunAndWait ( unsigned long numberoftimes );
	};
	class BbqueClass: public BbqueEXC
	{
		public:
			BbqueClass ( std::string const& name,
						 std::string const& recipe,
						 RTLIB_Services_t* rtlib, unsigned long _tot );
		protected:
			unsigned long tot;
			unsigned long num;
			uint8_t numthreads;
			std::mutex mtx;
			RTLIB_ExitCode_t onSetup();
			RTLIB_ExitCode_t onConfigure ( uint8_t awm_id );
			RTLIB_ExitCode_t onRun();
			RTLIB_ExitCode_t onMonitor();

	};
}

