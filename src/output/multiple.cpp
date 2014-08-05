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
#include "multiple.hpp"
#include "coarseresult.hpp"
#include "gnuplot.hpp"
Output::multiple::multiple ( TCLAP::CmdLine& cmd, shared_ptr< KeyGenerators::base > _keygen ) : base ( cmd, _keygen ), outputs()
{
	//Order here is important because coarseresult voluntarily modifies the matrix. (takes the absolute value of cells)
	outputs.push_back ( shared_ptr<Output::base> ( new Output::coarseresult ( cmd, keygen ) ) );
	//while gnuplot takes only a reference when puts the matrix in the queue.
	outputs.push_back ( shared_ptr<Output::base> ( new Output::gnuplot ( cmd, keygen ) ) );
	//Having them inverted, could cause some intervals of time with proper sign, and others in absolute value.
	//So we get the plot of absolute values.
	//Having the plot with proper sign costs a copy of the matrix. It is not worth it.
}
void Output::multiple::end()
{
	Output::base::end();
	for ( vector<shared_ptr<Output::base> >::iterator it = outputs.begin(); it != outputs.end(); ++it ) {
		( *it )->end();
	}
}
void Output::multiple::init()
{
	Output::base::init();
	for ( vector<shared_ptr<Output::base> >::iterator it = outputs.begin(); it != outputs.end(); ++it ) {
		( *it )->init();
	}
}
void Output::multiple::WriteBatch ( long long unsigned int id, shared_ptr< StatisticIndexMatrix >& s )
{
	for ( vector<shared_ptr<Output::base> >::iterator it = outputs.begin(); it != outputs.end(); ++it ) {
		( *it )->WriteBatch ( id, s );
	}
}
