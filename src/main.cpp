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

#include "dpacalc.h"
#include "includes.h"
#include "main.hpp"
#define VERSION "1.0beta"
using namespace std;

int DPA::doRun(int deviceID, int last_batchID) //BBQ-style. This method can be started multiple times in different threads. Elaborates BATCH_SIZE time samples, for each possible key value.
{
    unsigned long long myid;
    long long start_usec, end_usec;
    TracesMatrix traces;
    cout << "Correlating a new batch" << endl;
start_usec = PAPI_get_real_usec();
    int num = input->read ( &myid, &traces );
    if(num < BATCH_SIZE)  {
        num -= num % 16;
        if (num == 0) return myid;
    }
end_usec = PAPI_get_real_usec();
    cout << "Batch Number : " << myid << " and correlating " << num << " samples." << endl;
    cout << "Device ID: " << (deviceID / NUM_THREADS_PER_DEVICE) << "; Thread ID: " << deviceID << endl << endl;
    std::vector<StatisticIndexMatrix> vec_stat;
    //TODO: 16
    for(int i = 0; i < 16; i++) {
        StatisticIndexMatrix sim = (StatisticValueType*) malloc( 32 * 2 * sizeof(StatisticValueType) );
        vec_stat.push_back(sim);
    }
    start_usec = PAPI_get_real_usec();
    stat->generate ( vec_stat, traces, num, deviceID, myid, last_batchID );
    end_usec = PAPI_get_real_usec();
    profTimer.addNewTime("CPU time in Stat\n", end_usec - start_usec);
    start_usec = PAPI_get_real_usec();
    outp->WriteBatch ( myid, vec_stat, 32, deviceID );
    end_usec = PAPI_get_real_usec();
    profTimer.addBasicTime(timerUtil::GEN_OUTPUT_IDX, end_usec - start_usec);
    for(int i = 0; i < 16; i++) {
        free(vec_stat[i]);
    }
    free(traces);
    return myid;
}

void DPA::prefetch()
{
    while ( input->CurrentSample < input->SamplesPerTrace ) {
//		cout << "Reading samples..in sample number: " << input->CurrentSample << endl;
        input->populateQueue();
    }
//	cout << "Read all samples." << endl;
}

