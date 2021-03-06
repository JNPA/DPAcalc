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

#include "dpacalc.h"
#include "aes128.hpp"

Statistic::OpenCL::profileOpenCLCommand* GenerateIntermediateValues::aes128::getProfileEvents()
{
    return profileEvents;
}


void GenerateIntermediateValues::aes128::init(unsigned long long _numtraces, DataMatrix _knownData)
{
    if ( interModeArg.getValue().c_str() == (std::string)"regSBox" ){
        oclplat->addKernel("src/statisticaltest/openCL/generateInterVal_aes128regSBox.cl", true);
        kernelName = "generateInterVal_aes128regSBox";
    }else if ( interModeArg.getValue().c_str() == (std::string)"invSBox" ){
        oclplat->addKernel("src/statisticaltest/openCL/generateInterVal_aes128invSBox.cl", true);
        kernelName = "generateInterVal_aes128invSBox";
    }else{
        cerr << "Unknown mode to calculate intermediate value: " << interModeArg.getValue().c_str() << endl;
        exit ( 1 );
    }

    cout << "Using mode '" << interModeArg.getValue().c_str() << "' and kernel '" << kernelName << "'" << endl;

    GenerateIntermediateValues::base::init(_numtraces, _knownData);
    profileEvents = new Statistic::OpenCL::profileOpenCLCommand();

    cl_mem_flags flag_kdata = CL_MEM_READ_ONLY;
    cl_mem_flags flag_pm = CL_MEM_READ_WRITE;
    size_t size_kdata = sizeof(uint8_t) * numtraces * DATA_SIZE_BYTE;
    size_t size_pm = sizeof(float) * KEY_NUM * numtraces; /*matrix*/
    size_pm += sizeof(float) * KEY_NUM * 2; /*arrays with the sums*/
    oclplat->createBuffer(OCL_KDATA_BUFFER_ID, flag_kdata, size_kdata, (void*)NULL);
    oclplat->createBuffer(OCL_PM_BUFFER_ID, flag_pm, size_pm, (void*)NULL, 16);

    ocl_kdata_idx = oclplat->getBufferIndex(OCL_KDATA_BUFFER_ID);
    ocl_pm_idx = oclplat->getBufferIndex(OCL_PM_BUFFER_ID);
}

void GenerateIntermediateValues::aes128::generate ()
{
    std::vector<cl::CommandQueue> queues = oclplat->getCommandQueues();
    std::vector<cl::Buffer> buffers = oclplat->getBuffers();    
    /*Separate the bytes*/
    uint8_t* fullaesdata = new uint8_t[DATA_SIZE_BYTE * numtraces];
    int offset = 0;
    for ( int trcidx = 0; trcidx < numtraces; trcidx++, offset += DATA_SIZE_BYTE) {
        BitsetToBuffer<DATA_SIZE_BYTE> ( knownData[trcidx], ( char* ) &fullaesdata[offset] );
    }
    /*Write the bytes to the buffer*/
    for(int i = 0; i < oclplat->getNumOfDevices(); i++) {
        int cq_num = i * NUM_THREADS_PER_DEVICE;
        profileEvents->getNewEvent(constants::WRITE_KNOWNDATA + convertInt(i), i);
        queues[cq_num].enqueueWriteBuffer(buffers[ocl_kdata_idx + i],CL_FALSE,0,sizeof(uint8_t) * numtraces * DATA_SIZE_BYTE, fullaesdata, NULL, profileEvents->getEvent(constants::WRITE_KNOWNDATA + convertInt(i))); 
        cl::Kernel kernel = oclplat->getKernel(kernelName, i);
        for(int j = 0; j < KEY_SIZE_BYTE; j++) {
            kernel.setArg(0, buffers[ocl_kdata_idx + i]);
            kernel.setArg(1, buffers[ocl_pm_idx + i * KEY_SIZE_BYTE + j]);    
            kernel.setArg(2, j);    
            profileEvents->getNewEvent(constants::GENERATE_INTER_VAL + convertInt(i) + constants::BYTE + convertInt(j), i);
            queues[cq_num].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(KEY_NUM, numtraces), cl::NullRange, NULL, profileEvents->getEvent(constants::GENERATE_INTER_VAL + convertInt(i) + constants::BYTE + convertInt(j)));        
        }
    }
}

