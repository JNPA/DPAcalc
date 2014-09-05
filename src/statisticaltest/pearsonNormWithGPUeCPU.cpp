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
#include "pearsonNormWithGPUeCPU.hpp"

void StatisticTest::pearsonNormWithGPUeCPU::initGPU()
{
    //All programs with kernels
    std::vector<std::string> sources; 
    sources.push_back("src/statisticaltest/openCL/efficientMatrixMul.cl"); 
    sources.push_back("src/statisticaltest/openCL/matrixMul.cl"); 
    //Vector with the mem flag for each buffer
    std::vector<cl_mem_flags> flags; 
    flags.push_back(CL_MEM_WRITE_ONLY );
    flags.push_back(CL_MEM_READ_ONLY ); 
    flags.push_back(CL_MEM_READ_ONLY ); 
    flags.push_back(CL_MEM_READ_WRITE ); 
    flags.push_back(CL_MEM_READ_WRITE ); 
    flags.push_back(CL_MEM_READ_WRITE ); 
    flags.push_back(CL_MEM_READ_WRITE ); 
    //Vector with the sizes of each buffer
    std::vector<size_t> sizes; 
    sizes.push_back(sizeof(float) * KEY_NUM * BATCH_SIZE); 
    sizes.push_back(sizeof(float) * numtraces * KEY_NUM ); 
    sizes.push_back(sizeof(float) * numtraces * BATCH_SIZE ); 
    sizes.push_back(sizeof(float) * KEY_NUM ); 
    sizes.push_back(sizeof(float) * KEY_NUM ); 
    sizes.push_back(sizeof(float) * BATCH_SIZE ); 
    sizes.push_back(sizeof(float) * BATCH_SIZE ); 
    //Is the string the path to the program or the program itself
    bool read_from_file = true;
    //Init platform 
    openCLgpuPlat = new Statistic::OpenCL::openCLPlatform();
    openCLgpuPlat->init("NVIDIA", CL_DEVICE_TYPE_GPU, sources, read_from_file, flags, sizes);
}

void StatisticTest::pearsonNormWithGPUeCPU::initCPU()
{
    //All programs with kernels
    std::vector<std::string> sources; 
    sources.push_back("src/statisticaltest/openCL/computePearsonSummations.cl");
    //Is the string the path to the program or the program itself
    bool read_from_file = true;
    //Init platform
    openCLcpuPlat = new Statistic::OpenCL::openCLPlatform();
    openCLcpuPlat->init("Intel", CL_DEVICE_TYPE_CPU, sources, read_from_file);
}

void StatisticTest::pearsonNormWithGPUeCPU::init ( std::vector<PowerModelMatrix> _pm )
{
    long long start_usec, end_usec;
    start_usec = PAPI_get_real_usec();
    Statistic::base::init ( _pm );
    inv_traces = 1.0 / numtraces;
    numt = (int) numtraces;
    numkeys = KEY_NUM;
    numbatches = BATCH_SIZE;
    /**************************************************************************************/
    /*********************************** OPENCL INIT **************************************/ 
    /**************************************************************************************/
    initGPU();
    initCPU();
    end_usec = PAPI_get_real_usec();
    addNewTime("The initialization of platforms took (microsendos) \n", end_usec - start_usec);
    start_usec = PAPI_get_real_usec();
    //Normalize all PMs
    for(int i = 0; i < pm.size(); i++) {
        all_somatorio_pm.push_back((TracesMatrix) malloc(sizeof(TraceValueType) * KEY_NUM));
        all_somatorio_pm_pow_2.push_back((TracesMatrix) malloc(sizeof(TraceValueType) * KEY_NUM));
        openCLcpuPlat->createBuffer(CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR, sizeof(float) * numtraces * KEY_NUM, (void *) &pm[i][0]);
        openCLcpuPlat->createBuffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(float) * KEY_NUM, (void *) &all_somatorio_pm[i][0]);
        openCLcpuPlat->createBuffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(float) * KEY_NUM, (void *) &all_somatorio_pm_pow_2[i][0]);
        computeSummationsPM(i);
    }
    for(int i = 0; i < pm.size(); i++) {
        enforceSyncofBuffers(i);
    }
    for(int i = 0; i < pm.size(); i++) {
        openCLcpuPlat->releaseLastBuffer();
        openCLcpuPlat->releaseLastBuffer();
        openCLcpuPlat->releaseLastBuffer();
    }
    somatorio_traces = (TracesMatrix) malloc(sizeof(TraceValueType) * BATCH_SIZE);
    somatorio_traces_pow_2 = (TracesMatrix) malloc(sizeof(TraceValueType) * BATCH_SIZE);
    openCLcpuPlat->createBuffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(float) * BATCH_SIZE, (void *) &somatorio_traces[0]);
    openCLcpuPlat->createBuffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(float) * BATCH_SIZE, (void *) &somatorio_traces_pow_2[0]);
    openCLcpuPlat->createBuffer(CL_MEM_READ_ONLY, sizeof(float), NULL);
    end_usec = PAPI_get_real_usec();
    addNewTime("The computation of summations in GPU took (microsendos) \n", end_usec - start_usec);
}