int DPA::main ( int argc, char** argv )
{
    /* Init of Library Papi 
     *   time in microseconds
     */
    long long start_usec, end_usec;
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
      exit(1);
  
    start_usec = PAPI_get_real_usec();
    /*****************************************************************/
    /*********************** 1st Part Creation ***********************/
    /*****************************************************************/
    std::cout << "--- Compile Options ---" << std::endl;
    this->ShowCompileTimeOptions();
    std::cout << "-----------------------" << std::endl;
    TCLAP::CmdLine cmd( "DPA calc", ' ', VERSION );
    Statistic::OpenCL::openCLPlatform oclplat = Statistic::OpenCL::openCLPlatform();
    profTimer = timerUtil();
    deviceNum = 0;
    totalDevs = oclplat.getNumOfDevices();
    deviceNumMutex = new std::mutex();
        
    exec = (ExecMethod::base*) new ExecMethod::EXECCLASS ( &cmd );
    input = (SamplesInput::base*) new SamplesInput::INPUTCLASS ( &cmd, &profTimer);
    interm = (GenerateIntermediateValues::base*) new GenerateIntermediateValues::GENINTERMCLASS ( &cmd, &oclplat, &profTimer );
    genpm = (GeneratePowerModel::base*) new GeneratePowerModel::GENPOWERMODELCLASS ( &cmd, &oclplat, &profTimer);
    stat = (StatisticTest::base*) new StatisticTest::STATISTICCLASS ( &cmd, &oclplat, &profTimer );
    outp = (Output::base*) new Output::OUTPUTCLASS ( &cmd, &oclplat, &profTimer );
    try {
        cmd.parse ( argc, argv );
    } catch ( TCLAP::ArgException& e ) {
        cerr << "Error " << e.error() << " in command line argument " << e.argId() << std::endl;
        return 1;
    }
    /*****************************************************************/
    /******************** 2nd Part Initialization ********************/
    /*****************************************************************/
    input->init();
    numbatches = ( input->SamplesPerTrace / BATCH_SIZE ) + ( ( ( input->SamplesPerTrace % BATCH_SIZE ) == 0 ) ? 0 : 1 );
    data = input->readData();
    interm->init(input->getNumTraces(), input->readData());
    genpm->init(input->getNumTraces());
    outp->init();
    stat->init ( (int) input->getNumTraces() );
    exec->init(oclplat.getNumOfDevices());
    oclplat.init();                        //Has to be the last initialization - other classes fill the var sources

    oclplat.verifyBufferSize();
    if( !oclplat.is_sufficient_mem()) {
        std::cout << "Not good choice.." << std::endl; 
        return -1;
    } else {
        std::cout << "Devices' memory can store all the buffers." << std::endl;
    }
    end_usec = PAPI_get_real_usec();
    profTimer.addBasicTime(timerUtil::INIT_IDX, end_usec - start_usec);
    /*****************************************************************/
    /********************** 3rd Part Generation **********************/
    /*****************************************************************/
    long long s_usec, e_usec;
    s_usec = PAPI_get_real_usec();
    cout << endl << "Start generating..\n";
    interm->generate();
    e_usec = PAPI_get_real_usec();
    profTimer.addBasicTime(timerUtil::GEN_INTER_VALS, e_usec - s_usec);
    s_usec = PAPI_get_real_usec();
    genpm->generate();
    e_usec = PAPI_get_real_usec();
    profTimer.addBasicTime(timerUtil::GEN_POWER_MODEL, e_usec - s_usec);
    cout << "Done. Starting PM pre computation..\n";
    s_usec = PAPI_get_real_usec();
    stat->computeSummationsPM();
    e_usec = PAPI_get_real_usec();
    profTimer.addBasicTime(timerUtil::COMP_PMM_VAL_IDX, e_usec - s_usec);
    //free(data); // I don't need that data anymore.
    cout << "Done. Starting statistic test [multithreaded]" << endl;
    exec->RunAndWait ( numbatches );
    cout << "Finish the statistic test." << endl;
    /* Gets the ending time in microseconds */
    end_usec = PAPI_get_real_usec();
    profTimer.addBasicTime(timerUtil::TOTAL_TIME_IDX, end_usec - start_usec);
    cout << "This input took " << profTimer.getBasicTime(timerUtil::TOTAL_TIME_IDX)<< " microseconds to be computed." << endl;
    /*****************************************************************/
    /********************** 4th Part Profiling ***********************/
    /*****************************************************************/
    //std::map< long, std::vector<string> > timeLine;
    //((GenerateIntermediateValues::aes128round1*) interm)->getProfileEvents()->exportEventTimes(&timeLine);
    //((GeneratePowerModel::hammingweight*) genpm)->getProfileEvents()->exportEventTimes(&timeLine);
    //((StatisticTest::pearsonNormWithGPU2*) stat)->getProfileEvents()->exportEventTimes(&timeLine);
    ////cout <<"-----------------------------------BEGIN -----------------------------------" << endl;
    ////cout <<"sep=;" << endl;
    ////cout <<"Time;Event(s) Happened;" << endl;
    ////for (std::map< long, std::vector<string> >::iterator it=timeLine.begin(); it!=timeLine.end(); ++it) {
    ////    cout << it->first << ";";
    ////    vector<string> v = it->second;
    ////    for(int i = 0; i < v.size(); i++) {
    ////        cout << v[i] << ";";
    ////    }
    ////    cout << endl;
    ////}
    ////cout <<"------------------------------------END ------------------------------------" << endl;
    //Statistic::OpenCL::profileOpenCLCommand* command = ((GenerateIntermediateValues::aes128round1*) interm)->getProfileEvents();
    //long writeKnownDataTime = command->getExecutionTime(constants::WRITE_KNOWNDATA + convertInt(0));
    //long genInterValTime = 0;
    //for(int j = 0; j < KEY_SIZE_BYTE; j++) {
    //    genInterValTime += command->getExecutionTime(constants::GENERATE_INTER_VAL + convertInt(0) + constants::BYTE + convertInt(j));
    //}
    //profTimer.addBasicTime(timerUtil::GEN_INTER_VALS, (writeKnownDataTime + genInterValTime) / 1000);
    //command = ((GeneratePowerModel::hammingweight*) genpm)->getProfileEvents();
    //long genPowerModelTime = 0;
    //for(int j = 0; j < KEY_SIZE_BYTE; j++) {
    //    genPowerModelTime += command->getExecutionTime(constants::GENERATE_POWER_MATRIX + convertInt(0) + constants::BYTE + convertInt(j));
    //}
    //profTimer.addBasicTime(timerUtil::GEN_POWER_MODEL, genPowerModelTime / 1000);    
    //command = ((StatisticTest::pearsonNormWithGPU2*) stat)->getProfileEvents();
    //long computePMMTime = 0;
    //for(int j = 0; j < KEY_SIZE_BYTE; j++) {
    //    computePMMTime += command->getExecutionTime(constants::COMPUTE_PMM_SUMS + convertInt(0) + constants::BYTE + convertInt(j));
    //}
    //profTimer.addBasicTime(timerUtil::COMP_PMM_VAL_IDX, computePMMTime / 1000);
    //long computeTraceTime = 0;
    //long genStatMatrixTime = 0;
    //long genOutputTime = 0;
    //int batchNum = (input->getSamplesPerTrace() / BATCH_SIZE) + ((input->getSamplesPerTrace() % BATCH_SIZE) ? 1 : 0); 
    //for(int j = 0; j < batchNum; j++) {
    //    computeTraceTime += command->getExecutionTime(constants::WRITE_TRACES + convertInt(j));
    //    computeTraceTime += command->getExecutionTime(constants::COMPUTE_TRACE_SUMS + convertInt(j));
    //    for(int k = 0; k < KEY_SIZE_BYTE;k++) {
    //        genStatMatrixTime += command->getExecutionTime(constants::GENERATE_STAT_MATRIX + convertInt(j) + constants::BYTE + convertInt(k));
    //        genOutputTime += command->getExecutionTime(constants::REDUCE_STAT_MATRIX + convertInt(j) + constants::BYTE + convertInt(k));
    //        genOutputTime += command->getExecutionTime(constants::READ_RED_OUTP + convertInt(j) + constants::BYTE + convertInt(k));
    //        genOutputTime += command->getExecutionTime(constants::READ_RED_OUTP_IDXS + convertInt(j) + constants::BYTE + convertInt(k));
    //    }
    //}
    //profTimer.addBasicTime(timerUtil::COMP_TRACES_IDX, computeTraceTime / 1000);
    //profTimer.addBasicTime(timerUtil::GEN_STAT_MATRIX_IDX, genStatMatrixTime / 1000);
    //profTimer.addBasicTime(timerUtil::GEN_OUTPUT_IDX, genOutputTime / 1000);

    /*****************************************************************/
    /**********************    5th Finish      ***********************/
    /*****************************************************************/ 
    outp->end();
    //profTimer.ShowBasicTimers();
    //profTimer.ShowAllTimers();
    return 0;
}

