/*
Copyright 2015 Silvan Streit after João Amaral bin1.hpp

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

#pragma once
#include "dpacalc.h"
#include "base.hpp"

#include "H5Cpp.h"

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#ifndef H5_NO_STD
    using std::cout;
    using std::endl;
#endif  // H5_NO_STD
#endif  // H5_NO_NAMESPACE

#if defined(CONFIG_TRACETYPE_FLOAT)
    #define TRACETYPEHDF5 PredType::NATIVE_FLOAT
#elif defined(CONFIG_TRACETYPE_DOUBLE)
    #define TRACETYPEHDF5 PredType::NATIVE_DOUBLE
#endif





namespace SamplesInput
{
    struct queueelement {
        unsigned long long id;
        long long size;
        TracesMatrix traces;
        queueelement() {
            id = 0;
            size = -1;
        }
    };
    class hdf5 : public base
    {
        public:
            virtual unsigned long long read ( unsigned long long* id, TracesMatrix* traces );
            hdf5 ( TCLAP::CmdLine* cmd, timerUtil* _profTimer ) : // constructor
                base ( cmd, _profTimer ), // pass on to the base class
                nameArg ( "f", "filename", "Input file name", true, "", "path" ), // set input file command line argument
                dataArg ( "d", "data", "Used data-table in hdf5-file for intermediate result, e.g. plaintext or ciphertext. [plaintext]", false, "plaintext", "dataset-name" ), // set data-table command line argument
                queuemutex(), //init mutex for queue
                readytraces() //init queue with new traces
                {
                    cmd->add ( nameArg ); // init command line argument for input file
                    cmd->add ( dataArg ); // init command line argument for data-table
                };
            virtual void init(); // init function, called once at the beginning. Opens up the hdf5 file and calls readData.
            ~hdf5(); // destructor
            DataMatrix readData(); // read the data-matrix and return it for intermediate result.
        protected:
            TCLAP::ValueArg<std::string> nameArg; // input file name read from command line
            TCLAP::ValueArg<std::string> dataArg; // data-table for intermediate result read from command line
            std::mutex input_mutex; // mutex to lock input file to be thread save
            H5File inputfd; // hdf5 input file handler
            DataSet traces_dataset; // traces dataset
            DataSpace traces_dataspace; // traces dataspace
            DataMatrix data; // data matrix for intermediate result. Contains plain- or ciphertext.
            void populateQueue(); // method to populate the queue with new traces
            std::mutex queuemutex; // mutext to lock queue to be thread save
            std::queue<queueelement> readytraces; // queue with new traces
    };


}
