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

