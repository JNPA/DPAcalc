#pragma once
#include <string>
#include <fstream>
#include <streambuf>

namespace Statistic 
{
    namespace OpenCL
    {
        class openCLPlatform
        {
        public:
            openCLPlatform(std::vector<std::string> platformName, cl_device_type dev_type);
            virtual cl_int init();
            virtual cl::Kernel getKernel(std::string functionName, int deviceId);
            virtual std::vector<cl::CommandQueue> getCommandQueues();
            virtual std::vector<cl::Buffer> getBuffers();
            virtual int getNumOfDevices();
            virtual cl_int getBufferIndex(string id);
            virtual bool is_sufficient_mem();
            virtual void createBuffer(string id, cl_mem_flags mem_flag, size_t mem_size, void* host_ptr);
            virtual void createBuffer(string id, cl_mem_flags mem_flag, size_t mem_size, void* host_ptr, int numBufs);
            virtual void releaseLastBuffer();
            virtual void addKernel(string file, bool is_file_path);
            virtual void verifyBufferSize();
        protected:
            virtual void getPlatforms(std::vector<std::string> platformNames);
            virtual void getDevices();
            virtual void createContexts();
            virtual void createCommandQueues();
            virtual void createProgram();
        private:
            bool initialized;
            bool all_mem_in_gpu;
            std::map<string, bool> sources;
            std::vector<cl::Platform> platformList;
            std::vector<cl::Device> deviceList;
            std::vector<cl::CommandQueue> commandQueueList;
            std::vector<cl::Buffer> bufferList;
            std::map<string,int> bufferMap;
            std::vector<size_t> mem_sizes;
            std::vector<cl::Platform> platforms;
            std::vector<cl::Context> contexts;
            std::vector<cl::Program> programs;
        };
    }
}