void StatisticTest::pearsonNormWithGPUeCPU::computeSummationsPM(int index)
{
    std::vector<cl::CommandQueue> queues = openCLcpuPlat->getCommandQueues();
    std::vector<cl::Buffer> buffers = openCLcpuPlat->getBuffers();
    int buf_num = index * 3;
    //Call the kernel to compute the summations
    try {
        cl::Kernel kernel = openCLcpuPlat->getKernel("person_corr_red");
        kernel.setArg(0, buffers[buf_num++]);
        kernel.setArg(1, numkeys);
        kernel.setArg(2, numt);
        kernel.setArg(3, inv_traces);
        kernel.setArg(4, 1);
        kernel.setArg(5, buffers[buf_num++]);
        kernel.setArg(6, buffers[buf_num]);
        queues[0].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(KEY_NUM), cl::NullRange);
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

void StatisticTest::pearsonNormWithGPUeCPU::enforceSyncofBuffers(int index) 
{
    std::vector<cl::CommandQueue> queues = openCLcpuPlat->getCommandQueues();
    std::vector<cl::Buffer> buffers = openCLcpuPlat->getBuffers();
    int buf_num = index * 3 + 1 ;
    // Map cBuffer to host pointer. This enforces a sync with 
    // the host backing space, remember we choose GPU device.
    int * output = (int *) queues[0].enqueueMapBuffer(
        buffers[buf_num],
        CL_TRUE, // block 
        CL_MAP_READ,
        0,
        KEY_NUM * sizeof(float));
    int * output_pow_2 = (int *) queues[0].enqueueMapBuffer(
        buffers[buf_num + 1], 
        CL_TRUE, // block 
        CL_MAP_READ,
        0,
        KEY_NUM * sizeof(float));
    // Finally release our hold on accessing the memory
    cl_int err = queues[0].enqueueUnmapMemObject(
            buffers[buf_num],
            (void *) output);
    if(err != CL_SUCCESS) {
        std::cout << "Unmap the buffers of pm.." << std::endl;
        exit(1);
    }
    err = queues[0].enqueueUnmapMemObject(
            buffers[buf_num + 1],
            (void *) output_pow_2);
    if(err != CL_SUCCESS) {
        std::cout << "Unmap the buffers of pm.." << std::endl;
        exit(1);
    }
}

void StatisticTest::pearsonNormWithGPUeCPU::computeSummationsTraces(TracesMatrix traces, int numvalid)
{
    openCLcpuPlat->releaseLastBuffer();
    openCLcpuPlat->createBuffer(CL_MEM_READ_ONLY  | CL_MEM_USE_HOST_PTR, sizeof(float) * numtraces * BATCH_SIZE, (void *) &traces[0]);
    std::vector<cl::CommandQueue> queues = openCLcpuPlat->getCommandQueues();
    std::vector<cl::Buffer> buffers = openCLcpuPlat->getBuffers();
    try {
        cl::Kernel kernel = openCLcpuPlat->getKernel("person_corr_red");

        //Executing the GPU Kernel
        kernel.setArg(0, buffers[2]);
        kernel.setArg(1, numbatches);
        kernel.setArg(2, numt);
        kernel.setArg(3, inv_traces);
        kernel.setArg(4, 0);
        kernel.setArg(5, buffers[0]);
        kernel.setArg(6, buffers[1]);
        queues[0].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(numvalid), cl::NullRange);
        queues[0].finish();
        // Map cBuffer to host pointer. This enforces a sync with 
        // the host backing space, remember we choose GPU device.
        int * output = (int *) queues[0].enqueueMapBuffer(
            buffers[0],
            CL_TRUE, // block 
            CL_MAP_READ,
            0,
            BATCH_SIZE * sizeof(float));
        int * output_pow_2 = (int *) queues[0].enqueueMapBuffer(
            buffers[1],
            CL_TRUE, // block 
            CL_MAP_READ,
            0,
            BATCH_SIZE * sizeof(float));
        // Finally release our hold on accessing the memory
        cl_int err = queues[0].enqueueUnmapMemObject(
                buffers[0],
                (void *) output);
        if(err != CL_SUCCESS) {
            std::cout << "Unmap the buffers of pm.." << std::endl;
            exit(1);
        }
        err = queues[0].enqueueUnmapMemObject(
                buffers[1],
                (void *) output_pow_2);
        if(err != CL_SUCCESS) {
            std::cout << "Unmap the buffers of pm.." << std::endl;
            exit(1);
        }
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

void StatisticTest::pearsonNormWithGPUeCPU::generate ( std::vector<StatisticIndexMatrix> stat, TracesMatrix traces, long unsigned int numvalid )
{
    long long start_usec, end_usec;
    std::vector<cl::CommandQueue> queues = openCLgpuPlat->getCommandQueues();
    std::vector<cl::Buffer> buffers = openCLgpuPlat->getBuffers();
    queues[0].enqueueWriteBuffer(buffers[2],CL_FALSE,0,sizeof(float) * numtraces * BATCH_SIZE, traces);
    queues[0].finish();
    assert ( numvalid <= BATCH_SIZE );
    //normalize the chunck
    if(numvalid == BATCH_SIZE) {
        try{
            start_usec = PAPI_get_real_usec();
            //Executing the summation GPU kernel
            computeSummationsTraces(traces, numvalid);
            end_usec = PAPI_get_real_usec();
            addNewTime("The trace normalization took (microsendos) \n", end_usec - start_usec);
            start_usec = PAPI_get_real_usec();
            queues[0].enqueueWriteBuffer(buffers[5],CL_FALSE,0,sizeof(float) * numvalid, somatorio_traces);
            queues[0].enqueueWriteBuffer(buffers[6],CL_FALSE,0,sizeof(float) * numvalid, somatorio_traces_pow_2);
            //std::cout << "asdda :" << somatorio_traces[0] << std::endl;
            //std::cout << "asdda :" << somatorio_traces_pow_2[0] << std::endl;
            //Write the PM matrix to the GPU buffer
            for(int i = 0; i < pm.size(); i++) {
                //for( int j = 0; j < 8; j++ ) {
                //    std::cout << "pm:" << pm[i][j * KEY_NUM] << std::endl;
                //    std::cout << "traces:" << traces[j * BATCH_SIZE] << std::endl;
                //}
                //std::cout << "s_pm :" << all_somatorio_pm[i][0] << std::endl;
                //std::cout << "s_pm_2 :" << all_somatorio_pm_pow_2[i][0] << std::endl;
                //std::cout << "s_traces :" << somatorio_traces[0] << std::endl;
                //std::cout << "s_traces_2 :" << somatorio_traces_pow_2[0] << std::endl;
                queues[0].enqueueWriteBuffer(buffers[1],CL_FALSE,0,sizeof(float) * numtraces * KEY_NUM, pm[i]); 
                queues[0].enqueueWriteBuffer(buffers[3],CL_FALSE,0,sizeof(float) * KEY_NUM, all_somatorio_pm[i]);
                queues[0].enqueueWriteBuffer(buffers[4],CL_FALSE,0,sizeof(float) * KEY_NUM, all_somatorio_pm_pow_2[i]);
                queues[0].finish();
                //Executing the GPU Kernel
                cl::Kernel kernel = openCLgpuPlat->getKernel("matrixMul_new");
                kernel.setArg(0, buffers[0]);
                kernel.setArg(1, buffers[1]);
                kernel.setArg(2, buffers[2]);
                kernel.setArg(3, numt);
                kernel.setArg(4, inv_traces);
                kernel.setArg(5, buffers[3]);
                kernel.setArg(6, buffers[4]);
                kernel.setArg(7, buffers[5]);
                kernel.setArg(8, buffers[6]);
                queues[0].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(KEY_NUM,BATCH_SIZE), cl::NDRange(16,16));
                queues[0].finish();
                //Reading the GPU result
                queues[0].enqueueReadBuffer(buffers[0],CL_TRUE,0,sizeof(float) * KEY_NUM * BATCH_SIZE, stat[i]);
                //for(int j = 0 ; j < 9; j++) {
                //    std::cout << "stat:" << stat[i][j] << endl;
                //}
                //exit(1);
            }
            end_usec = PAPI_get_real_usec();
            addNewTime("The stat computation took (microsendos) \n", end_usec - start_usec);
        }
        catch( cl::Error err) {
            std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
        }
    }
    else{
        try{
            std::cout << "Valid number " << numvalid << std::endl;
            start_usec = PAPI_get_real_usec();
            //Executing the summation GPU Kernel
            computeSummationsTraces(traces,numvalid); 
            end_usec = PAPI_get_real_usec();
            addNewTime("The trace normalization took (microsendos) \n", end_usec - start_usec);
            start_usec = PAPI_get_real_usec();
            queues[0].enqueueWriteBuffer(buffers[5],CL_FALSE,0,sizeof(float) * numvalid, somatorio_traces);
            queues[0].enqueueWriteBuffer(buffers[6],CL_FALSE,0,sizeof(float) * numvalid, somatorio_traces_pow_2);
            for(int i = 0; i < pm.size(); i++) {
                queues[0].enqueueWriteBuffer(buffers[1],CL_FALSE,0,sizeof(float) * numtraces * KEY_NUM, pm[i]); 
                queues[0].enqueueWriteBuffer(buffers[3],CL_FALSE,0,sizeof(float) * KEY_NUM, all_somatorio_pm[i]);
                queues[0].enqueueWriteBuffer(buffers[4],CL_FALSE,0,sizeof(float) * KEY_NUM, all_somatorio_pm_pow_2[i]);
                queues[0].finish();

                cl::Kernel kernel = openCLgpuPlat->getKernel("person_corr_new");
                kernel.setArg(0, buffers[1]);
                kernel.setArg(1, buffers[2]);
                kernel.setArg(2, numt);
                kernel.setArg(3, numkeys);
                kernel.setArg(4, numbatches);
                kernel.setArg(5, inv_traces);
                kernel.setArg(6, buffers[0]);
                kernel.setArg(7, buffers[3]);
                kernel.setArg(8, buffers[4]);
                kernel.setArg(9, buffers[5]);
                kernel.setArg(10, buffers[6]);
                queues[0].enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(KEY_NUM,numvalid), cl::NullRange);
                queues[0].finish();

                //Reading the GPU result
                queues[0].enqueueReadBuffer(buffers[0],CL_TRUE,0,sizeof(float) * KEY_NUM * numvalid,stat[i]);
            }
            end_usec = PAPI_get_real_usec();
            addNewTime("The stat computation took (microsendos) \n", end_usec - start_usec);
        }
        catch( cl::Error err) {
            std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
        }
    }
}
