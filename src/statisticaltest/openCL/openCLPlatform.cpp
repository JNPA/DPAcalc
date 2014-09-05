
#include "dpacalc.h"
#include "openCLPlatform.hpp"

Statistic::OpenCL::openCLPlatform::openCLPlatform(std::vector<std::string> platformNames, cl_device_type dev_type) : initialized(false)
{
    try {
        std::cout << "Trying to get platform with name " << platformNames[0] << std::endl;
        getPlatforms(platformNames);
        createContexts();
        getDevices();
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
        if(err.what() == "clBuildProgram")
            std::cout<< " Error building: \n" << programs[0].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceList[0]) << std::endl;
    }
    initialized = true;
}

cl_int Statistic::OpenCL::openCLPlatform::init(int num_threads) 
{
    try{
        std::cout << "Create the command queues." << std::endl;
        createCommandQueues(num_threads);
        std::cout << "Create and build the program with all the sources." << std::endl;
        createProgram();
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
        if(err.what() == "clBuildProgram")
            std::cout<< " Error building: \n" << programs[0].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceList[0]) << std::endl;
    }
    initialized = true;
    return CL_SUCCESS;
}

void Statistic::OpenCL::openCLPlatform::verifyBufferSize() 
{
    size_t total_buffer_size = 0;
//    std::cout << "Device max alloc mem: " << deviceList[0].getInfo< CL_DEVICE_MAX_MEM_ALLOC_SIZE >() << std::endl;
    for(int i = 0; i < mem_sizes.size(); i++) { 
//        std::cout << "buffer size: " << mem_sizes[i] << std::endl;
        if(deviceList[0].getInfo< CL_DEVICE_MAX_MEM_ALLOC_SIZE >() < mem_sizes[i] ) {
            all_mem_in_gpu = false;
            return;
        }
        total_buffer_size += mem_sizes[i];
    }
//    std::cout << "Device Global: " << deviceList[0].getInfo< CL_DEVICE_GLOBAL_MEM_SIZE >() << std::endl;
//    std::cout << "BS:" << total_buffer_size << std::endl;
    if( deviceList[0].getInfo< CL_DEVICE_GLOBAL_MEM_SIZE >() < total_buffer_size) {
        all_mem_in_gpu = false;
        return;
    }
    all_mem_in_gpu = true;
}

void Statistic::OpenCL::openCLPlatform::getPlatforms(std::vector<std::string> platformNames) 
{
    //Pick a platform
    cl::Platform::get(&platformList);
    if(platformList.size()==0) {
        std::cout<<" No platforms found." << std::endl;
        exit(1);
    }
    std::cout << "Num platforms: " << platformList.size() << std::endl;
    cl_uint i;
    for (cl_uint j = 0; j < platformNames.size(); j++) {
        for (cl_uint i = 0; i < platformList.size(); i++) {
            std::size_t found = platformList[i].getInfo<CL_PLATFORM_NAME>().find(platformNames[j]);
            if ( found != std::string::npos ) {
                platforms.push_back(platformList[i]);
                std::cout << "Using platform: "<<platformList[i].getInfo<CL_PLATFORM_NAME>()<<"\n";
                std::cout << "Using platform: "<<platformList[i].getInfo<CL_PLATFORM_VERSION>()<<"\n";
                break;
            }
        }
    }
    if ( platforms.size() == 0) {
        platforms.push_back(platformList[0]);
    }
}

void Statistic::OpenCL::openCLPlatform::getDevices() 
{
    std::cout << "Get all devices from the platform." << std::endl;
    // Query the set of devices attached to the context
    std::vector<cl::Device> tmp;
    for(int i = 0; i < contexts.size(); i++) {
        tmp = contexts[i].getInfo<CL_CONTEXT_DEVICES>();
        for(int j = 0; j < tmp.size(); j++) {
            deviceList.push_back(tmp[j]);
        }
    }
    std::cout << "Device list has " << deviceList.size() << " devices." <<std::endl;

    //std::cout << "version of openCL" << deviceList[0].getInfo< CL_DRIVER_VERSION >() << std::endl;
    //std::cout << "Device max registers: " << deviceList[0].getInfo< CL_DEVICE_LOCAL_MEM_SIZE >() << std::endl;
    //std::cout << "Device max global mem: " << deviceList[0].getInfo< CL_DEVICE_GLOBAL_MEM_SIZE >() << std::endl;
}

void Statistic::OpenCL::openCLPlatform::createContexts()
{
    std::cout << "Create the context for all devices." << std::endl;
    for(int i = 0; i < platforms.size(); i++) {
        cl_context_properties cprops[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(platforms[i])(),
            0};
        contexts.push_back(cl::Context(CL_DEVICE_TYPE_GPU, cprops));
    }
}

void Statistic::OpenCL::openCLPlatform::createCommandQueues(int num_threads)
{
    for(cl_uint i = 0; i < contexts.size(); i++) {
        std::vector<cl::Device> devices = contexts[i].getInfo<CL_CONTEXT_DEVICES>();
        for(cl_uint j = 0; j < devices.size(); j++) {
            for(int k = 0; k < num_threads; k++) {
                commandQueueList.push_back( cl::CommandQueue( contexts[i], devices[j], CL_QUEUE_PROFILING_ENABLE ) );
            }
        }
    }
}