void DPA::ShowCompileTimeOptions()
{
    cout << "DPAcalc was compiled with " << endl;
    cout << "Number of bit of the key to guess : " << KEY_HYP_BIT << endl;
    cout << "Size of known data : " << DATA_SIZE_BIT << " bit " << endl;
    cout << "Size of key : " << KEY_SIZE_BIT << " bit " << endl;
    cout << "Batch size : " << BATCH_SIZE << endl;
    cout << "Number of threads per device : " << NUM_THREADS_PER_DEVICE << endl;
    cout << endl;
    cout << "Name of the class that reads input file: " << INPUTCLASS_STR << endl;
    cout << "Name of the class that generates intermediate values: " << GENINTERMCLASS_STR << endl;
    cout << "Name of the class that generates power model: " << GENPOWERMODELCLASS_STR << endl;
    cout << "Name of the class that calculates statistic data: " << STATISTICCLASS_STR << endl;
    cout << "Name of the class that manages parallelization: " << EXECCLASS_STR << endl;
    cout << "Name of the class that writes output: " << OUTPUTCLASS_STR << endl;
    cout << endl;
}

int DPA::nextDevice()
{
    deviceNumMutex->lock();
    int result = deviceNum;
    deviceNum = (deviceNum + 1) % totalDevs; 
    deviceNumMutex->unlock();
    return result;
}

int main ( int argc, char** argv )
{
    try{
        DPA* me = DPA::instance();
        return me->main ( argc, argv );
    }
    catch( cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what();
    }
    return -1;
}
