/*
Copyright 2014 João Amaral

This file is part of DPA Calc.

DPA Calc is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation, either version 3 of the License, 
or (at your option) any later version.

DPA Calc is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with DPA Calc. If not, see http://www.gnu.org/licenses/.
*/

#pragma once
#include "dpacalc.h"
using namespace std;
namespace StatisticTest
{
    class base
    {
        public:
            virtual void generate ( std::vector<StatisticIndexMatrix> stat, TracesMatrix traces, unsigned long numvalid, int deviceID, int myID, int last_batchID ) = 0;
            base ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* _profTimer ) : oclplat ( _oclplat), profTimer(_profTimer) {};
            virtual void init ( int _numtraces) {
                numtraces = _numtraces;
            }
            virtual void computeSummationsPM() = 0;
            long getSpentTime() { return time; };

        protected:
            Statistic::OpenCL::openCLPlatform* oclplat;
            timerUtil* profTimer;
            
            int numtraces;
            PowerModelMatrix pm;
            long time;
    };
}
