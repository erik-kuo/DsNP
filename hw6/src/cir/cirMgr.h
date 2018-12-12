/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr():_gates(0) { _const = new Const0();}
   ~CirMgr()
   {
   	delete _const;
   	delete _gates;
   	for(int i = 0; i < _PIs.size(); ++i)
   		delete _PIs[i];
   	_PIs.shrink_to_fit();
   	for(int i = 0; i < _POs.size(); ++i)
   		delete _POs[i];
   	_POs.shrink_to_fit();
   	for(int i = 0; i < _AIGs.size(); ++i)
   		delete _AIGs[i];
   	_AIGs.shrink_to_fit();
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const
   {
   	if(gid <= _maxId)
   		return _gates[gid];
   	return 0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

   // Dfs traversal
	void dfs(const CirGate* g, unsigned &cnt, const char &usage, ostream& os) const;

private:
	CirGate*		_const;
   GateList		_PIs;
   GateList		_POs;
   GateList		_AIGs;
   CirGate**	_gates;
   IdList		_float;
   IdList		_unused;
   unsigned 	_maxId;
};

#endif // CIR_MGR_H
