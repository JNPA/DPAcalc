

#pragma once
#include "dpacalc.h"
using namespace std;

namespace Statistic 
{
    namespace OpenCL
    {
        class profileOpenCLCommand
        {
            public:
                cl::Event* getNewEvent(string description, int deviceId) {
                    cl::Event* my_event = new cl::Event();
                    time_values[description] = my_event;
                    device_events[description] = deviceId;
                    return my_event;
                };
                cl::Event* getEvent(string description) { return time_values[description]; };
                void ShowAllCommandQueuesTimers() {
                    // showing contents:
                    for (std::map<string, cl::Event* >::iterator it=time_values.begin(); it!=time_values.end(); ++it) {
                        std::cout << it->first << std::endl;
                        cl::Event* event = it->second;
                        cl_long queued_time = event->getProfilingInfo<CL_PROFILING_COMMAND_QUEUED>();
                        cl_long submited_time = event->getProfilingInfo<CL_PROFILING_COMMAND_SUBMIT>();
                        cl_long started_time = event->getProfilingInfo<CL_PROFILING_COMMAND_START>();
                        cl_long ended_time = event->getProfilingInfo<CL_PROFILING_COMMAND_END>();
                        std::cout << "Queued;" << queued_time << "; nanoseconds." << std::endl;
                        std::cout << "Submited;" << submited_time << "; nanoseconds." << std::endl;
                        std::cout << "Started;" << started_time << "; nanoseconds." << std::endl;
                        std::cout << "Ended;" << ended_time << "; nanoseconds." << std::endl;
                    }
                };
                /*Nano seconds*/
                cl_long getExecutionTime(string description) {
                    cl::Event* my_event = time_values[description];
                    cl_long started_time = my_event->getProfilingInfo<CL_PROFILING_COMMAND_START>();
                    cl_long ended_time = my_event->getProfilingInfo<CL_PROFILING_COMMAND_END>();
                    return ended_time - started_time;    
                };
                cl_long getStartedTime(string description) {
                    cl::Event* my_event = time_values[description];
                    return my_event->getProfilingInfo<CL_PROFILING_COMMAND_START>();
                };
                cl_long getSubmissionTime(string description) {
                    cl::Event* my_event = time_values[description];
                    return my_event->getProfilingInfo<CL_PROFILING_COMMAND_SUBMIT>();
                };
                cl_long getQueuedTime(string description) {
                    cl::Event* my_event = time_values[description];
                    return my_event->getProfilingInfo<CL_PROFILING_COMMAND_QUEUED>();
                };
                void exportEventTimes(std::map<long, std::vector<string> >* timeLine) {
                    for (std::map<string, cl::Event* >::iterator it=time_values.begin(); it!=time_values.end(); ++it) {
                        cl::Event* event = it->second;
                        cl_long queued_time = event->getProfilingInfo<CL_PROFILING_COMMAND_QUEUED>() / 1000000;
                        cl_long submited_time = event->getProfilingInfo<CL_PROFILING_COMMAND_SUBMIT>() / 1000000;
                        cl_long started_time = event->getProfilingInfo<CL_PROFILING_COMMAND_START>() / 1000000;
                        cl_long ended_time = event->getProfilingInfo<CL_PROFILING_COMMAND_END>() / 1000000;
                        (*timeLine)[queued_time].push_back(it->first + ",Q," + convertInt(device_events[it->first]) );
                        (*timeLine)[submited_time].push_back(it->first + ",SUB," + convertInt(device_events[it->first]));
                        (*timeLine)[started_time].push_back(it->first + ",ST," + convertInt(device_events[it->first]));
                        (*timeLine)[ended_time].push_back(it->first + ",E," + convertInt(device_events[it->first]));
                    }
                };
            protected:
                std::map<string, cl::Event* > time_values;
                std::map<string, int > device_events;
        };
    }
}
