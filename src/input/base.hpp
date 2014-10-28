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
namespace SamplesInput
{
    class base
    {
        public:
            unsigned long long SamplesPerTrace; //from metadata, dimension N of matrix T
            unsigned long long NumTraces; 		//dimension N of matrix T
            unsigned long long CurrentSample;   //dimension N of matrix T
            base ( TCLAP::CmdLine* cmd, timerUtil* _profTimer ) : profTimer(_profTimer) {};
            virtual void init() {
                CurrentSample = 0;
                CurrentId = -1;
                NumTraces = 0;
                SamplesPerTrace = 0;
            };
            virtual unsigned long long read ( unsigned long long* id, TracesMatrix* traces ) = 0;
            virtual DataMatrix readData() = 0;
            virtual void populateQueue() {};
            unsigned long long getNumTraces() { return NumTraces; };
            unsigned long long getSamplesPerTrace() { return SamplesPerTrace; };
        protected:
            timerUtil* profTimer;
            DataMatrix data;
            unsigned long long CurrentId;
    };

}
