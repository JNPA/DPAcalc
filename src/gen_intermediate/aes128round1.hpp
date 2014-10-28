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
#include "base.hpp"
using namespace std;
namespace GenerateIntermediateValues
{
    class aes128round1 : public base
    {
        public:

            virtual void generate ();
            virtual void init(unsigned long long _numtraces, DataMatrix _knownData);
            aes128round1 ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* _profTimer);
            Statistic::OpenCL::profileOpenCLCommand* getProfileEvents();
        protected:
            Statistic::OpenCL::profileOpenCLCommand* profileEvents;
            int ocl_kdata_idx;
            int ocl_pm_idx;
    };
}

