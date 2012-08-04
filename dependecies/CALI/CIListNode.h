/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIListNode.h
+-----------------------------------------------------------------------------+
| Description: Class that abstracts a node of a double linked list. The node
|	      is build of an item where we save the information (data) and two
|	      pointers, one to the next node in the list and the other to the
|	      previous node.
|
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|	  http://immi.inesc-id.pt/~mjf
|
| Date: April 98
| Changed: April 99
+----------------------------------------------------------------------------
|
| Copyright (C) 1998, 1999, 2000, 2008 Manuel João da Fonseca
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

#ifndef _LIST_NODE_H
#define _LIST_NODE_H

template <class Item>
class CIListNode {

  public:

// Constructor and destructor
    CIListNode (Item item, double ordVal = 0);
    ~CIListNode () {}

// Node retrieval
    inline Item& getItem () ;
    inline CIListNode<Item>* next ();
    inline CIListNode<Item>* prev ();
    inline int getIdx ();
    inline double getOrdVal ();

// Node settings
    inline void setItem (Item item);
    inline void setNext(CIListNode<Item>* pos);
    inline void setPrev (CIListNode<Item>* pos);
    inline void setIdx (int idx);
    inline void setOrdVal (double ordVal);

  protected:

    Item	      _item;
    CIListNode<Item>* _next;
    CIListNode<Item>* _prev;
    int		      _idx;	 // index value of the last item retrieved
    double	      _ordValue; // value used to order the list
};


/*-----| Inline methods |-----*/

/*----------------------------------------------------------------------------+
| Description: Returns the Item stored into the list node.
+----------------------------------------------------------------------------*/
template <class Item>
inline Item& CIListNode<Item>::getItem () { return _item; }

/*----------------------------------------------------------------------------+
| Description: Returns a pointer to the next node of the list.
+----------------------------------------------------------------------------*/
template <class Item>
inline CIListNode<Item>* CIListNode<Item>::next () { return _next; }

/*----------------------------------------------------------------------------+
| Description: Returns a pointer to the previous node of the list.
+----------------------------------------------------------------------------*/
template <class Item>
inline CIListNode<Item>* CIListNode<Item>::prev () { return _prev; }

/*----------------------------------------------------------------------------+
| Description: Returns the index value.
+----------------------------------------------------------------------------*/
template <class Item>
inline int CIListNode<Item>::getIdx () { return _idx; }

/*----------------------------------------------------------------------------+
| Description: Returns the order value.
+----------------------------------------------------------------------------*/
template <class Item>
inline double CIListNode<Item>::getOrdVal () { return _ordValue; }

/*----------------------------------------------------------------------------+
| Description: Stores or updates the data into the list node.
+----------------------------------------------------------------------------*/
template <class Item>
inline void CIListNode<Item>::setItem (Item item) { _item = item; }

/*----------------------------------------------------------------------------+
| Description: Sets the value of the pointer to the next node.
+----------------------------------------------------------------------------*/
template <class Item>
inline void CIListNode<Item>::setNext (CIListNode<Item>* pos) { _next = pos; }

/*----------------------------------------------------------------------------+
| Description: Sets the value of the pointer to the previous node.
+----------------------------------------------------------------------------*/
template <class Item>
inline void CIListNode<Item>::setPrev (CIListNode<Item>* pos) { _prev = pos; }

/*----------------------------------------------------------------------------+
| Description: Sets the value of the index value.
+----------------------------------------------------------------------------*/
template <class Item>
inline void CIListNode<Item>::setIdx (int idx) { _idx = idx; }

/*----------------------------------------------------------------------------+
| Description: Sets the value of the order value.
+----------------------------------------------------------------------------*/
template <class Item>
inline void CIListNode<Item>::setOrdVal (double ordVal) { _ordValue = ordVal; }

/*-----| "Normal" methods  (Not inline) |-----*/
/*----------------------------------------------------------------------------+
| Description: Constructor of the class. Initializes the instance variables 
| Input: Item - The item we want to store into the list node.
+----------------------------------------------------------------------------*/
template <class Item>
inline CIListNode<Item>::CIListNode(Item item, double ordVal)
{
    _item = item;
    _next = _prev = NULL;
    _idx = -1;	// default value, node not numbered.
    _ordValue = ordVal;
}


#endif // _LIST_NODE_H
