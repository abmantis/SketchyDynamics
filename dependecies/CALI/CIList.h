/*-------------------------------------------------- -*- Mode: C++ -*- +
| Module:   CIList.h
+----------------------------------------------------------------------+
|    Description: Class that implements a double linked list, with a next
|		  and previous pointer, and a index operator [].
|    Notes:
|
|    Author:      Manuel Joao Fonseca
|		  e-mail: mjf@inesc-id.pt
|		  http://immi.inesc-id.pt/~mjf
|
|    Date:        April 98
|    Changed: 	  April 99
|    Changed:	  January 03
+----------------------------------------------------------------------------
|
| Copyright (C) 1998-2003 Manuel João da Fonseca
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

#ifndef _CILIST_H
#define _CILIST_H

#include <iostream>
//#include <stdlib>
#include "CIListNode.h"

template <class Item>
class CIList {

  public:

// Constructores and destructores
      CIList (bool acceptRepeated = true);
      ~CIList (); // The application that use the List, MUST delete all the 
      		  // Itens before delete the List.
		  // It is like this because we can not delete Itens like
		  // int, float, char, etc
		  // (confirmar!!)
		  // or we just want to delete the list and keep the objects
		  // pointed by each list node.

// Item Insertion
    CIListNode<Item> *insertHead (Item item);
    CIListNode<Item> *insertTail (Item item);
    CIListNode<Item> *insertBefore (CIListNode<Item>* pos, Item item);
    CIListNode<Item> *insertAfter (CIListNode<Item>* pos, Item item);
    CIListNode<Item> *insertInOrder (Item item, double ordValue);
    CIListNode<Item> *insertInRevOrder (Item item, double ordValue);
    inline CIListNode<Item> *push (Item item)
        { return insertTail(item); }
    void joinListAfter (CIList<Item>* list);

// Item Retrieval
    inline Item getFirstItem ();
    inline Item getLastItem ();
    inline Item getItemAt (CIListNode<Item>* pos);
    inline void setItemAt (CIListNode<Item>* pos, Item item);
    Item& operator[] (int index);
    Item pop ();    // This method extract the last item from the list
    		    // and return it

// Iteration
    inline CIListNode<Item> *getHeadPosition ();
    inline CIListNode<Item> *getTailPosition ();
    Item getNextItem (CIListNode<Item>* &pos);
    Item getPrevItem (CIListNode<Item>* &pos);

// Status
    inline int  getNumItems ();
    inline bool isEmpty ();
    inline bool isFirst (CIListNode<Item>* pos);
    inline bool isLast (CIListNode<Item>* pos);

// Searching
    CIListNode<Item>* findItem (Item item, CIListNode<Item>* pos = NULL);

// Operations
    void removeAt (CIListNode<Item>* pos);
    void removeFirstOcc (Item item);
    void removeAllNodes ();

  protected:

    CIListNode<Item>* _first;
    CIListNode<Item>* _last;
    CIListNode<Item>* _current;
    int	              _count;
    bool              _acceptRepeated;

    void deleteAll(CIListNode<Item>* pos);
};


/*----------------------------------------------------------------------------+
|			    Inline methods
+----------------------------------------------------------------------------*/

/*-----| Item retrieval methods  |-----*/
/*----------------------------------------------------------------------------+
| Description: Returns the first/last Item of the list.
+----------------------------------------------------------------------------*/
template <class Item>
inline Item CIList<Item>::getFirstItem () { return _first->getItem(); }

template <class Item>
inline Item CIList<Item>::getLastItem () { return _last->getItem(); }

/*----------------------------------------------------------------------------+
| Description: Gets/Sets the value of the Item at the specified position.
+----------------------------------------------------------------------------*/
template <class Item>
inline Item CIList<Item>::getItemAt (CIListNode<Item>* pos) 
{ return pos->getItem(); }

template <class Item>
inline void CIList<Item>::setItemAt (CIListNode<Item>* pos, Item item) 
{ pos->setItem(item); }


