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
#include "pearsonNormWithGPU2.hpp"

StatisticTest::pearsonNormWithGPU2::pearsonNormWithGPU2 ( TCLAP::CmdLine* cmd, Statistic::OpenCL::openCLPlatform* _oclplat, timerUtil* _profTimer ) : base ( cmd, _oclplat, _profTimer )
{
    oclplat->addKernel("src/statisticaltest/openCL/computePearsonSummations.cl", true);
    oclplat->addKernel("src/statisticaltest/openCL/efficientMatrixMul.cl", true);
}
void StatisticTest::pearsonNormWithGPU2::init (int _numtraces)
{
    StatisticTest::base::init ( _numtraces );
    profileEvents = new Statistic::OpenCL::profileOpenCLCommand();
    cl_mem_flags flag_trace = CL_MEM_READ_ONLY;
    cl_mem_flags flag_stat = CL_MEM_READ_WRITE;
    size_t size_trace = sizeof(float) * BATCH_SIZE * numtraces;
    size_trace += sizeof(float) * BATCH_SIZE * 2; /*arrays with the sums*/
    size_t size_stat = sizeof(float) * KEY_NUM * BATCH_SIZE; 
    oclplat->createBuffer(OCL_TRACE_BUFFER_ID, flag_trace, size_trace, (void*) NULL, NUM_THREADS_PER_DEVICE);
    oclplat->createBuffer(OCL_STAT_BUFFER_ID, flag_stat, size_stat, (void*) NULL, NUM_THREADS_PER_DEVICE);
    ocl_pm_idx = oclplat->getBufferIndex(OCL_PM_BUFFER_ID);
    ocl_trace_idx = oclplat->getBufferIndex(OCL_TRACE_BUFFER_ID);
    ocl_stat_idx = oclplat->getBufferIndex(OCL_STAT_BUFFER_ID);
    ocl_outp_idx = oclplat->getBufferIndex(OCL_OUTPUT_BUFFER_ID);
    ocl_outp2_idx = oclplat->getBufferIndex(OCL_OUTPUT2_BUFFER_ID);
    inv_traces = 1.0 / numtraces;
    numt = (int) numtraces;
    numkeys = KEY_NUM;
    numbatches = BATCH_SIZE;
}

void StatisticTest::pearsonNormWithGPU2::computeSummationsPM()
{
    std::vector<cl::CommandQueue> queues = oclplat->getCommandQueues();
    std::vector<cl::Buffer> buffers = oclplat->getBuffers();
    long long start_usec, end_usec;
    for(int j = 0; j < oclplat->getNumOfDevices(); j++) {
        int cq_num = j * NUM_THREADS_PER_DEVICE;
        cl::Kernel kernel = oclplat->getKernel("person_corr_red", j);
        int offset_m = 0;
        int offset_sum = KEY_NUM * numtraces;
        int offset_sum_pow_2 = offset_sum + KEY_NUM;
        for(int i = 0; i < KEY_SIZE_BYTE; i++) {
            start_usec = PAPI_get_real_usec();
            //Write the PM matrix to the GPU buffer
            //Call the kernel to compute the summations
            kernel.setArg(0, buffers[ocl_pm_idx + j * KEY_SIZE_BYTE + i]);
            kernel.setArg(1, numkeys);
            kernel.setArg(2, numt);
            kernel.setArg(3, inv_traces);
            kernel.setArg(4, 1);
            kernel.setArg(5, offset_m);
            kernel.setArg(6, offset_sum);
            kernel.setArg(7, offset_sum_pow_2);
            std::string eventName = constants::COMPUTE_PMM_SUMS + convertInt(j) + constants::BYTE + convertInt(i);
            profileEvents->getNewEvent(eventName, j);
            queues[cq_num].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(KEY_NUM), cl::NullRange,NULL,profileEvents->getEvent(eventName));
            end_usec = PAPI_get_real_usec();
            profTimer->addNewTime(eventName + "\n", end_usec - start_usec);
        }
    }
    
}

