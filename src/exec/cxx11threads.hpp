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
#include "base.hpp"

namespace ExecMethod
{
    class cxx11threads: public base
    {
        public:
            cxx11threads ( TCLAP::CmdLine* cmd ) : base ( cmd ), procArg ( "p", "processors", "Number of processors to use (0=autodetect)", false, 0, "int" ) {
                cmd->add ( procArg );
            };
            virtual void RunAndWait ( unsigned long numberoftimes );
            static void threadfunction(int i);
        protected:
            TCLAP::ValueArg<int> procArg;
    };
}
