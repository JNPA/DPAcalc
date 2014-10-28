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

#include <string>

#pragma once
namespace constants
{
    /************************************/
    /**** OpenCL Profile Event Names ****/
    /************************************/
    const std::string WRITE_KNOWNDATA = "WrKDATA";
    const std::string WRITE_TRACES = "WrTrace";
    const std::string READ_RED_OUTP = "RdReduOutp";
    const std::string READ_RED_OUTP_IDXS = "RdReduOutpIdx";
    const std::string GENERATE_INTER_VAL = "GenInterVal";
    const std::string GENERATE_POWER_MATRIX = "GenPowerMatrix";
    const std::string GENERATE_STAT_MATRIX = "GenStatMatrix";
    const std::string COMPUTE_PMM_SUMS = "CompPMMSum";
    const std::string COMPUTE_TRACE_SUMS = "CompTraceSum";
    const std::string BYTE = "ByteNum";
    const std::string REDUCE_STAT_MATRIX = "RedStatMatrix";

}