void Statistic::OpenCL::openCLPlatform::createProgram()
{
    cl::Program::Sources vec_sources;
    std::vector<std::string> test;
    int i = 0;
    for (std::map<std::string, bool>::iterator it=sources.begin(); it!=sources.end(); ++it) {
        if (it->second) {
            std::ifstream in(it->first);
            std::string contents((std::istreambuf_iterator<char>(in)), 
                                 std::istreambuf_iterator<char>());
            test.push_back(std::string(contents));
            vec_sources.push_back(std::make_pair( test[i].c_str(), test[i].length() ) );
        }
        else {
            vec_sources.push_back(std::make_pair( it->first.c_str(), it->first.length() ) );
        }
        i++;
    }
    for(i = 0; i < contexts.size(); i++) {
        programs.push_back(cl::Program(contexts[i], vec_sources));
    }
    //Get verbose
    //program.build(deviceList, "-cl-nv-verbose");
    /* DIRTY HACK - opencl bug */
    for(cl_uint j = 0; j < contexts.size(); j++) {
        std::vector<cl::Device> devices = contexts[j].getInfo<CL_CONTEXT_DEVICES>();
        for(i = 0; i < devices.size(); i++) {
            std::vector<cl::Device> dumb_test;
            dumb_test.push_back(devices[i]);
            programs[j].build(dumb_test);
        }
    }
    std::cout << "Build Status: " << programs[0].getBuildInfo<CL_PROGRAM_BUILD_STATUS>(deviceList[0]) << std::endl;
    std::cout << "Build Options:\t" << programs[0].getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(deviceList[0]) << std::endl;
    std::cout << "Build Log:\t " << programs[0].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceList[0]) << std::endl;
}

void Statistic::OpenCL::openCLPlatform::createBuffer(string id, cl_mem_flags mem_flag, size_t mem_size, void* host_ptr)
{
    try {
        if(!initialized) {
            throw new cl::Error(-59, "The openCL Platform is not initialized!");
        }
        /*Store the position of the buffer for the first device */
        bufferMap[id] = bufferList.size();
        mem_sizes.push_back(mem_size);
        for(int i = 0; i < contexts.size(); i++) {
            std::vector<cl::Device> devices = contexts[i].getInfo<CL_CONTEXT_DEVICES>();
            for(int j = 0; j < devices.size(); j++) {
                bufferList.push_back( cl::Buffer(contexts[i], mem_flag, mem_size, host_ptr) );
            }
        }
    }
    catch( cl::Error err) {
        std::cout << "Buffer Id:" << id << std::endl;
        std::cout << "Buffer Flag:" << mem_flag << std::endl;
        std::cout << "Buffer Size:" << mem_size << std::endl;
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

void Statistic::OpenCL::openCLPlatform::createBuffer(string id, cl_mem_flags mem_flag, size_t mem_size, void* host_ptr, int numBufs)
{
    try {
        if(!initialized) {
            throw new cl::Error(-59, "The openCL Platform is not initialized!");
        }
        /*Store the position of the buffer for the first device */
        bufferMap[id] = bufferList.size();
        mem_sizes.push_back(mem_size);
        for(int i = 0; i < contexts.size(); i++) {
            std::vector<cl::Device> devices = contexts[i].getInfo<CL_CONTEXT_DEVICES>();
            for(int j = 0; j < devices.size(); j++) {
                for(int k = 0; k < numBufs; k++) {
                    bufferList.push_back( cl::Buffer(contexts[i], mem_flag, mem_size, host_ptr) );
                }
            }
        }
    }
    catch( cl::Error err) {
        std::cout << "Buffer Id:" << id << std::endl;
        std::cout << "Buffer Flag:" << mem_flag << std::endl;
        std::cout << "Buffer Size:" << mem_size << std::endl;
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

void Statistic::OpenCL::openCLPlatform::releaseLastBuffer()
{
    bufferList.erase(bufferList.end() - 1);
}

void Statistic::OpenCL::openCLPlatform::addKernel(string file, bool is_file_path)
{
    sources[file] = is_file_path;
}

cl::Kernel Statistic::OpenCL::openCLPlatform::getKernel(std::string functionName, int deviceId)
{
    cl_uint i, j = 0;
    for(i = 0; i < contexts.size(); i++) {
        j += contexts[i].getInfo<CL_CONTEXT_DEVICES>().size();
        if( deviceId < j ) {
            break;
        }
    }
    return cl::Kernel( programs[i], functionName.c_str() );
}

std::vector<cl::CommandQueue> Statistic::OpenCL::openCLPlatform::getCommandQueues()
{
    return commandQueueList;
}

std::vector<cl::Buffer> Statistic::OpenCL::openCLPlatform::getBuffers()
{
    return bufferList;
}

cl_int Statistic::OpenCL::openCLPlatform::getBufferIndex(string id)
{
    return bufferMap[id];
}

bool Statistic::OpenCL::openCLPlatform::is_sufficient_mem()
{
    return all_mem_in_gpu;
}

int Statistic::OpenCL::openCLPlatform::getNumOfDevices()
{
    return deviceList.size();
}