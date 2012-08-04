/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIEvaluate
+-----------------------------------------------------------------------------+
| Description: This class defines all the features we can use to classify a
|              gesture.
| 
| Notes:       
|
| $Log$
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 98, April 99
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
#ifndef _CIEVALUATE_H
#define _CIEVALUATE_H

#include "CIScribble.h"
#include "myValues.h"

/* --- Abreviation meaning
Tl     - Scribble total length
Ns     - Number of strokes of the scribble
ch     - Convex Hull
lt     - Largest Triangle
lq     - Largest Quadrilateral
er     - Enclosing Rectangle
bb     - Bounding Box

P      - Perimeter
A      - Area

Pch    - Convex Hull perimeter
Pch2   - Convex Hull perimeter squared
...
*/

class CIEvaluate {
public:
    // Special ratios
    double Tl_Pch (CIScribble *sc);
    double Pch2_Ach (CIScribble *sc);
    double Pch_Ns_Tl (CIScribble *sc);
    double Hollowness(CIScribble *sc);
    double Ns(CIScribble *sc);

    // Other special ones
    double Hm_Wbb  (CIScribble *sc);
    double Vm_Hbb  (CIScribble *sc);
    double Hbb_Wbb (CIScribble *sc);
    double Her_Wer (CIScribble *sc);
    double Diag_er (CIScribble *sc);
    double pch_psc (CIScribble *sc);

    // Area ratios
    double Alt_Ach (CIScribble *sc);
    double Ach_Aer (CIScribble *sc);
    double Alt_Aer (CIScribble *sc);
    double Ach_Abb (CIScribble *sc);
    double Alt_Abb (CIScribble *sc);
    double Alq_Ach (CIScribble *sc);
    double Alq_Aer (CIScribble *sc);
    double Alt_Alq (CIScribble *sc);
    double Aer_Abb (CIScribble *sc);
    double Alq_Abb (CIScribble *sc);

    // Perimeter ratios
    double Plt_Pch (CIScribble *sc);
    double Pch_Per (CIScribble *sc);
    double Plt_Per (CIScribble *sc);
    double Pch_Pbb (CIScribble *sc);
    double Plt_Pbb (CIScribble *sc);
    double Plq_Pch (CIScribble *sc);
    double Plq_Per (CIScribble *sc);
    double Plt_Plq (CIScribble *sc);
    double Per_Pbb (CIScribble *sc);
    double Plq_Pbb (CIScribble *sc);
};
#endif
