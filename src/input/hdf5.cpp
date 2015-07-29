/*
Copyright 2015 Silvan Streit after João Amaral bin1.cpp

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

#include "hdf5.hpp"


long long unsigned int SamplesInput::hdf5::read ( long long unsigned int* id, TracesMatrix* traces )
{   //checks the queue for new traces and waits until there are new ones. Calls pupulateQueue if necessary. 
    //Will write the traces to traces and the id to id and returns the number of samples

    queueelement qe;
    //check for received traces
    while ( qe.size == -1 ) {
        queuemutex.lock();
        // get new traces if there are any
        if ( !readytraces.empty() ) {
            qe = readytraces.front();
            readytraces.pop();
            queuemutex.unlock();
        // if there are no new traces call populateQueue to get new traces
        } else {
            queuemutex.unlock();
            populateQueue();
        }
    }
    *id = qe.id;
    *traces = qe.traces;
    std::cout << "My first sample is " << qe.id * BATCH_SIZE << std::endl;
    return qe.size;
}

void SamplesInput::hdf5::init()
{   //opens datafile (hdf5) and outputs basic information about it. Calls readData()

    SamplesInput::base::init();

    input_mutex.lock();

    //open HDF5 input file, given by command line option (-f)
    inputfd.openFile( nameArg.getValue().c_str(), H5F_ACC_RDONLY );

    //Select traces dataset
    traces_dataset = inputfd.openDataSet( "traces" );

    // Get the class of the datatype that is used by the dataset.
    H5T_class_t type_class = traces_dataset.getTypeClass();

    // Get dataspace of the dataset.
    traces_dataspace = traces_dataset.getSpace();


    // Get the dimension size of each dimension in the dataspace.
    hsize_t dims_out[2];
    traces_dataspace.getSimpleExtentDims( dims_out, NULL);

    input_mutex.unlock();

    NumTraces = (unsigned long long)(dims_out[0]);
    SamplesPerTrace = (unsigned long long)(dims_out[1]);

    cout << "---File's Metadata---" << endl;
    cout << "Number of samples per trace: " << SamplesPerTrace << endl;
    cout << "Number of traces: " << NumTraces << endl;
    cout << "Data type: ";
    switch ( type_class ) {
        case H5T_INTEGER:
            cout << "integer";
            break;
        case H5T_FLOAT:
            cout << "float";
            break;
        case H5T_STRING:
            cout << "string";
            break;
        case H5T_BITFIELD:
            cout << "bitfield";
            break;
        default: //H5T_NO_CLASS
            cout << "unknown type";
    }
    cout << endl;
    cout << "---------------------" << endl;

    // read the data-table (plain- or ciphertext)
    readData();
}

void SamplesInput::hdf5::populateQueue()
{   //Populates the queue with new traces. Reads them from the hdf5 input file

    long long start_usec, end_usec;
    start_usec = PAPI_get_real_usec();
    unsigned long long mysample;
    queueelement qe = queueelement();
    if ( readytraces.size() >= INPUT_QUEUE_SIZE ) {
        return;
    }
    input_mutex.lock();
    if ( CurrentSample >= SamplesPerTrace ) {
        input_mutex.unlock();
        return ;
    }
    unsigned long long num = min<unsigned long long> ( BATCH_SIZE, SamplesPerTrace - CurrentSample );
    mysample = CurrentSample;
    CurrentSample += num;
    ++CurrentId;
    qe.size = num;
    qe.id = CurrentId;
    TracesMatrix tmp = (TracesMatrix) malloc( sizeof(TraceValueType) * NumTraces * num);
    qe.traces = tmp;
    //cout << "Traces:I'm going to allocate a " << NumTraces << " * " << num << " * " << sizeof(TraceValueType) << " = " << (NumTraces*num*sizeof(TraceValueType)/1024) << " kb matrix."<<endl;

    // Define hyperslab in the dataset, only take current part of samples
    hsize_t      offset[2];	// hyperslab offset in the file
    hsize_t      count[2];	// size of the hyperslab in the file
    offset[0] = 0;
    offset[1] = mysample;
    count[0]  = NumTraces;
    count[1]  = num;
    traces_dataspace.selectHyperslab( H5S_SELECT_SET, count, offset );


    // Define the memory dataspace. -> linear memory
    hsize_t     dimsm_traces[1];             // memory space dimensions
    dimsm_traces[0] = NumTraces*num;
    DataSpace traces_memspace( 1, dimsm_traces );

    // read the data from the file
    traces_dataset.read( qe.traces, TRACETYPEHDF5, traces_memspace, traces_dataspace );

    queuemutex.lock();
    readytraces.push ( qe );
    queuemutex.unlock();
    input_mutex.unlock();
    end_usec = PAPI_get_real_usec();
    profTimer->addBasicTime(timerUtil::READ_SAMPLES_IDX, end_usec - start_usec);
    return;
}



DataMatrix SamplesInput::hdf5::readData()
{   //reads the data-table (plaintext or ciphertext), used to compute intermediate result

    char* buffer;
    //cout << "Reading known data.." << std::endl;
    if ( data != NULL ) {
        //cout << "Already read known data." << std::endl;
        return data;
    }
    long long start_usec, end_usec;
    // Gets the starting time in microseconds
    start_usec = PAPI_get_real_usec();
    //cout << "Input lock" << std::endl;
    input_mutex.lock();
    DataMatrix dm = (DataValueType*) malloc( sizeof(DataValueType) * NumTraces );
    data = dm;

    //Open dataset with data from which the intermediate result will be calculated. Given as a command line option (-d).
    DataSet dataset = inputfd.openDataSet( dataArg.getValue().c_str() ); 

    // Get dataspace of the dataset.
    DataSpace dataspace = dataset.getSpace();

    // Define hyperslab in the dataset
    hsize_t      offset[2];	// hyperslab offset in the file
    hsize_t      count[2];	// size of the hyperslab in the file
    offset[0] = 0;
    offset[1] = 0;
    count[0]  = NumTraces;
    count[1]  = DATA_SIZE_BYTE;
    dataspace.selectHyperslab( H5S_SELECT_SET, count, offset );

    // Define the memory dataspace.
    DataSpace memspace( 2, count );

    // read the data
    dataset.read( data, PredType::NATIVE_UINT8, memspace, dataspace ); //automatically converts to bitset ;-)

    //cout << "Input unlock" << std::endl;
    input_mutex.unlock();
    cout << "Read data" << std::endl;
    // Gets the ending time in microseconds 
    end_usec = PAPI_get_real_usec();
    profTimer->addBasicTime(timerUtil::KNOWNDATA_IDX, end_usec - start_usec);
    return data;
}

SamplesInput::hdf5::~hdf5() // close input file
{
    inputfd.close();
}

