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
#include "openCL/openCLPlatform.hpp"
#include "baseNorm.hpp"

using namespace std;
namespace StatisticTest
{
    class pearsonNormWithGPUeCPU : public baseNorm
    { 
        public:
            virtual void generate ( std::vector<StatisticIndexMatrix> stat, TracesMatrix traces, unsigned long numvalid );
            pearsonNormWithGPUeCPU ( TCLAP::CmdLine* cmd ) : baseNorm ( cmd ) {};
            virtual void init ( std::vector<PowerModelMatrix> _pm );
            virtual void initGPU();
            virtual void initCPU();
            virtual void computeSummationsPM(int index);
            virtual void enforceSyncofBuffers(int index); 
            virtual void computeSummationsTraces(TracesMatrix traces, int numvalid);
        protected:
            int numt;
            int numkeys;
            int numbatches;
            float inv_traces;
            std::vector<TracesMatrix> all_somatorio_pm;
            std::vector<TracesMatrix> all_somatorio_pm_pow_2;
            TracesMatrix somatorio_traces;
            TracesMatrix somatorio_traces_pow_2;
            Statistic::OpenCL::openCLPlatform* openCLcpuPlat;
            Statistic::OpenCL::openCLPlatform* openCLgpuPlat;
    };
}

