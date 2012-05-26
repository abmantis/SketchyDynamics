/*-------------------------------------------------- -*- Mode: C++ -*- +
| Module:	CIFuzzySet.C
+----------------------------------------------------------------------+
| Description: Implementation of class CIFuzzySet
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98
+-----------------------------------------------------------------------------+
|
| Copyright (C) 1998, 1999, 2000 Manuel João da Fonseca
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or any later version.
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
| 
| You should have received a copy of the GNU General Public License
| along with this program; if not, write to the Free Software
| Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
| 
+----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <iostream>
#include "CALI/CIFuzzySet.h"

/*----------------------------------------------------------------------+
| Description: Evaluate the degree of membership for a specific value
| Input:  value - point of evaluation
| Output: degree of membership
+----------------------------------------------------------------------*/
double CIFuzzySet::degOfMember (double value)
{
    if ((value < (_a - _wa)) || (value > (_b + _wb)) )
	return 0;   
    else if (value >= _a && value <= _b)
	return 1;
    else if (value > _b && (value <= _b + _wb))
	return 1.0 - (value - _b) / _wb;
    else if (value < _a && (value >= _a - _wa))
	return 1.0 + (value - _a) / _wa;
    else {
	// cerr << "Invalid fuzzy set" << endl;
	abort();
	return 0.0;
    }
}

/*----------------------------------------------------------------------+
| Description: Evaluate the distance to the fuzzy set
| Input:  value - point of evaluation
| Output: distance
+----------------------------------------------------------------------*/
double CIFuzzySet::distance (double value)
{
    double low = _a - _wa; 
    double high = _b + _wb;

    if ( (value >= low) && (value <= high) ) return 0;
    else if (value < low) return (low - value);
    else return (value - high);
}

/*----------------------------------------------------------------------+
| Description: Verify if the current fuzzy set is intersected by the
|	      fuzzy set fs.
| Input:  fs - a fuzzy set
| Output: true if they intersect each other, false otherwise
+----------------------------------------------------------------------*/
bool CIFuzzySet::intersects (CIFuzzySet *fs)
{
    if (fs == NULL) return false;
    if ((fs->_b + fs->_wb) < (_a - _wa) || ((fs->_a - fs->_wa) > (_b + _wb)))
	return false;
    else
	return true;
}

/*----------------------------------------------------------------------+
| Description: Verify if the value belongs to the fuzzy set
| Input:  value
| Output: true if it belongs, false otherwise
+----------------------------------------------------------------------*/
bool CIFuzzySet::isInSet (double value)
{
    if (value < (_a - _wa) || (value > (_b + _wb)))
        return false;
    else
        return true;
}

/*---------------| Private members of the class |-----------------------*/

/*----------------------------------------------------------------------+
| Description: Check fuzzy set limits
| Input:  CIFuzzy set values
| Output: true if ok, false otherwise
+----------------------------------------------------------------------*/
bool CIFuzzySet::checkFuzzySet(double a, double b, double wa, double wb)
{
  return (a <= b) && (wa >= 0) && (wb >= 0);
}

/*----------------------------------------------------------------------+
| Description: Create a fuzzy set from 4 values
| Input:  a, b, wa, wb - Values that define the limits of the fuzzy set
+----------------------------------------------------------------------*/
void CIFuzzySet::init (double a, double b, double wa, double wb)
{
    if (checkFuzzySet(a, b, wa, wb)) {
	_a = a; _b = b;
	_wa = wa; _wb = wb;
    }
    else {
	// cerr << "Attempting to create invalid fuzzy set" << a << b << wa << wb << endl;
	abort();
    }
}