void StatisticTest::pearsonNormWithGPU2::computeSummationsTraces(cl::Kernel kernel, std::vector<cl::CommandQueue> queues, std::vector<cl::Buffer> buffers, TracesMatrix traces, int numvalid, int deviceId, int threadID, int batchId)
{
    int offset_sum = BATCH_SIZE * numtraces;
    //Executing the GPU Kernel
    kernel.setArg(0, buffers[ocl_trace_idx + threadID]);
    kernel.setArg(1, numbatches);
    kernel.setArg(2, numt);
    kernel.setArg(3, inv_traces);
    kernel.setArg(4, 0);
    kernel.setArg(5, 0);
    kernel.setArg(6, offset_sum);
    kernel.setArg(7, offset_sum + BATCH_SIZE);
    profileEvents->getNewEvent(constants::COMPUTE_TRACE_SUMS + convertInt(batchId), deviceId);
    queues[threadID].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numvalid), cl::NullRange, NULL, profileEvents->getEvent(constants::COMPUTE_TRACE_SUMS + convertInt(batchId)));
}

Statistic::OpenCL::profileOpenCLCommand* StatisticTest::pearsonNormWithGPU2::getProfileEvents()
{
    return profileEvents;
}
 
void StatisticTest::pearsonNormWithGPU2::generate ( std::vector<StatisticIndexMatrix> stat, TracesMatrix traces, long unsigned int numvalid, int threadID, int myId, int last_batchID)
{
    int batchId = myId;
    int deviceId = threadID / NUM_THREADS_PER_DEVICE;
    long long start_usec, end_usec;
    std::vector<cl::CommandQueue> queues = oclplat->getCommandQueues();
    std::vector<cl::Buffer> buffers = oclplat->getBuffers();
    profileEvents->getNewEvent(constants::WRITE_TRACES + convertInt(batchId), deviceId);
    std::vector<cl::Event> wait_list; 
    //if(batchId != 0) {
        //wait_list.push_back(*profileEvents->getEvent(constants::GENERATE_STAT_MATRIX + convertInt(batchId - 1) + constants::BYTE + convertInt(KEY_SIZE_BYTE - 1) ) );
    //}
    try{
        queues[threadID].enqueueWriteBuffer(buffers[ocl_trace_idx + threadID],CL_FALSE,0,sizeof(float) * numtraces * BATCH_SIZE, traces, &wait_list, profileEvents->getEvent(constants::WRITE_TRACES + convertInt(batchId)));
    }catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    assert ( numvalid <= BATCH_SIZE );
    long long times = 0, start_out, end_out;
    //normalize the chunck
    try{
        start_usec = PAPI_get_real_usec();
        //Executing the summation GPU kernel
        computeSummationsTraces(oclplat->getKernel("person_corr_red", deviceId), oclplat->getCommandQueues(), oclplat->getBuffers(), traces, numvalid, deviceId, threadID, batchId);
        end_usec = PAPI_get_real_usec();
        profTimer->addBasicTime(timerUtil::COMP_TRACES_IDX, end_usec - start_usec);
        profTimer->addNewTime("The trace normalization took (microsendos) \n", end_usec - start_usec);
        //Executing the GPU Kernel
        start_usec = PAPI_get_real_usec();
//TODO        std::string kernelName = (numvalid == BATCH_SIZE) ? "matrixMul_new" : "person_corr_new";
        std::string kernelName = "matrixMul_new";
        cl::Kernel kernel = oclplat->getKernel(kernelName, deviceId);
//TODO        cl::NDRange localNDRange = (numvalid == BATCH_SIZE) ? cl::NDRange(16, 16) : cl::NullRange;
        cl::NDRange localNDRange = cl::NDRange(BLOCK_SIZE, BLOCK_SIZE);
        kernel.setArg(0, buffers[ocl_stat_idx + threadID]);
        kernel.setArg(2, buffers[ocl_trace_idx + threadID]);
        kernel.setArg(3, numt);
        kernel.setArg(4, inv_traces);
        int offset_m = 0;
        int offset_sum = KEY_NUM * numtraces;
        int offset_sum_pow_2 = offset_sum + KEY_NUM;
        kernel.setArg(5, offset_m);
        kernel.setArg(6, offset_sum);
        kernel.setArg(7, offset_sum_pow_2);
        int offset_trace_sum = BATCH_SIZE * numtraces;
        kernel.setArg(8, offset_trace_sum);
        kernel.setArg(9, offset_trace_sum + BATCH_SIZE);
        kernel.setArg(10, numkeys);
        kernel.setArg(11, numbatches);
        cl::Kernel kernel_red = oclplat->getKernel("reduce", deviceId);
        int localsize = BLOCK_SIZE;
        int globalsize = BLOCK_SIZE * 32; //TODO use number of compute units
        kernel_red.setArg(0, buffers[ocl_stat_idx + threadID]);
        kernel_red.setArg(1, sizeof(float) * localsize, NULL);
        kernel_red.setArg(2, sizeof(float) * localsize, NULL);
        kernel_red.setArg(3, KEY_NUM * ((int) numvalid));
        kernel_red.setArg(4, buffers[ocl_outp_idx + threadID]);
        kernel_red.setArg(5, buffers[ocl_outp2_idx + threadID]);
        /*Wait for the last correlation is done.*/
        if( ( threadID % NUM_THREADS_PER_DEVICE ) != 0) {
            std::string lastPMeventName = constants::COMPUTE_PMM_SUMS + convertInt(deviceId) + constants::BYTE + convertInt(KEY_SIZE_BYTE - 1);
            profileEvents->getEvent(lastPMeventName)->wait();
        }
        for(int i = 0 ; i < KEY_SIZE_BYTE; i++) {
            kernel.setArg(1, buffers[ocl_pm_idx + deviceId * KEY_SIZE_BYTE + i]);
            kernel.setArg(5, offset_m);
            kernel.setArg(6, offset_sum);
            kernel.setArg(7, offset_sum_pow_2);
            string mul_name = constants::GENERATE_STAT_MATRIX + convertInt(batchId) + constants::BYTE + convertInt(i);
            profileEvents->getNewEvent(mul_name, deviceId);
            queues[threadID].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(KEY_NUM,numvalid), localNDRange, NULL, profileEvents->getEvent(mul_name));
            string stat_name = constants::REDUCE_STAT_MATRIX + convertInt(batchId) + constants::BYTE + convertInt(i);
            profileEvents->getNewEvent(stat_name, deviceId);
            string outp_idx_name = constants::READ_RED_OUTP + convertInt(batchId) + constants::BYTE + convertInt(i);
            //cout << "name:" << outp_idx_name << endl;
            string outp_name = constants::READ_RED_OUTP_IDXS + convertInt(batchId) + constants::BYTE + convertInt(i);
            profileEvents->getNewEvent(outp_idx_name, deviceId);
            profileEvents->getNewEvent(outp_name, deviceId);
            start_out = PAPI_get_real_usec();
            queues[threadID].enqueueNDRangeKernel(kernel_red, cl::NullRange, cl::NDRange(globalsize), cl::NDRange(localsize), NULL, profileEvents->getEvent(stat_name));
            end_out = PAPI_get_real_usec();
            queues[threadID].enqueueReadBuffer(buffers[ocl_outp_idx + threadID],CL_TRUE,0,sizeof(float) * 32,stat[i], NULL, profileEvents->getEvent(outp_name));
            queues[threadID].enqueueReadBuffer(buffers[ocl_outp2_idx + threadID],CL_TRUE,0,sizeof(float) * 32,&stat[i][32], NULL, profileEvents->getEvent(outp_idx_name));
            times += end_out - start_out; 
        } 
        end_usec = PAPI_get_real_usec();
        profTimer->addNewTime("The stat computation took (microsendos) \n", end_usec - start_usec);
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    profTimer->addNewTime("The output reduction took (microseconds)\n " ,times);
}
