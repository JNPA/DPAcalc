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

#include "cxx11threads.hpp"
#include "main.hpp"
void prefetchthread();
void threadfunction();
unsigned long batchmax = 0;
unsigned long batchcur = 0;
mutex cur_mutex;
void ExecMethod::cxx11threads::RunAndWait ( unsigned long numberoftimes )
{
    batchmax = numberoftimes;
    batchcur = 0;
    int numCPU = procArg.getValue();

    /* Find number of cores */
    /*
    if ( numCPU == 0 ) {
        numCPU = sysconf ( _SC_NPROCESSORS_ONLN );
    }
    */
    int endDev = NUM_THREADS_PER_DEVICE * numDevices;
    int startDev = 0;
//    int endDev = NUM_THREADS_PER_DEVICE * TEST_PARAM_END_DEV;
//    int startDev = NUM_THREADS_PER_DEVICE * TEST_PARAM_START_DEV;
    vector<thread> thrs;
    //thrs.push_back( thread (prefetchthread ) );
    for ( int i = startDev; i < endDev; i++ ) {
        thrs.push_back ( thread ( threadfunction, i ) );
    }
    for ( auto t = thrs.begin(); t != thrs.end(); ++t ) {
        t->join();
    }
}

void prefetchthread()
{
    DPA::instance()->prefetch();
}

void ExecMethod::cxx11threads::threadfunction(int i)
{
    int last_batch_id = -1;
    for ( ;; ) {
        cur_mutex.lock();
        if ( batchcur < batchmax ) {
            ++batchcur;
        } else {
            cur_mutex.unlock();
            break;
        }
        cur_mutex.unlock();
        last_batch_id = DPA::instance()->doRun(i, last_batch_id);
    }
}
