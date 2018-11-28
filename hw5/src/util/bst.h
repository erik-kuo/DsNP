/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d,BSTreeNode<T>* p = 0, BSTreeNode<T>* r = 0, BSTreeNode<T>* l = 0):
         _data(d), _parent(p), _left(l), _right(r) {}

   T               _data;
   BSTreeNode<T>*  _parent;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree():_root(0) {}
   ~BSTree() { clear(); }

   class iterator
   {
   	friend class BSTree;

   public:
   	iterator(BSTreeNode<T>* n = 0): _node(n), _endFlag(0) {}
   	iterator(const iterator& i) : _node(i._node), _endFlag(i._endFlag) {}
   	~iterator() {}

   	const T& operator * () const { return _node->_data; }
   	T& operator * () { return _node->_data; }
   	iterator& operator ++ ()
   	{
   		BSTreeNode<T>* suc = successor();
      	if(suc)
      	{
   			_node = suc;
      	}
   		//no succer in right child tree
   		else
   		{
   			BSTreeNode<T>* tmp = _node;
   			if(tmp->_parent)
   			{
   				while(tmp->_parent->_right == tmp)
   				{
	   				tmp = tmp->_parent;
	   				if(tmp == 0) break;
	   				if(tmp->_parent == 0) break;
   				}
   				tmp = tmp->_parent;
   			}
   			else
   				tmp = 0;
   			if(tmp)
   				_node = tmp;
   			else // means at the --end()
   			{
   				_endFlag = 1;
   			}
   		}
   		return *(this);
   	}
      iterator operator ++ (int)
      {
      	iterator tmp(*this);
      	++(*this);
      	return tmp;
      }
      iterator& operator -- ()
      {
      	if(_endFlag)
      		_endFlag = 0;
      	else
      	{
      		BSTreeNode<T>* pres = presuccessor();
	      	if(pres)
	      	{
	   			_node = pres;
	      	}
	   		//no succer in left child tree
	   		else
	   		{
	   			BSTreeNode<T>* tmp = _node;
	   			if(tmp->_parent)
	   			{
						while(tmp->_parent->_left == tmp )
	   				{
		   				tmp = tmp->_parent;
		   				if(tmp == 0) break;
		   				if(tmp->_parent == 0) break;
		   			}
		   			tmp = tmp->_parent;
	   			}
	   			if(tmp)
		   			_node = tmp;
		   		else
		   		{
		   			_node = 0;
		   		}
	   		}
	   	}
      	return *(this);
      }
      iterator operator -- (int)
      {
      	iterator tmp(*this);
      	--(*this);
      	return tmp;
      }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return ((_node != i._node) || (_endFlag != i._endFlag)); }
      bool operator == (const iterator& i) const { return ((_node == i._node) && (_endFlag == i._endFlag)); }

      BSTreeNode<T>* min(BSTreeNode<T>* travel)
      {
      	while(travel->_left)
      		travel = travel->_left;
      	return travel;
      }
      BSTreeNode<T>* max(BSTreeNode<T>* travel)
      {
      	while(travel->_right)
      		travel = travel->_right;
      	return travel;
      }

      BSTreeNode<T>* successor()
      {
      	if(_node)
      		if(_node->_right)
      			return min(_node->_right);
      	return 0;                        //no successor in right child tree 
      }
      BSTreeNode<T>* presuccessor()
      {
      	if(_node)
	      	if(_node->_left)
					return max(_node->_left);
      	return 0;                        //no successor in right child tree 
      }

   private:
   	BSTreeNode<T>*        _node;
   	bool 						 _endFlag;

   };

   iterator begin() const
   {
   	iterator li(_root);
   	if(!empty())
   		while(li._node->_left)
	   		li._node = li._node->_left;
   	return li;
   }
   iterator end() const //IDK rrrrrrrrrrrrrrrrrrrrrrrrr
   {
   	iterator li(_root);
   	if(!empty())
   	{
   		while(li._node->_right)
	   		li._node = li._node->_right;
	   	li._endFlag = 1;
	   }
   	return li;
   }
   bool empty() const { if(_root) return false; return true; }
   size_t size() const
   {
   	if(empty()) return 0;
   	size_t cnt = 0;
   	InOrderCnt(cnt, _root);
   	return cnt;
   }

   void insert(const T& x) { insert(x, _root); }
   void pop_front()
   {
   	if(empty()) return;
   	erase(begin());
   }
   void pop_back()
   {
   	if(empty()) return;
   	erase(--end());
   }

   bool erase(iterator pos)
   {
   	if(empty()) return false;
   	if(pos._node->_left == 0)
	   {
	   	if(pos._node->_right == 0) // leaf
	   	{
	   		if(pos._node == _root)
	   			_root = 0;
	   		else
	   		{
	   			if(pos._node->_parent->_left == pos._node)
		  				pos._node->_parent->_left = 0;
		  			else
		  				pos._node->_parent->_right = 0;
	   		}
	   		delete pos._node;
	   	}
	   	else                      // one child at right
	   	{
	   		if(pos._node == _root)
	   			_root = pos._node->_right;
	   		else
	   		{
	   			if(pos._node->_parent->_left == pos._node)
		  				pos._node->_parent->_left = pos._node->_right;
		  			else
		  				pos._node->_parent->_right = pos._node->_right;
	   		}
	   		pos._node->_right->_parent = pos._node->_parent;
	   		delete pos._node;
	   	}
	   }
	   else
	   {
	   	if(pos._node->_right == 0) // one child at left
	   	{
	   		if(pos._node == _root)
	   			_root = pos._node->_left;
	   		else
	   		{
	   			if(pos._node->_parent->_left == pos._node)
		  				pos._node->_parent->_left = pos._node->_left;
		  			else
		  				pos._node->_parent->_right = pos._node->_left;
	   		}
	   		pos._node->_left->_parent = pos._node->_parent;
	   		delete pos._node;
	   	}
	   	else                      // two children
	   	{
	   		BSTreeNode<T>* suc = pos.successor();  //suc != 0 in this case
	   		if(suc->_parent->_left == suc)
	   			suc->_parent->_left = suc->_right;
	   		else
	   			suc->_parent->_right = suc->_right;
	   		if(suc->_right)
	   			suc->_right->_parent = suc->_parent;
	   		pos._node->_data = suc->_data;
	   		delete suc;
	   	}
	   }
   	return true;
   }
   bool erase(const T& x)
   {
   	iterator li = find(x);
   	if(li == end())
   		return false;
   	return erase(li);
   }

   iterator find(const T& x)
   {
   	if(empty()) return end();
   	iterator li(_root);
   	while(li._node != 0)
   	{
   		if(*li == x) return li;
   		if(*li > x)
   			li._node = li._node->_left;
   		else
   			li._node = li._node->_right;
   	}
   	return end();
   }

   void clear()
   {
   	PostOrderDelete(_root);
   }

   void sort() const {} //sort when insert new node 

   void print() const   //for verbose print option
   {
   	if(!empty())
   	{
   		size_t h = 0;
   		PreOrederPrint(h, _root);
   	}
   }

