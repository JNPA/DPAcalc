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
#include "utilsOpenCL.hpp"

cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID, char* platformName)
{

   char chBuffer[1024];
   cl_uint num_platforms;
   cl_platform_id* clPlatformIDs;
   cl_int ciErrNum;
   *clSelectedPlatformID = NULL;
   // Get OpenCL platform count
   ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
   if (ciErrNum != CL_SUCCESS)
   {
     printf(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
     return -1000;
   }
   else
   {
     if(num_platforms == 0)
     {
       printf("No OpenCL platform found!\n\n");
       return -2000;
     }
     else
     {
       // if there's a platform or more, make space for ID's
       if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
       {
         printf("Failed to allocate memory for cl_platform ID's!\n\n");
         return -3000;
       }
       // get platform info for each platform and trap the NVIDIA platform if found
       ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
       for(cl_uint i = 0; i < num_platforms; ++i)
       {
         ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
         if(ciErrNum == CL_SUCCESS)
         {
           if(strstr(chBuffer, platformName) != NULL)
           {
             printf("i:%d\n", i);
             *clSelectedPlatformID = clPlatformIDs[i];
             break;
           }
         }
       }
       // default to zeroeth platform if NVIDIA not found
       if(*clSelectedPlatformID == NULL)
       {
         printf("WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
         *clSelectedPlatformID = clPlatformIDs[0];
       }
       free(clPlatformIDs);
     }
   }
   return CL_SUCCESS;
}

cl_int oclGetDevicesIDs(cl_platform_id* clPlatform, cl_device_type clDeviceType, cl_device_id** clDevices, cl_uint* numDevices)
{
    cl_int ciErrNum;		//status
    //Retrive the number of devices
    ciErrNum = clGetDeviceIDs((*clPlatform), clDeviceType, 0, NULL, numDevices);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout << "Error: Failed to retrieve number of devices!" << std::endl;
        return ciErrNum;
    }
    (*numDevices) = 1;
    //Fill in the devices
    (*clDevices) = (cl_device_id*) malloc(sizeof(cl_device_id) * (*numDevices) );
    ciErrNum = clGetDeviceIDs((*clPlatform), clDeviceType, (*numDevices), (*clDevices), NULL);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout << "Error: Failed to fill in the devices vector!" << std::endl;
        return ciErrNum;		
    }
    return CL_SUCCESS;
}

cl_int oclCreateCommandQueue(cl_context* clContext, cl_uint numDevices, cl_device_id* clDevices, cl_command_queue** clCommandsQueues)
{
    cl_int ciErrNum;
    (*clCommandsQueues) = (cl_command_queue*) malloc(sizeof(cl_command_queue) * numDevices);
    for(int i = 0; i < numDevices; i++) {
        (*clCommandsQueues)[i] = clCreateCommandQueue((*clContext), clDevices[i], 0, &ciErrNum);
        if (ciErrNum != CL_SUCCESS)
        {
            std::cout << "Error: Failed to create a command queue number " << i << "!" << std::endl;
            return ciErrNum;
        }
    }
    return CL_SUCCESS;
}

cl_int oclCreateProgramAndKernel(cl_context* clContext, cl_uint count, const char** strings, const char** kernelNames, cl_uint numDevices, cl_device_id* clDevices, cl_program* clProgram, cl_kernel** clKernels)
{
    cl_int ciErrNum;
    clProgram = (cl_program*) malloc(sizeof(cl_program) * 1);
    (*clKernels) = (cl_kernel*) malloc(sizeof(cl_kernel) * count);
    // Create the compute program from the source buffers
    (*clProgram) = clCreateProgramWithSource((*clContext), count, strings, NULL, &ciErrNum);
    if (ciErrNum != CL_SUCCESS)
    {
        std::cout << "Error: Failed to create compute program!" << std::endl;
        return ciErrNum;
    }
    // Build the program executable
    //
    ciErrNum = clBuildProgram((*clProgram), numDevices, clDevices, NULL, NULL, NULL);
    if (ciErrNum != CL_SUCCESS)
    {
        size_t len;
        char buffer[16384];
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo((*clProgram), clDevices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
        std::cout << buffer << std::endl;
        exit(1);
    }
    for (int i = 0; i < count; i++) {
        // Create the compute kernel in the program we wish to run
        //
        (*clKernels)[i] = clCreateKernel((*clProgram), kernelNames[i], &ciErrNum);
        if (!(*clKernels)[i] || ciErrNum != CL_SUCCESS)
        {
            printf("Error: Failed to create compute kernel!\n");
            return ciErrNum;
        }
    }
    return CL_SUCCESS;
}

cl_int oclCreateBuffers(cl_context* clContext, cl_mem_flags* clFlags, cl_int numFlags, size_t* clBufSizes, cl_int numBufSizes, cl_mem** buffers, cl_int numBuffers)
{
    if(clFlags == NULL || numFlags <= 0 || clBufSizes == NULL || numBufSizes <= 0 || buffers == NULL || numBuffers <= 0) {
        return CL_INVALID_ARG_VALUE;
    }
    (*buffers) = (cl_mem*) malloc(sizeof(cl_mem) * numBuffers);
    cl_int ciErrNum;
    cl_mem_flags flags =clFlags[0];
    size_t size = clBufSizes[0];
    for(int i = 0; i < numBuffers; i++) {
        flags = (i < numFlags) ? clFlags[i] : flags;
        size = (i < numBufSizes) ? clBufSizes[i] : size;
        (*buffers)[i] = clCreateBuffer((*clContext), flags, size, NULL, &ciErrNum);
        if (ciErrNum != CL_SUCCESS) {
            std::cout << "Error: Failed to allocate device memory for the buffer number " << i+1 << "!" << std::endl;
            return ciErrNum;
        }
        std::cout << "Create Buffer number " << i << " sized " << size << "." << std::endl;
    }
    return CL_SUCCESS;
}

cl_int oclRWBuffers(cl_bool isWrite, cl_command_queue* clCommandsQueues, cl_mem* buffers, cl_bool blocking_write, void* host_ptr, cl_uint numDevices, size_t* clBufSizes, cl_int numBufSizes)
{
    cl_int ciErrNum;
    size_t size = clBufSizes[0];
    PowerModelMatrix host = (PowerModelMatrix)host_ptr;
    for(int i = 0; i < numDevices; i++) {
        size = (i < numBufSizes) ? clBufSizes[i] : size;
        if(isWrite)
            ciErrNum = clEnqueueWriteBuffer(clCommandsQueues[i], buffers[i], blocking_write, 0, size, host, 0, NULL, NULL);
        else
            ciErrNum = clEnqueueReadBuffer(clCommandsQueues[i], buffers[i], blocking_write, 0, size, host, 0, NULL, NULL);
        if (ciErrNum != CL_SUCCESS)
        {
            if(isWrite)
                std::cout << "Error: Failed to write the source array in the buffer number " << i+1 << "!" << std::endl;
            else
                std::cout << "Error: Failed to read the buffer number " << i+1 << " to the source array!" << std::endl;
            return ciErrNum;
        }
        std::cout << "I'm " << ( isWrite ? "writing" : "reading" ) << " in buffer number " << i << " sized " << size << "." << std::endl;
        host += ( size / sizeof(float) );
    }
    return CL_SUCCESS;
}

