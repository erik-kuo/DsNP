/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = true;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator tmp(*this); _node = _node->_next; return tmp; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator tmp(*this); _node = _node->_prev; return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node) ; }
      bool operator == (const iterator& i) const { return (_node == i._node) ; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head->_next == _head) ; }
   size_t size() const
   {
      size_t cnt = 0;
      for(iterator li = begin(); li != end(); ++li)
         ++cnt;
      return cnt;
   }

   void push_back(const T& x)
   {
      DListNode<T>* newNode = new DListNode<T>(x, _head->_prev, _head);
      _head->_prev->_next = newNode;
      _head->_prev = newNode;
      if(x < newNode->_prev->_data)
         _isSorted = false;
   }
   void pop_front() { erase(iterator(_head->_next)); }
   void pop_back() { erase(iterator(_head->_prev)); }

   // return false if nothing to erase
   bool erase(iterator pos)
   {
      if(empty()) return false;
      pos._node->_prev->_next = pos._node->_next;
      pos._node->_next->_prev = pos._node->_prev;
      delete pos._node;
      return true;
   }
   bool erase(const T& x) { iterator tmp = find(x); if(tmp == end()) return false; return erase(tmp); }

   iterator find(const T& x)
   {
      for(iterator li = begin(); li != end(); ++li)
         if(*li == x)
            return li;
      return end();
   }

   void clear() // delete all nodes except for the dummy node
   {
      if(!empty())
      {
         for(iterator li = begin(); li != end(); ++li)
            delete li._node;
         _isSorted = true;
         _head->_prev = _head->_next = _head;
      }
   }

   void sort() const
   {
      if(empty() || _isSorted) return;
      /* bubble sort
      for(iterator right = begin(); right != end(); ++right)
         for(iterator left = begin(); left != right; ++left)
            if(*right < *left)
               swap(*right,*left);
      */
      quickSort(begin(), --end());
      _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node forever XDDDDD ///////if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
   void quickSort(iterator left, iterator pivot) const 
   {
      if(left != pivot && left._node != pivot._node->_next)
      {
         iterator stored = left;
         for(iterator tmp = left; tmp._node != pivot._node; ++tmp)
         {
            if(*tmp < *pivot)
            {
               swap(*tmp, *stored);
               ++stored;
            }
         }
         swap(*pivot, *stored);
         quickSort(left, --stored);
         ++stored;
         quickSort(++stored, pivot);
      }
   }
};

#endif // DLIST_H