/*-----| Iteration methods  |-----*/
/*----------------------------------------------------------------------------+
| Description: Gets a pointer to the first/last node of the list.
+----------------------------------------------------------------------------*/
template <class Item>
inline CIListNode<Item>* CIList<Item>::getHeadPosition () { return _first; }

template <class Item>
inline CIListNode<Item>* CIList<Item>::getTailPosition () { return _last; }


/*-----| Status methods  |-----*/
/*----------------------------------------------------------------------------+
| Description: Returns the number of nodes of the list.
+----------------------------------------------------------------------------*/
template <class Item>
inline int CIList<Item>::getNumItems () { return _count; }

/*----------------------------------------------------------------------------+
| Description: Verify if the list is empty.
+----------------------------------------------------------------------------*/
template <class Item>
inline bool CIList<Item>::isEmpty () { return (_count==0); }

/*----------------------------------------------------------------------------+
| Description: Verify if position points to the first/last node of the list.
+----------------------------------------------------------------------------*/
template <class Item>
inline bool CIList<Item>::isFirst (CIListNode<Item>* pos) { return (pos ==
 _first); }

template <class Item>
inline bool CIList<Item>::isLast (CIListNode<Item>* pos) { return (pos ==
 _last); }


/*----------------------------------------------------------------------------+
|			    Normal methods (Not inline)
+----------------------------------------------------------------------------*/
template <class Item>
CIList<Item>::CIList(bool acceptRepeated)
{
    _first = _last = _current = NULL;
    _count = 0;
    _acceptRepeated = acceptRepeated;
}

template <class Item>
CIList<Item>::~CIList()
{
    removeAllNodes();
}

/*----------------------------------------------------------------------------+
| Description: Unlink the last element(node) of(from) the list and returns it
| Output: A pointer to the element
+----------------------------------------------------------------------------*/
template <class Item>
Item CIList<Item>::pop ()
{
    Item tmp;

    tmp = _last->getItem();
    if (_last != _first) {
        _last = _last->prev();
        delete _last->next();
        _last->setNext(NULL);
    }
    else {
	delete _last;
	_last = NULL;
	_first = NULL;
    }
    _count--;
    _current = NULL;

    return tmp;
}

/*-----| Item insertion methods |-----*/
/*----------------------------------------------------------------------------+
| Description: Inserts a new node at the begin/end of the list, and returns a 
|	      pointer to that node.
| Input: The item we want to add to the list.
| Output: A pointer to the new node
+----------------------------------------------------------------------------*/
template <class Item>
CIListNode<Item>* CIList<Item>::insertHead (Item item)
{
    CIListNode<Item>* pos = new CIListNode<Item>(item);
    pos->setNext(_first);
    pos->setPrev(NULL);
    if (_first)
	_first->setPrev(pos);
    else
	_last = pos;
    _first = pos;
    _count++;
    return _first;
}
template <class Item>
CIListNode<Item>* CIList<Item>::insertTail (Item item)
{
    CIListNode<Item>* pos = new CIListNode<Item>(item);
    pos->setNext(NULL);
    pos->setPrev(_last);
    if (_last)
	_last->setNext(pos);
    else
	_first = pos;
    _last = pos;
    _count++;
    return _last;
}

/*----------------------------------------------------------------------------+
| Description: Appends another list to the current list.
+----------------------------------------------------------------------------*/
template <class Item>
void CIList<Item>::joinListAfter (CIList<Item>* list)
{
    int i;

    for (i=0; i < list->getNumItems(); i++)
	push((*list)[i]);
}

