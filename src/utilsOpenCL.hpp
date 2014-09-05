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
#pragma once

cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID, char* platformName);
cl_int oclGetDevicesIDs(cl_platform_id* clPlatform, cl_device_type clDeviceType, cl_device_id** clDevices, cl_uint* numDevices);
cl_int oclCreateCommandQueue(cl_context* clContext, cl_uint numDevices, cl_device_id* clDevice, cl_command_queue** clCommandsQueues);
cl_int oclCreateProgramAndKernel(cl_context* clContext, cl_uint count, const char** strings, const char** kernelNames, cl_uint numDevices, cl_device_id* clDevices, cl_program* clPrograms, cl_kernel** clKernels);
cl_int oclCreateBuffers(cl_context* clContext, cl_mem_flags* clFlags, cl_int numFlags, size_t* clBufSizes, cl_int numBufSizes, cl_mem** buffers, cl_int numBuffers);
cl_int oclRWBuffers(cl_bool isWrite, cl_command_queue* clCommandsQueues, cl_mem* buffers, cl_bool blocking_write, void* host_ptr, cl_uint numDevices, size_t* clBufSizes, cl_int numBufSizes);