private:
	BSTreeNode<T>* _root; //the dummy node ,aka fake root

	void insert(const T& x, BSTreeNode<T>*& travel)
   {
   	if(travel == 0)
   	{
   		travel = new BSTreeNode<T>(x);
   		return;
   	}
   	if(x > travel->_data)
   	{
   		insert(x, travel->_right);
   		if(travel->_right->_parent == 0)
   			travel->_right->_parent = travel;
   	}
   	else
   	{
   		insert(x, travel->_left);
   		if(travel->_left->_parent == 0)
   			travel->_left->_parent = travel;
   	}
   }

   void InOrderCnt(size_t& cnt, BSTreeNode<T>* const& travel) const
   {
   	if(travel->_left)
   		InOrderCnt(cnt, travel->_left);
   	++cnt;
   	if(travel->_right)
   		InOrderCnt(cnt, travel->_right);
   }

   void PreOrederPrint(size_t& h, BSTreeNode<T>* const& travel) const
   {
   	for(size_t i = 0; i < h; ++i)
   		cout<<"  ";
   	if(travel) cout<< travel->_data <<endl;
   	else     { cout<< "[0]"  <<endl; return; }
   	PreOrederPrint(++h, travel->_left);
   	--h;
   	PreOrederPrint(++h, travel->_right);
   	--h;
   }

   void PostOrderDelete(BSTreeNode<T>*& travel)
   {
   	if(empty()) return;
   	if(travel->_left)
   		PostOrderDelete(travel->_left);
   	if(travel->_right)
   		PostOrderDelete(travel->_right);
   	delete travel;
   	travel = 0;
   }

};

#endif // BST_H