/*----------------------------------------------------------------------------+
| Description: Inserts a new node in ascendent order. If exists a node with
|              the same ordValue, the insertion is not performed.
| Input: The item we want to add to the list, and its order value.
| Output: NULL if there is no node with the same ordValue, 
|         or a pointer to the node with the same ordValue.
+----------------------------------------------------------------------------*/
template <class Item>
CIListNode<Item>* CIList<Item>::insertInOrder (Item item, double ordVal)
{
    CIListNode<Item>* pos = new CIListNode<Item>(item, ordVal);
    CIListNode<Item>* ptr;

    if (_first) {
	ptr = _first;
	while(ptr->next() && ptr->getOrdVal() < ordVal)
	    ptr = ptr->next();

        if (!_acceptRepeated && ptr->getOrdVal() == ordVal) { // we found a node
 							      // with the same ordVal
	    delete pos;
            return ptr;
	}
        else
            if (!ptr->next() && ptr->getOrdVal() < ordVal) {
                pos->setNext(NULL);
                pos->setPrev(_last);
                if (_last)
	            _last->setNext(pos);
                else
	            _first = pos;
                _last = pos;
            }
	    else {
	        pos->setNext(ptr);
	        if (ptr->prev())
		    ptr->prev()->setNext(pos);
	        else
		    _first = pos;
	        pos->setPrev(ptr->prev());
	        ptr->setPrev(pos);
	    }
    }
    else {
        pos->setNext(NULL);
	pos->setPrev(NULL);
	_first = _last = pos;
    }
    _count++;
    return NULL;  // insertion OK!
}

/*----------------------------------------------------------------------------+
| Description: Inserts a new node in descendent order. If exists a node with
|              the same ordValue, the insertion is not performed.
| Input: The item we want to add to the list, and its order value.
| Output: NULL if there is no node with the same ordValue, 
|         or a pointer to the node with the same ordValue.
+----------------------------------------------------------------------------*/
template <class Item>
CIListNode<Item>* CIList<Item>::insertInRevOrder (Item item, double ordVal)
{
    CIListNode<Item>* pos = new CIListNode<Item>(item, ordVal);
    CIListNode<Item>* ptr;

    if (_first) {
	ptr = _first;
	while(ptr->next() && ptr->getOrdVal() > ordVal)
	    ptr = ptr->next();

        if (!_acceptRepeated && ptr->getOrdVal() == ordVal) { // we found a node
 							      // with the same ordVal
	    delete pos;
            return ptr;
	}
        else
            if (!ptr->next() && ptr->getOrdVal() > ordVal) {
                pos->setNext(NULL);
                pos->setPrev(_last);
                if (_last)
	            _last->setNext(pos);
                else
	            _first = pos;
                _last = pos;
            }
	    else {
	        pos->setNext(ptr);
	        if (ptr->prev())
		    ptr->prev()->setNext(pos);
	        else
		    _first = pos;
	        pos->setPrev(ptr->prev());
	        ptr->setPrev(pos);
	    }
    }
    else {
        pos->setNext(NULL);
	pos->setPrev(NULL);
	_first = _last = pos;
    }
    _count++;
    return NULL;  // insertion OK!
}

/*----------------------------------------------------------------------------+
| Description: Inserts a new node before/after the node pointed by pos, and 
|	      returns a pointer to that new node.
| Input: The item we want to add to the list and the position.
| Output: A pointer to the new node
+----------------------------------------------------------------------------*/
template <class Item>
CIListNode<Item>* CIList<Item>::insertBefore (CIListNode<Item>* pos, Item item)
{
    CIListNode<Item>* np = new CIListNode<Item>(item);

    if (pos != _first) {
        np->setNext(pos);
        pos->prev()->setNext(np);
	np->setPrev(pos->prev());
	pos->setPrev(np);
	_count++;
	return np;
    }
    else
	return insertHead(item);
}
template <class Item>
CIListNode<Item>* CIList<Item>::insertAfter (CIListNode<Item>* pos, Item item)
{
    CIListNode<Item>* np = new CIListNode<Item>(item);

    if (pos != _last) 
	return insertBefore(pos->next(), item);
    else
	return insertTail(item);
}

/*-----| Iteration methods |-----*/
/*----------------------------------------------------------------------------+
| Description: Returns the Item pointed by pos, and makes pos point to the 
|	       next/prev node in the list.
|
| Input: pos - current node of the list
| Output: The item of the current node, and a new value for pos, ie. a pointer 
|	  to the next/prev node in the list.
+----------------------------------------------------------------------------*/
template <class Item>
Item CIList<Item>::getNextItem (CIListNode<Item>* &pos)
{
    Item item = pos->getItem();

    pos = pos->next();
    return item;
}
template <class Item>
Item CIList<Item>::getPrevItem (CIListNode<Item>* &pos)
{
    Item item = pos->getItem();

    pos = pos->prev();
    return item;
}

