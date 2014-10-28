
#include "dpacalc.h"
#include "openCLPlatform.hpp"

Statistic::OpenCL::openCLPlatform::openCLPlatform() : initialized(false)
{
    try {
        std::cout << "--- OpenCL initialization ---" << std::endl;
        std::cout << "Getting platforms with GPUs" << std::endl;
        getPlatforms();
        createContexts();
        getDevices();
        std::cout << "----------------------------" << std::endl;
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
        if(err.what() == "clBuildProgram")
            std::cout<< " Error building: \n" << programs[0].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceList[0]) << std::endl;
    }
    initialized = true;
}

cl_int Statistic::OpenCL::openCLPlatform::init() 
{
    try{
        std::cout << "Create the command queues." << std::endl;
        createCommandQueues();
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
    size_t total_required_mem_size = 0;
    cl_int max_mem_size = deviceList[0].getInfo< CL_DEVICE_MAX_MEM_ALLOC_SIZE >();
    for(int i = 0; i < mem_sizes.size(); i++) { 
        if(max_mem_size < mem_sizes[i] ) {
            std::cout << "One buffer exceeded the maximum size permited by the device " << deviceList[0].getInfo< CL_DEVICE_NAME >() << "." << std::endl;
            all_mem_in_gpu = false;
            return;
        }
        total_required_mem_size += mem_sizes[i];
    }
    cl_int max_global_size = deviceList[0].getInfo< CL_DEVICE_GLOBAL_MEM_SIZE >();
    if( max_global_size < total_required_mem_size) {
        std::cout << "The required memory (" << total_required_mem_size << ") exceeds the global memory (" << max_global_size << ") of the device " << deviceList[0].getInfo< CL_DEVICE_NAME >() << "." << std::endl;
        all_mem_in_gpu = false;
        return;
    }
    all_mem_in_gpu = true;
}

void Statistic::OpenCL::openCLPlatform::getPlatforms() 
{
    //Pick a platform
    cl::Platform::get(&platformList);
    if(platformList.size()==0) {
        std::cout<<" No platforms found." << std::endl;
        exit(1);
    }
    std::cout << "Found " << platformList.size() << " platforms." << std::endl;
    cl_uint i;
    for (cl_uint i = 0; i < platformList.size(); i++) {
        cl_uint deviceIdCount = 0;
        std::vector<cl::Device> aux_devices;
        try{
            platformList[i].getDevices(CL_DEVICE_TYPE_GPU, &aux_devices);
            platforms.push_back(platformList[i]);
            std::cout << "Using platform "<<platformList[i].getInfo<CL_PLATFORM_NAME>() << " with "<<platformList[i].getInfo<CL_PLATFORM_VERSION>() << std::endl;
        }
        catch (cl::Error err) {
            if(err.err() != CL_DEVICE_NOT_FOUND) {
                std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
                exit(1);
            }
        }
    }
    std::cout << "Selected " << platforms.size() << " platforms (platforms with one or more GPUs)." << std::endl;
    if ( platforms.size() == 0) {
        std::cout<<" No platforms with GPUs found." << std::endl;
        exit(1);
    }
}

void Statistic::OpenCL::openCLPlatform::getDevices() 
{
    std::cout << "Get all devices from the platform." << std::endl;
    // Query the set of devices attached to the context
    std::vector<cl::Device> tmp;
    int k = 0;
    for(int i = 0; i < contexts.size(); i++) {
        tmp = contexts[i].getInfo<CL_CONTEXT_DEVICES>();
        for(int j = 0; j < tmp.size(); j++) {
            deviceList.push_back(tmp[j]);
            std::cout << "Device name: " << deviceList[k].getInfo< CL_DEVICE_NAME >() << std::endl;
            std::cout << "    - maximum size for each dimension of NDRange: " << deviceList[k].getInfo< CL_DEVICE_MAX_WORK_ITEM_SIZES >()[0] << "|" << deviceList[0].getInfo< CL_DEVICE_MAX_WORK_ITEM_SIZES >()[1]<< "|" << deviceList[0].getInfo< CL_DEVICE_MAX_WORK_ITEM_SIZES >()[2] << std::endl;
            std::cout << "    - maximum workitems within workgroup: " << deviceList[k].getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >() << std::endl;
            k++;
        }
    }
    std::cout << "Found " << deviceList.size() << " devices." <<std::endl;
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

void Statistic::OpenCL::openCLPlatform::createCommandQueues()
{
    for(cl_uint i = 0; i < contexts.size(); i++) {
        std::vector<cl::Device> devices = contexts[i].getInfo<CL_CONTEXT_DEVICES>();
        for(cl_uint j = 0; j < devices.size(); j++) {
            for(int k = 0; k < NUM_THREADS_PER_DEVICE; k++) {
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
    /*
    for(i = 0; i < contexts.size(); i++) {
        programs.push_back(cl::Program(contexts[i], vec_sources));
    }
    Get verbose
    program.build(deviceList, "-cl-nv-verbose");
    */
    /* DIRTY HACK to resolve opencl bug when using similar devices*/
    int devidx = 0;
    for(cl_uint j = 0; j < contexts.size(); j++) {
        std::vector<cl::Device> devices = contexts[j].getInfo<CL_CONTEXT_DEVICES>();
        for(i = 0; i < devices.size(); i++) {
            programs.push_back(cl::Program(contexts[j], vec_sources));
            std::vector<cl::Device> dumb_test;
            dumb_test.push_back(devices[i]);
            programs[devidx].build(dumb_test);
            devidx++;
        }
    }
    for(cl_uint j = 0; j < programs.size(); j++) {
        cl_int status = programs[j].getBuildInfo<CL_PROGRAM_BUILD_STATUS>(deviceList[j]);
        if(status != CL_BUILD_SUCCESS) {
            std::cout << "Build Status: " << status << std::endl;
            std::cout << "Build Options:\t" << programs[j].getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(deviceList[j]) << std::endl;
            std::cout << "Build Log:\t " << programs[j].getBuildInfo<CL_PROGRAM_BUILD_LOG>(deviceList[j]) << std::endl;
        }
    }
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
    return cl::Kernel( programs[deviceId], functionName.c_str() );
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
