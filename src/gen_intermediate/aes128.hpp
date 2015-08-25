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
#include <string>

using namespace std;
namespace GenerateIntermediateValues
{
    class aes128 : public base
    {
        public:

            virtual void generate ();
            virtual void init(unsigned long long _numtraces, DataMatrix _knownData);
            aes128 ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* _profTimer): base ( cmd, _oclplat, _profTimer ),
                interModeArg ( "m", "mode", "Mode used to calculate intermediate value. Available options: 'regSBox' or 'invSBox' using different SBox. [regSBox]", false, "regSBox", "mode-name" ) // set command line argument for intermediate value calculation
                {
                    cmd->add ( interModeArg ); // init command line argument for intermediate value calculation
                };
            Statistic::OpenCL::profileOpenCLCommand* getProfileEvents();
        protected:
            Statistic::OpenCL::profileOpenCLCommand* profileEvents;
            TCLAP::ValueArg<std::string> interModeArg; // command line argument for intermediate value calculation
            std::string kernelName;
            int ocl_kdata_idx;
            int ocl_pm_idx;
    };
}

