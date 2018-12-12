/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const
{
  	cout << "==================================================" << endl;
  	stringstream ss;
  	string report;
  	ss << "= " << getTypeStr() << "(" << getId() << ")" ;
	if(getSymbol() != "")
		ss << "\"" << getSymbol() << "\"";
	ss << ", line " << getLineNo() ;
  	getline(ss, report);
	cout << setw(49) << left << report << "=" << endl;
	cout << "==================================================" << endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   int cnt = 0;
   preOrderReport(this, cnt, level, 'i', 0);
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   setGlobalRef();
   int cnt = 0;
   preOrderReport(this, cnt, level, 'o', 0);
}

unsigned CirGate::_globalRef = 0;

void
CirGate::preOrderReport(const CirGate* g, int &cnt, int &level, const char &usage, bool inv) const
{
	for(size_t i = 0; i < cnt; ++i)
		cout << "  ";
	if(inv)
		cout << "!";
	cout << g->getTypeStr() << " " << g->getId();
	if(g->isGlobalRef() && cnt < level && g->getTypeStr() != "PI" && g->getTypeStr() != "PO" && g->getTypeStr() != "CONST")
		cout << " (*)";
	cout << endl;
	if(g->isGlobalRef() && cnt < level)
	{
		--cnt;
		return;
	}
	if(usage == 'i')
	{
		for(size_t i = 0; i < g->getFaninIdSize(); ++i)
		{
			if (g->getFaninPin(i).gate())
			{
				if(cnt < level)
				{
					preOrderReport(g->getFaninPin(i).gate(), ++cnt, level, usage, g->getFaninPin(i).isInv());
					if(cnt < level - 1)
						g->getFaninPin(i).gate()->setToGlobalRef();
				}
			}
			else		// Floating gate
			{
				++cnt;
				if(cnt <= level)
				{
					for(size_t j = 0; j < cnt; ++j)
						cout << "  ";
					if(g->getFaninPin(i).isInv())
						cout << "!";
					cout << "UNDEF" << " " << g->getFaninId(i) / 2 << endl;
				}
				--cnt;
			}
		}
	}
	else
	{
		for(int i = g->getFanoutPinSize() - 1; i >= 0 ; --i)
		{
			if (g->getFanoutPin(i).gate())
			{
				if(cnt < level)
				{
					preOrderReport(g->getFanoutPin(i).gate(), ++cnt, level, usage, g->getFanoutPin(i).isInv());
					if(cnt < level - 1)
						g->getFanoutPin(i).gate()->setToGlobalRef();
				}
			}
		}
	}
	--cnt;
}