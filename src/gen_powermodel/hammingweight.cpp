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
#include "dpacalc.h"
#include "hammingweight.hpp"

Statistic::OpenCL::profileOpenCLCommand* GeneratePowerModel::hammingweight::getProfileEvents()
{
    return profileEvents;
}

GeneratePowerModel::hammingweight::hammingweight ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* _profTimer) : base ( cmd, _oclplat, _profTimer ) 
{
    oclplat->addKernel("src/statisticaltest/openCL/generatePMVal.cl", true);
}

void GeneratePowerModel::hammingweight::init(unsigned long long _numtraces)
{
    GeneratePowerModel::base::init(_numtraces);
    profileEvents = new Statistic::OpenCL::profileOpenCLCommand();
    ocl_pm_idx = oclplat->getBufferIndex(OCL_PM_BUFFER_ID);
}

void GeneratePowerModel::hammingweight::generate ()
{
    std::vector<cl::CommandQueue> queues = oclplat->getCommandQueues();
    std::vector<cl::Buffer> buffers = oclplat->getBuffers();
    for(int i = 0; i < oclplat->getNumOfDevices(); i++) {
        int cq_num = i * NUM_THREADS_PER_DEVICE;
        cl::Kernel kernel_pm = oclplat->getKernel("generatePMVal", i);
        for(int j = 0; j < KEY_SIZE_BYTE; j++) {        
            kernel_pm.setArg(0, buffers[ocl_pm_idx + i * KEY_SIZE_BYTE + j]);
            kernel_pm.setArg(1, j);
            profileEvents->getNewEvent(constants::GENERATE_POWER_MATRIX + convertInt(i) + constants::BYTE + convertInt(j), i);
            queues[cq_num].enqueueNDRangeKernel(kernel_pm, cl::NullRange, cl::NDRange(KEY_NUM, numtraces), cl::NullRange, NULL, profileEvents->getEvent(constants::GENERATE_POWER_MATRIX + convertInt(i) + constants::BYTE + convertInt(j)));
        }
    }
}
