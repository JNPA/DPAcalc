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
#include "dpacalc.h"
#include <iostream>
#include <queue>
#include <utility>
#include "base.hpp"
using namespace Eigen;
using namespace std;
namespace SamplesInput
{
#pragma pack(push)
#pragma pack(1)
	struct fileheaders {
		uint32_t numtraces;
		uint32_t numsamples_per_trace;
		char datatype;
		uint8_t knowndatalength;
	};
#pragma pack(pop)
	struct queueelement {
		unsigned long long id;
		long long size;
		shared_ptr<TracesMatrix> traces;
		queueelement() {
			id = 0;
			size = -1;
		}
	};
	class bin1 : public base
	{
		public:
			virtual unsigned long long read ( unsigned long long* id, shared_ptr<TracesMatrix>* traces );
			bin1 ( TCLAP::CmdLine& cmd ) :
				base ( cmd ),
				nameArg ( "f", "filename", "Input file name", true, "", "path" ),
				mlockArg ( "m", "mlock", "mlock entire input file in ram? Use only if you know what you're doing.", false ),
				queuemutex(), readytraces() {
				cmd.add ( nameArg );
				cmd.add ( mlockArg );
			};
			virtual void init();
			~bin1();
			shared_ptr<DataMatrix> readData();
		protected:
			TCLAP::ValueArg<std::string> nameArg;
			TCLAP::SwitchArg mlockArg;
			std::mutex input_mutex;
			int inputfd;
			template <class T> void readSamples ( shared_ptr<TracesMatrix>& traces, unsigned long curtrace, unsigned long startingsample, unsigned long numsamples );
			char sampletype;
			int samplesize;
			void* fileoffset;
			shared_ptr<DataMatrix> data;
			void populateQueue();
			mutex queuemutex;
			long long RealFileSize;
			queue<queueelement> readytraces;
			unsigned long long getSampleOffset ( unsigned long long trace, unsigned long long samplenum ) {
				//trace and samplenum are zero-based
				return sizeof ( struct fileheaders ) + trace * ( samplesize * SamplesPerTrace + DATA_SIZE_BYTE ) + samplesize * samplenum;
			}
			unsigned long long getDataOffset ( unsigned long long trace ) {
				//trace and samplenum are zero-based
				return sizeof ( struct fileheaders ) + trace * ( samplesize * SamplesPerTrace + DATA_SIZE_BYTE ) + samplesize * SamplesPerTrace;
			}
	};


}
