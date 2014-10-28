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

#include "coarseresult.hpp"

Output::coarseresult::coarseresult ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* _profTimer ) : base ( cmd, _oclplat, _profTimer )
{
    cl_mem_flags flag_output = CL_MEM_READ_WRITE;
    size_t size_output = sizeof(float) * (512 / 16);
    oclplat->createBuffer(OCL_OUTPUT_BUFFER_ID, flag_output, size_output, (void*)NULL, NUM_THREADS_PER_DEVICE);
    oclplat->createBuffer(OCL_OUTPUT2_BUFFER_ID, flag_output, size_output, (void*)NULL, NUM_THREADS_PER_DEVICE);
    oclplat->addKernel("src/statisticaltest/openCL/reduceStat.cl", true);
}

void Output::coarseresult::end()
{
    cout << "Secret key hypothesis:" << endl;
    for(int i = 0; i < KEY_SIZE_BYTE; i++) {
        cout << "The best key hypothesis for byte " << i << " is: " << getKeyAsString ( best[i] ) << endl;
//        cout << "and its statistic index is: " << bestval[i] << endl;
    }
//    cout << "Time of CPU in reduction:" << time << endl;
}

void Output::coarseresult::init()
{
    profileEvents = new Statistic::OpenCL::profileOpenCLCommand();
    ocl_outp_idx = oclplat->getBufferIndex(OCL_OUTPUT_BUFFER_ID);
    ocl_outp2_idx = oclplat->getBufferIndex(OCL_OUTPUT2_BUFFER_ID);
    for(int i = 0; i < KEY_SIZE_BYTE; i++) {
        KeyIndexType best_init = 0;
        StatisticValueType bestval_init = -1;
        best.push_back(best_init);
        bestval.push_back(bestval_init);
        std::mutex* mutex = new std::mutex();
        checkingMutex.push_back(mutex);
    }
    time = 0;
}

void Output::coarseresult::WriteBatch ( unsigned long long id, std::vector<StatisticIndexMatrix> s, int length, int deviceID )
{
    //The StatisticIndexMatrix is sized N * K
    int inc = 1;
    long long start_usec, end_usec;
    for(int i = 0; i < KEY_SIZE_BYTE; i++) {
        StatisticValueType max;
        int index = 0;
        start_usec = PAPI_get_real_usec();
        float maxReduction = -2;
        int idexx = -1;
        for(int jj=0; jj < length;jj++) {
            float value = std::abs(s[i][jj]);
            if(value > maxReduction){ maxReduction = value; idexx = s[i][jj+32]; }
        }
//        std::cout << "valores maximo da ronda " << i << " e " << maxReduction << " com o index de " << idexx << std::endl;
        max = maxReduction;
        index = idexx;
        end_usec = PAPI_get_real_usec();
        time += end_usec - start_usec;
        //Update max value
        checkingMutex[i]->lock();
        if ( max > bestval[i] ) {
            bestval[i] = max;
            KeyIndexType maxCol = ROW(index, BATCH_SIZE);
            best[i] = maxCol;
//            std::cout << "Byte " << i << " is now " << maxCol << ". res: " << max << std::endl;
        }
        checkingMutex[i]->unlock();
    }
}

Statistic::OpenCL::profileOpenCLCommand* Output::coarseresult::getProfileEvents()
{
    return profileEvents;
}