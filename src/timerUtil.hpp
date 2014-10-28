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

#pragma once
#include "dpacalc.h"
using namespace std;

class timerUtil
{
    public:
        static const int INIT_IDX = 0;
        static const int KNOWNDATA_IDX = 1;
        static const int GEN_INTER_VALS = 2;
        static const int GEN_POWER_MODEL = 3;
        static const int READ_SAMPLES_IDX = 4;
        static const int COMP_PMM_VAL_IDX = 5;
        static const int COMP_TRACES_IDX = 6;
        static const int GEN_STAT_MATRIX_IDX = 7;
        static const int GEN_OUTPUT_IDX = 8;
        static const int TOTAL_TIME_IDX = 9;

        timerUtil() {
            basic_timers.push_back(0);
            names.push_back("Init");
            basic_timers.push_back(0);
            names.push_back("KnwonData");
            basic_timers.push_back(0);
            names.push_back("GenInterVal");
            basic_timers.push_back(0);
            names.push_back("GenPowerModel");
            basic_timers.push_back(0);
            names.push_back("ReadSamples");
            basic_timers.push_back(0);
            names.push_back("CompPMMVal");
            basic_timers.push_back(0);
            names.push_back("CompTraceVal");
            basic_timers.push_back(0);
            names.push_back("GenStatMatrix");
            basic_timers.push_back(0);
            names.push_back("GenOutputVal");
            basic_timers.push_back(0);
            names.push_back("TotalTimeExecution");
        }
        void addBasicTime(int idx, long long value) {
            basic_timers[idx] += value;
        };
        void addNewTime(string description, long long value) {
            time_values[description].push_back(value);
        };
        std::map<string, std::vector<long long> > getSpentTime() { return time_values; };
        void ShowAllTimers() {
            // showing contents:
            for (std::map<string, std::vector<long long> >::iterator it=time_values.begin(); it!=time_values.end(); ++it) {
                std::cout << it->first;
                vector<long long> v = it->second;
                for(int i = 0; i < v.size(); i++) {
                    std::cout << v[i] << std::endl;
                }
            }
        };
        void ShowBasicTimers(){
            for(int i = 0; i < basic_timers.size(); i++) {
                std::cout << names[i] << "\t-\t" << basic_timers[i] << std::endl;
            }
        };
        long long getBasicTime(int idx) {
            return basic_timers[idx];
        }
    protected:
        std::map<string, std::vector<long long> > time_values;
        std::vector<long long> basic_timers;
        std::vector<std::string> names;
};

