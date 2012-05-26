/*---------------------------------------------------------- -*- Mode: C++ -*- +
| Module:	CIFuzzySet.h
+------------------------------------------------------------------------------+
| Description: Linear representation of Fuzzy Sets.
|	       A fuzzy set is represented by four values as we can see in the
|	       next figure.
|
|
|   N(u)
|    |
|  1 +            .+---+.                    a - wa <= a <= b <= b + wb
|    |           /       \
|  0 +----------+--+---+--+----------> u     0 <= N(u) <= 1 for all u
|               ^  a   b  ^
|               |         |                  wa >= 0, wb >= 0
|             a - wa      b + wb
|
| Notes: This class was completely rebuild from the original one 
|	 developed by Filipe Barreira and Paulo Vilar
|
| Author:      Manuel Joao Fonseca
|	       e-mail: mjf@inesc-id.pt
|
| Date:        April 98, July 99
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
#ifndef _FUZZY_H
#define _FUZZY_H

class CIFuzzySet {
  
  public:
// Constructors and destructors
    CIFuzzySet (double a, double b) 			{ init (a, b, 0, 0); };
    CIFuzzySet (double a, double b, double wa, double wb)	{ init (a, b, wa, wb); };

    double degOfMember (double value);
    double distance (double value);
    bool intersects (CIFuzzySet *b);
    bool isInSet (double value);

  protected:

// Variables
    double _a,   // smallest u st N(u) = 1 
	   _b,   // biggest  u st N(u) = 1 
	   _wa,  // width of left slope (wa) st  N(a - wa) = 0
	   _wb;  // width of right slope (wb) st N(b + wb) = 0

// Methods
    bool checkFuzzySet (double a, double b, double wa, double wb);
    void init (double a, double b, double wa, double wb);

};
  
#endif //_FUZZY_H
