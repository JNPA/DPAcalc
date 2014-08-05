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
#include "coarseresult.hpp"
void Output::coarseresult::end()
{
	cout << "The best key hypotesis is: " << keygen->getKeyAsString ( best ) << endl;
	cout << "and its statistic index is: " << bestval << endl;
}
void Output::coarseresult::WriteBatch ( unsigned long long id, shared_ptr<StatisticIndexMatrix>& s )
{
	StatisticIndexMatrix::Index maxRow, maxCol;
	( *s ) = s->cwiseAbs();
	StatisticValueType max = s->maxCoeff ( &maxRow, &maxCol );
	checkingMutex.lock();
	if ( max > bestval ) {
		bestval = max;
		best = maxCol;
	}
	checkingMutex.unlock();
}