/*----------------------------------------------------------------------------+
| Description: Remove the node pointed by pos
| Input: pos - A pointer to the node
+----------------------------------------------------------------------------*/
template <class Item>
void CIList<Item>::removeAt (CIListNode<Item>* pos)
{
    if (pos->prev())
        pos->prev()->setNext(pos->next());
    else if (pos->next()) 
	_first = pos->next();
    else _first=NULL;

    if (pos->next())
	pos->next()->setPrev(pos->prev());
    else if (pos->prev())
	_last = pos->prev();
    else _last=NULL;

    delete pos;
    _count--;
    _current = NULL;
}

/*----------------------------------------------------------------------------+
| Description: 	Remove all nodes from the list, but the items of the list 
|		still exists.
|
| Note: DO NOT FORGET to delete the items before destroying the list 
| 	(if you want to delete them)
+----------------------------------------------------------------------------*/
template <class Item>
void CIList<Item>::removeAllNodes ()
{
    // deleteAll(_first);

    CIListNode<Item>* actual=_first;
    CIListNode<Item>* next;
    while(actual!=NULL){
	next=actual->next();
	delete actual;
	actual=next;
    }
    _count = 0;
    _first = NULL;
    _last = NULL;
    _current = NULL;
}

/*----------------------------------------------------------------------------+
| Description: Finds an item in the list 
| Output: A pointer to the node containing the item
+----------------------------------------------------------------------------*/
template <class Item>
CIListNode<Item>* CIList<Item>::findItem (Item item, CIListNode<Item>* pos)
{
    if (!pos)
	pos = _first;
    while(pos && (item != pos->getItem()))
	pos = pos->next();
    return pos;
}

/*----------------------------------------------------------------------------+
| Description: Removes the first occurrence of an item from the list
+----------------------------------------------------------------------------*/
template <class Item>
void CIList<Item>::removeFirstOcc (Item item)
{
    CIListNode<Item>* temp=findItem(item);
    removeAt(temp);
}

/*----------------------------------------------------------------------------+
| Description: Allows the use of the list as an Array.
+----------------------------------------------------------------------------*/
template <class Item>
Item& CIList<Item>::operator[] (int index)
{
    int dif1, dif2, i, idx=0;
    CIListNode<Item>* ptr;

    if (index <0 || index >= _count) {
	fprintf(stderr, "PANIC! Index (%d) out of bounds [0,%d]\n", index, _count-1);	
	exit (8);  // Index out of bounds
    }

    // distances to the index position, from the begining, the end and the
    // middle.
    
    if (_current) {
        idx = _current->getIdx();
	dif1 = index - idx;
    }
    else
	dif1 = _count;
    dif2 = _count - index -1;
    
    if (index < abs(dif1) && index < dif2) {
	ptr = _first;
	for (i=0; i<index; i++)
	    ptr = ptr->next();
    }
    else if (abs(dif1) < index && abs(dif1) < dif2) {
	ptr = _current;
	if (dif1 > 0)
	    for (i=idx; i<index; i++)
		ptr = ptr->next();
	else
	    for (i=idx; i>index; i--)
		ptr = ptr->prev();
    }
    else {
	ptr = _last;
	for (i=_count-1; i>index; i--)
	    ptr = ptr->prev();
    }
    if (_current)
	_current->setIdx(-1);
    _current = ptr;
    _current->setIdx(index);
    return (_current->getItem());
}

/*----------------------------------------------------------------------------+
|		     Protected members of the class
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| Description: Delete recursively all the nodes of the list.
+----------------------------------------------------------------------------*/
template <class Item>
void CIList<Item>::deleteAll (CIListNode<Item>* pos)
{
    if (pos != NULL) {
	deleteAll(pos->next());
	delete pos;
    }
}
    
#endif // _CILIST_H
