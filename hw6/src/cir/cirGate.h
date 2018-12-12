/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

class pin
{
public:
	#define NEG 0x1
	pin(CirGate* g, size_t phase): 
		_gateV(size_t(g) + phase) {}
	CirGate* gate() const {
		return (CirGate*)(_gateV & ~size_t(NEG)); }
	bool isInv() const { return (_gateV & NEG); }
private:
	size_t           _gateV;
};

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
	CirGate() {}
	CirGate(unsigned ln = 0, unsigned id = 0):_LineNo(ln), _id(id), _symbol(""), _ref(0) {}
	virtual ~CirGate() {}

	// Basic access methods
	virtual string getTypeStr() const { return ""; }
	unsigned getLineNo() const { return _LineNo; }
	unsigned getId() const { return _id; }

	// Printing functions
	virtual void printGate() const = 0;
	void reportGate() const;
	void reportFanin(int level) const;
	void reportFanout(int level) const;

	void setSymbol(string &str) { _symbol = str; }
	string getSymbol() const { return _symbol; }

	// Setting functions
	void addFaninId(const unsigned &id) { _faninId.push_back(id); }
	unsigned getFaninId(const size_t &idx) const { return _faninId[idx]; }
	unsigned getFaninIdSize() const { return _faninId.size(); }
	//void addFanoutId(const unsigned &id) { _fanoutId.push_back(id); }
	//unsigned getFanoutId(const size_t &idx) { return _fanoutId[idx]; }

	void addFaninPin(pin const &g) { _faninList.push_back(g); }
	pin getFaninPin(const size_t &idx) const { return _faninList[idx]; }
	void addFanoutPin(pin const &g) { _fanoutList.push_back(g); }
	pin getFanoutPin(const size_t &idx) const { if(_fanoutList.size()) return _fanoutList[idx]; return pin(0,0); }
	unsigned getFanoutPinSize() const { return _fanoutList.size(); }

	// Dfs functions
	bool isGlobalRef() const { return (_ref == _globalRef); }
	void setToGlobalRef() { _ref = _globalRef; }
	static void setGlobalRef() { ++_globalRef; }
	void preOrderReport(const CirGate* g, int &cnt, int &level, const char &usage, bool inv) const;

private:
	vector<pin>   			_faninList;
	IdList					_faninId;
	vector<pin> 			_fanoutList;
	//IdList					_fanoutId;
	unsigned 				_LineNo;
	unsigned					_id;
	string 					_symbol;
	static unsigned		_globalRef;
	unsigned					_ref;

protected:
	
};

class AIG : public CirGate
{
public:
	AIG(unsigned ln = 0, unsigned id = 0):CirGate(ln, id) {}
	~AIG() {}

	// Basic access methods
	string getTypeStr() const { return "AIG";}

	// Printing functions
	void printGate() const {}
};

class PI : public CirGate
{
public:
	PI(unsigned ln = 0, unsigned id = 0):CirGate(ln, id) {}
	~PI() {}

	// Basic access methods
	string getTypeStr() const { return "PI";}

	// Printing functions
	void printGate() const {}
	
};

class PO : public CirGate
{
public:
	PO(unsigned ln = 0, unsigned id = 0):CirGate(ln, id) {}
	~PO() {}

	// Basic access methods
	string getTypeStr() const { return "PO";}

	// Printing functions
	void printGate() const {}

};

class Const0 : public CirGate
{
public:
	Const0():CirGate(0,0) {}
	~Const0() {}

	// Basic access methods
	string getTypeStr() const { return "CONST";}

	// Printing functions
	void printGate() const {}
};

#endif // CIR_GATE_H
