
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