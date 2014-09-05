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
#include "input/base.hpp"
#include "gen_intermediate/base.hpp"
#include "gen_powermodel/base.hpp"
#include "statisticaltest/base.hpp"
#include "exec/base.hpp"
#include "output/base.hpp"

using namespace std;

class DPA
{
    public:

        int main ( int argc, char** argv );
        int doRun(int deviceID, int last_batchID);
        void prefetch();
        int nextDevice();
        static DPA* instance() {
            static DPA theInstance;
            return &theInstance;
        }
    protected:
        virtual void ShowCompileTimeOptions();
    protected:
        timerUtil profTimer;

        DataMatrix data;
        IntermediateValueMatrix intval;
        PowerModelMatrix pm;
        unsigned long numbatches;
        ExecMethod::base* exec;
        SamplesInput::base* input;
        GenerateIntermediateValues::base* interm;
        GeneratePowerModel::base* genpm;
        StatisticTest::base* stat;
        Output::base* outp;
        std::mutex* deviceNumMutex;
        int totalDevs;
        int deviceNum;
    private:
        DPA() {}
};
