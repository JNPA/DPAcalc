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
#include "openCL/openCLPlatform.hpp"
#include "openCL/profileOpenCLCommand.hpp"
#include "base.hpp"

using namespace std;
namespace StatisticTest
{
    class pearsonNormWithGPU2 : public base
    { 
        public:
            virtual void generate ( std::vector<StatisticIndexMatrix> stat, TracesMatrix traces, unsigned long numvalid, int threadID, int myId, int last_batchID );
            pearsonNormWithGPU2 ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* profTimer );
            virtual void init ( int numtraces );
            virtual void computeSummationsPM();
            virtual void computeSummationsTraces(cl::Kernel kernel, std::vector<cl::CommandQueue> queues, std::vector<cl::Buffer> buffers, TracesMatrix traces, int numvalid, int deviceId, int threadID, int batchId);
            Statistic::OpenCL::profileOpenCLCommand* getProfileEvents();
        protected:
            int ocl_pm_idx;
            int ocl_trace_idx;
            int ocl_stat_idx;
            int ocl_outp_idx;
            int ocl_outp2_idx;
            Statistic::OpenCL::profileOpenCLCommand* profileEvents;

            int numt;
            int numkeys;
            int key_size_bytes;
            int numbatches;
            float inv_traces;
            std::vector<TracesMatrix> somatorio_pm;
            std::vector<TracesMatrix> somatorio_pm_pow_2;
            TracesMatrix somatorio_traces;
            TracesMatrix somatorio_traces_pow_2;
    };
}

