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

#include <assert.h>
#include <algorithm>
#include <bitset>
#include "config.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <unistd.h>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <Eigen/Dense>
#include <tclap/CmdLine.h>
#include <thread>
#include <map>
#include <cmath>
//enable exceptions for openCL
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
/*project files*/
#include "utils.hpp"
#include "utilsOpenCL.hpp"
#include "timerUtil.hpp"
#include "statisticaltest/openCL/openCLPlatform.hpp"
#include "statisticaltest/openCL/profileOpenCLCommand.hpp"
#include "profileEventsNames.h"
//#include <google/profiler.h>
#include "gen_intermediate/aes.h"
#include <papi.h>
//#include "cuda_runtime_api.h"
//#include "cuda.h"
//#include "cublas_v2.h"
//#include "viennacl/matrix.hpp"
//#include "viennacl/linalg/prod.hpp"
//#include "viennacl/ocl/context.hpp"
//#include "viennacl/ocl/backend.hpp"
//#include "mkl.h"



typedef std::bitset<DATA_SIZE_BIT> DataValueType;

#define DATA_SIZE_BYTE (DATA_SIZE_BIT >> 3)
#define KEY_SIZE_BYTE (KEY_SIZE_BIT >> 3)
#define KEY_NUM (1 <<KEY_HYP_BIT)

typedef std::bitset<KEY_SIZE_BIT> KeyDataType;
typedef unsigned long KeyIndexType;
typedef unsigned long long IntermediateValueType;
typedef float StatisticValueType;

/* Eigen types
typedef Eigen::Matrix<TraceValueType, Eigen::Dynamic, BATCH_SIZE> TracesMatrix;
typedef std::vector< DataValueType > DataMatrix;
typedef Eigen::Matrix<IntermediateValueType, Eigen::Dynamic, Eigen::Dynamic> IntermediateValueMatrix;
typedef Eigen::Matrix<TraceValueType, Eigen::Dynamic, Eigen::Dynamic> PowerModelMatrix;
typedef Eigen::Matrix<StatisticValueType, Eigen::Dynamic, Eigen::Dynamic> StatisticIndexMatrix;
*/

typedef TraceValueType* TracesMatrix;
typedef DataValueType* DataMatrix;
typedef IntermediateValueType* IntermediateValueMatrix;
typedef TraceValueType* PowerModelMatrix;
typedef StatisticValueType* StatisticIndexMatrix;

#define COLUMN(total, rowsize) ( total % rowsize )
#define ROW(total, rowsize) ( total / rowsize )
#define CHECK_SUCCESS(err, str) if (err != CL_SUCCESS) { cout << str << err << endl; exit(1); }

#define CL_WRITE_TRUE CL_TRUE
#define CL_WRITE_FALSE CL_FALSE
//#define CONFIG_NO_TRICKS
//#define DECOMPOSITE_EQUATION
//#define WITH_CUBLAS
//#define WITH_MKL
//#define WITH_VIENNACL


#define OCL_KDATA_BUFFER_ID "kdata_buf"
#define OCL_PM_BUFFER_ID "pm_buf"
#define OCL_TRACE_BUFFER_ID "trace_buf"
#define OCL_STAT_BUFFER_ID "stat_buf"
#define OCL_OUTPUT_BUFFER_ID "outp_buf"
#define OCL_OUTPUT2_BUFFER_ID "outp2_buf"
