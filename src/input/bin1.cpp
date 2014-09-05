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
#include "bin1.hpp"


long long unsigned int SamplesInput::bin1::read ( long long unsigned int* id, TracesMatrix* traces )
{
    queueelement qe;
    while ( qe.size == -1 ) {
        queuemutex.lock();
        if ( !readytraces.empty() ) {
            qe = readytraces.front();
            readytraces.pop();
            queuemutex.unlock();
        } else {
            queuemutex.unlock();
            populateQueue();
        }
    }
    *id = qe.id;
    *traces = qe.traces;
    return qe.size;
}
void SamplesInput::bin1::init()
{
    long long start_usec, end_usec;
    start_usec = PAPI_get_real_usec();
    struct fileheaders header;
    long long TotalFileSize;
    SamplesInput::base::init();
    inputfd = open ( nameArg.getValue().c_str(), O_RDONLY );
    if ( inputfd == -1 ) {
        cerr << "Cannot open " << nameArg.getValue() << endl;
        exit ( 1 );
    }
    if ( ::read ( inputfd, ( void* ) &header, sizeof ( header ) ) != sizeof ( header ) ) {
        cerr << "Invalid header size" << endl;
        exit ( 1 );
    }
    SamplesPerTrace = header.numsamples_per_trace;
    NumTraces = header.numtraces;
    cout << "---File's Metadata---" << endl;
    cout << "Number of samples per trace: \t" << header.numsamples_per_trace << endl;
    cout << "Number of traces: \t" << header.numtraces << endl;
    cout << "Known Data Lenght: \t" << (int) header.knowndatalength << endl;
    cout << "Data type: \t" << header.datatype << endl;
    cout << "---------------------" << endl;
    switch ( header.datatype ) {
        case 'b':
            samplesize = 1;
            sampletype = 'b';
            break;
        case 'c':
            samplesize = 2;
            sampletype = 'c';
            break;
        case 'f':
            samplesize = 4;
            sampletype = 'f';
            break;
        case 'd':
            samplesize = 8;
            sampletype = 'd';
            break;
        default:
            cerr << "Sample data type should be b,c,f, or d. " << endl;
            exit ( 1 );
    }
    TotalFileSize = ( long long ) sizeof ( header ) + ( long long ) header.numtraces * ( ( long long ) header.knowndatalength + samplesize * ( long long ) header.numsamples_per_trace );
    RealFileSize = lseek ( inputfd, 0, SEEK_END );
//    cout << "File size is " << RealFileSize << endl << " Header size: " << sizeof ( header ) << endl << header.numtraces << " traces, single trace size: " << ( int ) header.knowndatalength << " (known data) + " << header.numsamples_per_trace << "*" << samplesize << " bytes." << endl;
    if ( RealFileSize != TotalFileSize ) {
        cerr << "File size should be " << TotalFileSize << " but it is " << RealFileSize << endl << " Header size: " << sizeof ( header ) << endl << header.numtraces << " traces, single trace size: " << ( int ) header.knowndatalength << " (known data) + " << header.numsamples_per_trace << "*" << samplesize << " bytes." << endl;
        exit ( 2 );
    }
    if ( header.knowndatalength != DATA_SIZE_BYTE ) {
        cerr << "The length of each known data should be " << DATA_SIZE_BYTE << " but it is " << ( int ) header.knowndatalength << ". Change data file or recompile this program with appropriate options." << endl;
        exit ( 2 );
    }
    fileoffset = mmap ( NULL, RealFileSize, PROT_READ, MAP_SHARED, inputfd, 0 );
    if ( fileoffset == MAP_FAILED ) {
        cerr << "Cannot memory map input file. Cannot continue" << endl;
        exit ( 3 );
    }
    if ( mlockArg.getValue() ) {
        cout << "mlock-ing" << endl;
        mlock ( fileoffset, RealFileSize );
        cout << "mlock-ed" << endl;
    }
    /* Gets the ending time in microseconds */
    end_usec = PAPI_get_real_usec();
    //printf("Verifying the input file took %lld microseconds.\n", end_usec - start_usec);
    readData();
}

void SamplesInput::bin1::populateQueue()
{
    long long start_usec, end_usec;
    start_usec = PAPI_get_real_usec();
    unsigned long long cur_trace;
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
    TracesMatrix tmp = (TracesMatrix) malloc( sizeof(TraceValueType) * NumTraces * BATCH_SIZE);
    qe.traces = tmp;
    //cout << "Traces:I'm going to allocate a " << NumTraces << " * " << BATCH_SIZE << " * " << sizeof(TraceValueType) << " = " << (NumTraces*BATCH_SIZE*sizeof(TraceValueType)/1024) << " kb matrix."<<endl;
    for ( cur_trace = 0; cur_trace < NumTraces; cur_trace++ ) {
        switch ( sampletype ) {
            case 'b':
                readSamples<uint8_t> ( qe.traces, cur_trace, mysample, num );
                break;
            case 'c':
                readSamples<uint16_t> ( qe.traces, cur_trace, mysample, num );
                break;
            case 'f':
                readSamples<float> ( qe.traces, cur_trace, mysample, num );
                break;
            case 'd':
                readSamples<double> ( qe.traces, cur_trace, mysample, num );
                break;
        }
    }
    std::cout << "My first sample is " << mysample << std::endl;
    queuemutex.lock();
    readytraces.push ( qe );
    queuemutex.unlock();
    input_mutex.unlock();
    end_usec = PAPI_get_real_usec();
    profTimer->addBasicTime(timerUtil::READ_SAMPLES_IDX, end_usec - start_usec);
    return;
}


template <class T>void SamplesInput::bin1::readSamples ( TracesMatrix traces, unsigned long curtrace, unsigned long startingsample, unsigned long numsamples )
{
    T* buffer;
    //File is big enough, checked right after open.
    buffer = ( T* ) ( ( char* ) fileoffset + getSampleOffset ( curtrace, startingsample ) );
    for ( unsigned long i = 0; i < numsamples; i++ ) {
        traces[ curtrace * BATCH_SIZE + i ] = buffer[i];
    }
}


DataMatrix SamplesInput::bin1::readData()
{
    char* buffer;
    cout << "Reading data.." << std::endl;
    if ( data != NULL ) {
        cout << "Already read data." << std::endl;
        return data;
    }
    long long start_usec, end_usec;
    /* Gets the starting time in microseconds */
    start_usec = PAPI_get_real_usec();
    cout << "Input lock" << std::endl;
    input_mutex.lock();
    DataMatrix dm = (DataValueType*) malloc( sizeof(DataValueType) * NumTraces );
    data = dm;
    for ( unsigned long cur_trace = 0; cur_trace < NumTraces; cur_trace++ ) {
        buffer = ( char* ) fileoffset +  getDataOffset ( cur_trace );
        BufferToBitset<DATA_SIZE_BYTE> ( buffer, data[cur_trace] );
    }
    cout << "Input unlock" << std::endl;
    input_mutex.unlock();
    cout << "Read data" << std::endl;
    /* Gets the ending time in microseconds */
    end_usec = PAPI_get_real_usec();
    profTimer->addBasicTime(timerUtil::KNOWNDATA_IDX, end_usec - start_usec);
    return data;
}

SamplesInput::bin1::~bin1()
{
    if ( mlockArg.getValue() ) {
        munlock ( fileoffset, RealFileSize );
    }
    munmap ( fileoffset, RealFileSize );
    close ( inputfd );
}

