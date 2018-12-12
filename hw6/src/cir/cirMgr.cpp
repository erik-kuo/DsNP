/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
	EXTRA_SPACE,
	MISSING_SPACE,
	ILLEGAL_WSPACE,
	ILLEGAL_NUM,
	ILLEGAL_IDENTIFIER,
	ILLEGAL_SYMBOL_TYPE,
	ILLEGAL_SYMBOL_NAME,
	MISSING_NUM,
	MISSING_IDENTIFIER,
	MISSING_NEWLINE,
	MISSING_DEF,
	CANNOT_INVERTED,
	MAX_LIT_ID,
	REDEF_GATE,
	REDEF_SYMBOLIC_NAME,
	REDEF_CONST,
	NUM_TOO_SMALL,
	NUM_TOO_BIG,

	DUMMY_END
};

/**************************************/
/*   Static variables and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
	switch (err) {
		case EXTRA_SPACE:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Extra space character is detected!!" << endl;
			break;
		case MISSING_SPACE:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Missing space character!!" << endl;
			break;
		case ILLEGAL_WSPACE: // for non-space white space character
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Illegal white space char(" << errInt
				  << ") is detected!!" << endl;
			break;
		case ILLEGAL_NUM:
			cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
				  << errMsg << "!!" << endl;
			break;
		case ILLEGAL_IDENTIFIER:
			cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
				  << errMsg << "\"!!" << endl;
			break;
		case ILLEGAL_SYMBOL_TYPE:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Illegal symbol type (" << errMsg << ")!!" << endl;
			break;
		case ILLEGAL_SYMBOL_NAME:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Symbolic name contains un-printable char(" << errInt
				  << ")!!" << endl;
			break;
		case MISSING_NUM:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Missing " << errMsg << "!!" << endl;
			break;
		case MISSING_IDENTIFIER:
			cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
				  << errMsg << "\"!!" << endl;
			break;
		case MISSING_NEWLINE:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": A new line is expected here!!" << endl;
			break;
		case MISSING_DEF:
			cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
				  << " definition!!" << endl;
			break;
		case CANNOT_INVERTED:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": " << errMsg << " " << errInt << "(" << errInt/2
				  << ") cannot be inverted!!" << endl;
			break;
		case MAX_LIT_ID:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
				  << endl;
			break;
		case REDEF_GATE:
			cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
				  << "\" is redefined, previously defined as "
				  << errGate->getTypeStr() << " in line " << errGate->getLineNo()
				  << "!!" << endl;
			break;
		case REDEF_SYMBOLIC_NAME:
			cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
				  << errMsg << errInt << "\" is redefined!!" << endl;
			break;
		case REDEF_CONST:
			cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
				  << ": Cannot redefine constant (" << errInt << ")!!" << endl;
			break;
		case NUM_TOO_SMALL:
			cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
				  << " is too small (" << errInt << ")!!" << endl;
			break;
		case NUM_TOO_BIG:
			cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
				  << " is too big (" << errInt << ")!!" << endl;
			break;
		default: break;
	}
	return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
	ifstream ifs(fileName.c_str());
	if(!ifs)
	{
		cerr << "Cannot open design \"" << fileName << "\"!!"<<endl;
		return false;
	}

	string header, token;
	char parser;
	int m,i,l,o,a,id;
	lineNo = 0, colNo = 0;


	getline(ifs, header);

	// Check aag
	if(header == "")
	{
		errMsg = "aag";
		return parseError(MISSING_IDENTIFIER);
	}

	// Check space before "aag"
	size_t begin = header.find_first_not_of(' ', 0);
	if(begin != 0)
		return parseError(EXTRA_SPACE);
	begin = header.find_first_not_of('\t', 0);
	if(begin != 0)
	{
		errInt = int('\t');
		return parseError(ILLEGAL_WSPACE);
	}

	// No space before "aag"
	colNo = 3;
	size_t end =  myStrGetTok(header, token);
	//cout << "token : \"" << token << "\"" << endl;
	if(token != "aag")
	{
		if(token.size() > 3)
			if(isdigit(token[3]) || token[3] == '\t')
				return parseError(MISSING_SPACE);
		errMsg = token;
		return parseError(ILLEGAL_IDENTIFIER);
	}

	// Check space before m
	if(end == string::npos)
	{
		errMsg = "number of variables";
		return parseError(MISSING_NUM);
	}
	++colNo; // 4 -> + 1 = 5
	begin = header.find_first_not_of(' ', end);
	if(begin == string::npos)
	{
		errMsg = "number of variables";
		return parseError(MISSING_NUM);
	}
	if(begin != end + 1)
		return parseError(EXTRA_SPACE);
	end = myStrGetTok(header, token, end);
	//cout << "token : \"" << token << "\"" << endl;
	begin = token.find_first_of('\t');
	if(begin == 0)
	{
		errInt = int('\t');
		return parseError(ILLEGAL_WSPACE);
	}
	if(begin != string::npos)
	{
		colNo += begin;
		return parseError(MISSING_SPACE);
	}
	if(!myStr2Int(token, m))
	{
		errMsg = "number of variables(";
		errMsg += token;
		errMsg += ")";
		return parseError(ILLEGAL_NUM);
	}
	colNo += token.size(); // 5 -> + 1 = 6
	//cout << "pass m" <<endl;

	// Check space before i
	if(end == string::npos)
	{
		errMsg = "number of PIs";
		return parseError(MISSING_NUM);
	}
	++colNo; // 6 -> + 1 = 7
	begin = header.find_first_not_of(' ', end);
	if(begin == string::npos)
	{
		errMsg = "number of PIs";
		return parseError(MISSING_NUM);
	}
	if(begin != end + 1)
		return parseError(EXTRA_SPACE);
	end = myStrGetTok(header, token, end);
	//cout << "token : \"" << token << "\"" << endl;
	begin = token.find_first_of('\t');
	if(begin == 0)
	{
		errInt = int('\t');
		return parseError(ILLEGAL_WSPACE);
	}
	if(begin != string::npos)
	{
		colNo += begin;
		return parseError(MISSING_SPACE);
	}
	if(!myStr2Int(token, i))
	{
		errMsg = "number of PIs(";
		errMsg += token;
		errMsg += ")";
		return parseError(ILLEGAL_NUM);
	}
	colNo += token.size(); // 7 -> + 1 = 8
	//cout << "pass i" <<endl;

	// Check space before l
	if(end == string::npos)
	{
		errMsg = "number of latches";
		return parseError(MISSING_NUM);
	}
	++colNo; // 8 -> + 1 = 9
	begin = header.find_first_not_of(' ', end);
	if(begin == string::npos)
	{
		errMsg = "number of latches";
		return parseError(MISSING_NUM);
	}
	if(begin != end + 1)
		return parseError(EXTRA_SPACE);
	end = myStrGetTok(header, token, end);
	//cout << "token : \"" << token << "\"" << endl;
	begin = token.find_first_of('\t');
	if(begin == 0)
	{
		errInt = int('\t');
		return parseError(ILLEGAL_WSPACE);
	}
	if(begin != string::npos)
	{
		colNo += begin;
		return parseError(MISSING_SPACE);
	}
	if(!myStr2Int(token, l))
	{
		errMsg = "number of latches(";
		errMsg += token;
		errMsg += ")";
		return parseError(ILLEGAL_NUM);
	}
	colNo += token.size(); // 9 -> + 1 = 10
	//cout << "pass l" <<endl;

	// Check space before o
	if(end == string::npos)
	{
		errMsg = "number of POs";
		return parseError(MISSING_NUM);
	}
	++colNo; // 10 -> + 1 = 11
	begin = header.find_first_not_of(' ', end);
	if(begin == string::npos)
	{
		errMsg = "number of POs";
		return parseError(MISSING_NUM);
	}
	if(begin != end + 1)
		return parseError(EXTRA_SPACE);
	end = myStrGetTok(header, token, end);
	//cout << "token : \"" << token << "\"" << endl;
	begin = token.find_first_of('\t');
	if(begin == 0)
	{
		errInt = int('\t');
		return parseError(ILLEGAL_WSPACE);
	}
	if(begin != string::npos)
	{
		colNo += begin;
		return parseError(MISSING_SPACE);
	}
	if(!myStr2Int(token, o))
	{
		errMsg = "number of POs(";
		errMsg += token;
		errMsg += ")";
		return parseError(ILLEGAL_NUM);
	}
	colNo += token.size(); // 11 -> + 1 = 12
	//cout << "pass o" <<endl;

	// Check space before a
	if(end == string::npos)
	{
		errMsg = "number of AIGs";
		return parseError(MISSING_NUM);
	}
	++colNo; // 12-> + 1 = 13
	begin = header.find_first_not_of(' ', end);
	if(begin == string::npos)
	{
		errMsg = "number of AIGs";
		return parseError(MISSING_NUM);
	}
	if(begin != end + 1)
		return parseError(EXTRA_SPACE);
	end = myStrGetTok(header, token, end);
	//cout << "token : \"" << token << "\"" << endl;
	begin = token.find_first_of('\t');
	if(begin == 0)
	{
		errInt = int('\t');
		return parseError(ILLEGAL_WSPACE);
	}
	if(begin != string::npos)
	{
		colNo += begin;
		return parseError(MISSING_NEWLINE);
	}
	if(!myStr2Int(token, a))
	{
		errMsg = "number of AIGs(";
		errMsg += token;
		errMsg += ")";
		return parseError(ILLEGAL_NUM);
	}
	colNo += token.size(); // 13 -> + 1 = 14
	//cout << "pass a" <<endl;

	// Check after a
	if(end != string::npos)
		return parseError(MISSING_NEWLINE);

	// Check m >= i + a
	if(m < i + a)
	{
		errMsg = "Number of variables";
		errInt = m;
		return parseError(NUM_TOO_SMALL);
	}

	// Check l == 0
	if(l != 0)
	{
		errMsg = "latches";
		return parseError(ILLEGAL_NUM);
	}
	
	_maxId = m + o + 1;

	//cout<<"read MILOA:"<<m<<","<<i<<","<<l<<","<<o<<","<<a<<endl;
	_gates = new CirGate* [m + o + 1] {0};

	//const 0
	_gates[0] = _const;
	//cout<<"id:"<<0<<", gate:const 0, line:"<<lineNo<<endl;
	//PI
	for(size_t j = 0; j < i; ++j)
	{
		++lineNo;
		colNo = 0;

		getline(ifs, header);

		// Check has def PI or not
		if(ifs.eof())
		{
			errMsg = "PI";
			return parseError(MISSING_DEF);
		}
		if(header == "")
		{
			errMsg = "PI literal ID";
			return parseError(MISSING_NUM);
		}

		// Check space before PI
		begin = header.find_first_not_of(' ', 0);
		if(begin != 0)
			return parseError(EXTRA_SPACE);
		end = myStrGetTok(header, token);
		//cout << "token : \"" << token << "\"" << endl;
		begin = token.find_first_of('\t');
		if(begin == 0)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}
		if(!myStr2Int(token, id))
		{
			errMsg = "PI literal ID(";
			errMsg += token;
			errMsg += ")";
			return parseError(ILLEGAL_NUM);
		}
		if(begin != string::npos)
		{
			colNo += begin;
			return parseError(MISSING_NEWLINE);
		}

		// Check reDef CONST
		if(id < 2)
		{
			errInt = id;
			return parseError(REDEF_CONST);
		}

		// Check exceed max
		if((id / 2) > m)
		{
			errInt = id;
			return parseError(MAX_LIT_ID);
		}

		// Check PI invert
		if(id % 2 != 0)
		{
			errMsg = "PI";
			errInt = id;
			return parseError(CANNOT_INVERTED);
		}

		// Check reDef PI
		if(_gates[id / 2])
		{
			errGate = _gates[id / 2];
			errInt = id;
			return parseError(REDEF_GATE);			
		}
		//cout << "pass PI" <<endl;

		id /= 2;

		// Check after def PI
		colNo += token.size(); // 1 -> + 1 = 2
		if(end != string::npos)
			return parseError(MISSING_NEWLINE);
		
		CirGate* newPI = new PI(lineNo + 1, id);
		_PIs.push_back(newPI);
		_gates[id] = newPI;
		//cout << "id:"<<id<<", gate:PI, line:"<<lineNo<<endl;
	}
	//PO
	for(size_t j = 0; j < o; ++j)
	{
		int fanInId;
		++lineNo;
		colNo = 0;

		getline(ifs, header);

		// Check has def PO or not
		if(ifs.eof())
		{
			errMsg = "PO";
			return parseError(MISSING_DEF);
		}
		if(header == "")
		{
			errMsg = "PO literal ID";
			return parseError(MISSING_NUM);
		}

		// Check space before PO
		begin = header.find_first_not_of(' ', 0);
		if(begin != 0)
			return parseError(EXTRA_SPACE);
		end = myStrGetTok(header, token);
		//cout << "token : \"" << token << "\"" << endl;
		begin = token.find_first_of('\t');
		if(begin == 0)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}
		if(begin != string::npos)
		{
			colNo += begin;
			return parseError(MISSING_NEWLINE);
		}
		if(!myStr2Int(token, fanInId))
		{
			errMsg = "PO literal ID(";
			errMsg += token;
			errMsg += ")";
			return parseError(ILLEGAL_NUM);
		}

		// Check exceed max
		if((fanInId / 2) > m)
		{
			errInt = fanInId;
			return parseError(MAX_LIT_ID);
		}
		//cout << "pass PO" << endl;

		// Check after def PO
		colNo += token.size(); // 1 -> + 1 = 2
		if(end != string::npos)
			return parseError(MISSING_NEWLINE);

		CirGate* newPO = new PO(lineNo + 1, m + j + 1);
		_POs.push_back(newPO);
		_gates[m + j + 1] = newPO;
		newPO->addFaninId(fanInId);
		//cout << "id:"<<m+j+1<<", gate:PO, line:"<<lineNo<<", fanin:"<<fanInId/2<<endl;
	}
	//AIG
	for(size_t j = 0; j < a; ++j)
	{
		int id, fanInId1, fanInId2;
		++lineNo;
		colNo = 0;

		getline(ifs, header);

		// Check has def AIG or not
		if(ifs.eof())
		{
			errMsg = "AIG";
			return parseError(MISSING_DEF);
		}
		if(header == "")
		{
			errMsg = "AIG gate literal ID";
			return parseError(MISSING_NUM);
		}

		// Check space before AIG
		begin = header.find_first_not_of(' ', 0);
		if(begin != 0)
			return parseError(EXTRA_SPACE);
		end = myStrGetTok(header, token);
		//cout << "token : \"" << token << "\"" << endl;
		begin = token.find_first_of('\t');
		if(begin == 0)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}
		if(begin != string::npos)
		{
			colNo += begin;
			return parseError(MISSING_SPACE);
		}
		if(!myStr2Int(token, id))
		{
			errMsg = "AIG gate literal ID(";
			errMsg += token;
			errMsg += ")";
			return parseError(ILLEGAL_NUM);
		}

		// Check reDef CONST
		if(id < 2)
		{
			errInt = id;
			return parseError(REDEF_CONST);
		}

		// Check exceed max
		if((id / 2) > m)
		{
			errInt = id;
			return parseError(MAX_LIT_ID);
		}

		// Check AIG invert
		if(id % 2 != 0)
		{
			errMsg = "AIG gate";
			errInt = id;
			return parseError(CANNOT_INVERTED);
		}

		// Check reDef AIG
		if(_gates[id / 2])
		{
			errGate = _gates[id / 2];
			errInt = id;
			return parseError(REDEF_GATE);			
		}
		//cout << "pass AIG" <<endl;

		// Check space before fanInId1
		colNo += token.size(); // 1 -> + 1 = 2
		if(end == string::npos)
			return parseError(MISSING_SPACE);
		++colNo; // 2 -> + 1 = 3
		begin = header.find_first_not_of(' ', end);
		if(begin == string::npos)
		{
			errMsg = "AIG input literal ID";
			return parseError(MISSING_NUM);
		}
		if(begin != end + 1)
			return parseError(EXTRA_SPACE);
		end = myStrGetTok(header, token, end);
		//cout << "token : \"" << token << "\"" << endl;
		begin = token.find_first_of('\t');
		if(begin == 0)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}
		if(begin != string::npos)
		{
			colNo += begin;
			return parseError(MISSING_SPACE);
		}
		if(!myStr2Int(token, fanInId1))
		{
			errMsg = "AIG input literal ID(";
			errMsg += token;
			errMsg += ")";
			return parseError(ILLEGAL_NUM);
		}

		// Check exceed max
		if((fanInId1 / 2) > m)
		{
			errInt = fanInId1;
			return parseError(MAX_LIT_ID);
		}
		//cout << "pass fanInId1" << endl;

		// Check space before fanInId2
		colNo += token.size(); // 3 -> + 1 = 4
		if(end == string::npos)
		{
			errMsg = "AIG input literal ID";
			return parseError(MISSING_NUM);
		}
		++colNo; // 4 -> + 1 = 5
		begin = header.find_first_not_of(' ', end);
		if(begin == string::npos)
			return parseError(MISSING_SPACE);
		if(begin != end + 1)
			return parseError(EXTRA_SPACE);
		end = myStrGetTok(header, token, end);
		//cout << "token : \"" << token << "\"" << endl;
		begin = token.find_first_of('\t');
		if(begin == 0)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}
		if(begin != string::npos)
		{
			colNo += begin;
			return parseError(MISSING_NEWLINE);
		}
		if(!myStr2Int(token, fanInId2))
		{
			errMsg = "AIG input literal ID(";
			errMsg += token;
			errMsg += ")";
			return parseError(ILLEGAL_NUM);
		}

		// Check exceed max
		if((fanInId2 / 2) > m)
		{
			errInt = fanInId2;
			return parseError(MAX_LIT_ID);
		}
		//cout << "pass fanInId1" << endl;

		id /= 2;

		// Check after def AIG
		colNo += token.size(); // 5 -> + 1 = 6
		if(end != string::npos)
			return parseError(MISSING_NEWLINE);

		CirGate* newAIG = new AIG(lineNo + 1, id);
		_AIGs.push_back(newAIG);
		_gates[id] = newAIG;
		newAIG->addFaninId(fanInId1);
		newAIG->addFaninId(fanInId2);
		//cout << "id:"<<id<<", gate:AIG, line:"<<lineNo<<", fanin1:"<<fanInId1/2<<", fainin2:"<<fanInId2/2<<endl;
	}
	// Symbol
	string symbol;
	do
	{
		++lineNo;
		colNo = 0;
		getline(ifs, header);
		//cout << "header:\""<<header<<"\""<<endl;

		// No symbol
		if(ifs.eof())
		{
			symbol = "";
			break;
		}
		if(header == "")
		{
			errMsg = "";
			errMsg += char(0x00);
			return parseError(ILLEGAL_SYMBOL_TYPE);
		}
		// Has symbol

		// Check space before symbol
		begin = header.find_first_not_of(' ', 0);
		if(begin != 0)
			return parseError(EXTRA_SPACE);
		end = myStrGetTok(header, symbol);
		//cout<<"has symbol:\""<<symbol<<"\""<<endl;
		begin = symbol.find_first_of('\t');
		if(begin == 0)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}

		// Check symbol type
		if(symbol[0] == 'c')
			break;
		if(symbol[0] != 'i' && symbol[0] != 'o')
		{
			errMsg = symbol[0];
			return parseError(ILLEGAL_SYMBOL_TYPE);
		}
		++colNo; // 1 -> + 1 = 2

		// Check space between i/o and num
		if(end == 1)
			return parseError(EXTRA_SPACE);
		if(symbol.size() == 1)
		{
			errMsg = "symbol index";
			return parseError(MISSING_NUM);
		}
		if(begin == 1)
		{
			errInt = int('\t');
			return parseError(ILLEGAL_WSPACE);
		}

		// Check num is valid or not
		token = symbol.substr(1, begin - 1);
		//cout << "token(id):\""<<token<<"\""<<endl;
		int id;
		if(!myStr2Int(token, id))
		{
			errMsg = "symbol index(";
			errMsg += token;
			errMsg += ")";
			return parseError(ILLEGAL_NUM);
		}
		if(begin != string::npos)
		{
			colNo += begin - 1;
			return parseError(MISSING_SPACE);
		}

		// Check reDef symbol
		if(symbol[0] == 'i')
		{
			if(id >= _PIs.size())
			{
				errInt = id;
				errMsg = "PI index";
				return parseError(NUM_TOO_BIG);
			}
			if(_PIs[id]->getSymbol() != "")
			{
				errMsg = symbol[0];
				errInt = id;
				return parseError(REDEF_SYMBOLIC_NAME);
			}
		}
		else
		{
			if(id >= _POs.size())
			{
				errInt = id;
				errMsg = "PO index";
				return parseError(NUM_TOO_BIG);
			}
			if(_POs[id]->getSymbol() != "")
			{
				errMsg = symbol[0];
				errInt = id;
				return parseError(REDEF_SYMBOLIC_NAME);
			}
		}

		colNo += token.size(); // 2 -> + 1 = 3
		// Check space after symbol
		if(end == string::npos)
		{
			errMsg = "symbolic name";
			return parseError(MISSING_IDENTIFIER);
		}
		++colNo; // 3 -> + 1 = 4

		// Check symbolic name
		begin = header.find_first_not_of(' ', end);
		if(begin == string::npos)
		{
			errMsg = "symbolic name";
			return parseError(MISSING_IDENTIFIER);
		}
		token = header.substr(end + 1);
		for(int i = 0; i < token.size(); ++i)
		{
			if(!isprint(token[i]))
			{
				errInt = int(token[i]);
				return parseError(ILLEGAL_SYMBOL_NAME);
			}
			++colNo;
		}

		if(symbol[0] == 'i')
		{
			_PIs[id]->setSymbol(token);
			//cout<<"PI"<<id<<" symbol:"<<tokent<<endl;
		}
		else if(symbol[0] == 'o')
		{
			_POs[id]->setSymbol(token);
			//cout<<"PO"<<id<<" symbol"<<token<<endl;
		}
	}while(symbol[0] != 'c');

	colNo = 1;
	// Check comment initial
	if(symbol.size())
	{
		if(symbol.size() > 1)
			return parseError(MISSING_NEWLINE);
		if(end != string::npos)
			return parseError(MISSING_NEWLINE);
	}

	// Gen connection
	// PO
	for(size_t j = 0; j < _POs.size(); ++j)
	{
		unsigned fanInId = _POs[j]->getFaninId(0);
		if(fanInId % 2 == 0)
		{
			fanInId /= 2;
			_POs[j]->addFaninPin(pin(_gates[fanInId], 0));
			if(_gates[fanInId])
				_gates[fanInId]->addFanoutPin(pin(_POs[j], 0));
			else
				_float.push_back(_POs[j]->getId());
		}
		else
		{
			fanInId /= 2;
			_POs[j]->addFaninPin(pin(_gates[fanInId], 1));
			if(_gates[fanInId])
				_gates[fanInId]->addFanoutPin(pin(_POs[j], 1));
			else
				_float.push_back(_POs[j]->getId());
		}
		/*
		cout<<"PO"<<j<<"'s fanInId:";
		if(_POs[j]->getFaninPin(0).isInv())
			cout<<"!";
		cout<<fanInId<<endl;
		*/
	}
	//AIG
	for(size_t j = 0; j < _AIGs.size(); ++j)
	{
		unsigned fanInId1 = _AIGs[j]->getFaninId(0);
		unsigned fanInId2 = _AIGs[j]->getFaninId(1);
		if(fanInId1 % 2 == 0 && fanInId2 % 2 == 0)
		{
			
			fanInId1 /= 2;
			fanInId2 /= 2;
			_AIGs[j]->addFaninPin(pin(_gates[fanInId1], 0));
			_AIGs[j]->addFaninPin(pin(_gates[fanInId2], 0));
			if(_gates[fanInId1] && _gates[fanInId2])
			{
				_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 0));
				_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 0));
			}
			else
			{
				_float.push_back(_AIGs[j]->getId());
				if(_gates[fanInId1])
					_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 0));
				if(_gates[fanInId2])
					_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 0));
			}
		}
		else if(fanInId1 % 2 ==0 && fanInId2 % 2 == 1)
		{
			fanInId1 /= 2;
			fanInId2 /= 2;
			_AIGs[j]->addFaninPin(pin(_gates[fanInId1], 0));
			_AIGs[j]->addFaninPin(pin(_gates[fanInId2], 1));
			if(_gates[fanInId1] && _gates[fanInId2])
			{
				_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 0));
				_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 1));
			}
			else
			{
				_float.push_back(_AIGs[j]->getId());
				if(_gates[fanInId1])
					_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 0));
				if(_gates[fanInId2])
					_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 1));
			}
		}
		else if(fanInId1 % 2 ==1 && fanInId2 % 2 == 0)
		{
			fanInId1 /= 2;
			fanInId2 /= 2;
			_AIGs[j]->addFaninPin(pin(_gates[fanInId1], 1));
			_AIGs[j]->addFaninPin(pin(_gates[fanInId2], 0));
			if(_gates[fanInId1] && _gates[fanInId2])
			{
				_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 1));
				_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 0));
			}
			else
			{
				_float.push_back(_AIGs[j]->getId());
				if(_gates[fanInId1])
					_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 1));
				if(_gates[fanInId2])
					_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 0));
			}
		}
		else if(fanInId1 % 2 ==1 && fanInId2 % 2 == 1)
		{
			fanInId1 /= 2;
			fanInId2 /= 2;
			_AIGs[j]->addFaninPin(pin(_gates[fanInId1], 1));
			_AIGs[j]->addFaninPin(pin(_gates[fanInId2], 1));
			if(_gates[fanInId1] && _gates[fanInId2])
			{
				_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 1));
				_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 1));
			}
			else
			{
				_float.push_back(_AIGs[j]->getId());
				if(_gates[fanInId1])
					_gates[fanInId1]->addFanoutPin(pin(_AIGs[j], 1));
				if(_gates[fanInId2])
					_gates[fanInId2]->addFanoutPin(pin(_AIGs[j], 1));
			}
		}
		/*
		cout<<"AIG"<<_AIGs[j]->getId()<<"'s fanInId1:";
		if(_AIGs[j]->getFaninPin(0).isInv())
			cout<<"!";
		cout<<fanInId1<<endl;
		cout<<"AIG"<<_AIGs[j]->getId()<<"'s fanInId2:";
		if(_AIGs[j]->getFaninPin(1).isInv())
			cout<<"!";
		cout<<fanInId2<<endl;
		*/
	}
	sort(_float.begin(), _float.end());
	// Check unused
	// AIG
	for(size_t j = 0; j < _AIGs.size(); ++j)
	{
		if(_AIGs[j]->getFanoutPin(0).gate() == 0)
			_unused.push_back(_AIGs[j]->getId());
	}
	// PI
	for(size_t j = 0; j < _PIs.size(); ++j)
	{
		if(_PIs[j]->getFanoutPin(0).gate() == 0)
			_unused.push_back(_PIs[j]->getId());
	}
	sort(_unused.begin(), _unused.end());

	return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
	cout << endl;
	cout << "Circuit Statistics" << endl;
	cout << "==================" << endl;
	cout << "  PI   " << setw(9) << right << _PIs.size() << endl;
	cout << "  PO   " << setw(9) << right << _POs.size() << endl;
	cout << "  AIG  " << setw(9) << right << _AIGs.size() << endl;
	cout << "------------------" << endl;
	cout << "  Total" << setw(9) << _PIs.size() + _POs.size() + _AIGs.size() << right << endl;
}

void
CirMgr::printNetlist() const
{
	cout << endl;
	unsigned cnt = 0;
	CirGate::setGlobalRef();
	for(size_t i = 0; i < _POs.size(); ++i)
	{
		dfs(_POs[i], cnt, 'n', cout);
	}
}

void
CirMgr::printPIs() const
{
	cout << "PIs of the circuit:";
	for(size_t i = 0; i < _PIs.size(); ++i)
		cout<<" "<< _PIs[i]->getId();
	cout << endl;
}

void
CirMgr::printPOs() const
{
	cout << "POs of the circuit:";
	for(size_t i = 0; i < _POs.size(); ++i)
		cout<<" "<< _POs[i]->getId();
	cout << endl;
}

void
CirMgr::printFloatGates() const
{
	if(_float.size())
	{
		cout << "Gates with floating fanin(s):";
		for(size_t i = 0; i < _float.size(); ++i)
			cout << " " << _float[i];
		cout << endl;
	}
	if(_unused.size())
	{
		cout << "Gates defined but not used  :";
		for(size_t i = 0; i < _unused.size(); ++i)
			cout << " " << _unused[i];
		cout << endl;
	}
}

void
CirMgr::writeAag(ostream& outfile) const
{
	unsigned cnt = 0;
	CirGate::setGlobalRef();
	for(size_t i = 0; i < _POs.size(); ++i)
	{
		dfs(_POs[i], cnt, 'c', outfile);
	}
	outfile << "aag " << _maxId - _POs.size() - 1 << " " << _PIs.size() << " 0 " << _POs.size() << " " << cnt << endl;
	for(size_t i = 0; i < _PIs.size(); ++i)
		outfile << _PIs[i]->getId() * 2 << endl;
	for(size_t i = 0; i < _POs.size(); ++i)
		outfile << _POs[i]->getFaninId(0) << endl;
	CirGate::setGlobalRef();
	for(size_t i = 0; i < _POs.size(); ++i)
	{
		dfs(_POs[i], cnt, 'w', outfile);
	}
	for(size_t i = 0; i < _PIs.size(); ++i)
		if(_PIs[i]->getSymbol() != "")
			outfile << "i" << i << " " << _PIs[i]->getSymbol() << endl;
	for(size_t i = 0; i < _POs.size(); ++i)
		if(_POs[i]->getSymbol() != "")
			outfile << "o" << i << " " << _POs[i]->getSymbol() << endl;
	outfile << "c" << endl;
	outfile << "finally it comes to an end (TAT)" << endl;
}


void
CirMgr::dfs(const CirGate* g, unsigned &cnt, const char &usage, ostream& os) const
{
	// Print netList
	for(size_t i = 0; i < g->getFaninIdSize(); ++i)
	{
		if(g->getFaninPin(i).gate())
		{
			if(!g->getFaninPin(i).gate()->isGlobalRef())
			{
				g->getFaninPin(i).gate()->setToGlobalRef();
				dfs(g->getFaninPin(i).gate(), cnt, usage, os);
			}
		}
	}
	if(usage == 'n')
	{
		os << "[" << cnt << "] " << setw(4) << left << g->getTypeStr() << g->getId();
		for(size_t i = 0; i < g->getFaninIdSize(); ++i)
		{
			os << " ";
			if(!g->getFaninPin(i).gate())
				os << "*";
			if(g->getFaninId(i) % 2 != 0)
				os << "!";
			os << g->getFaninId(i) / 2 ;
		}
		if(g->getSymbol() != "")
			os << " (" << g->getSymbol() << ")";
		os << endl;
		++cnt;
	}
	else if(usage == 'c')
	{
		if(g->getTypeStr() != "PI" && g->getTypeStr() != "PO" && g->getTypeStr() != "CONST")
			++cnt;
	}
	else if(usage == 'w')
	{
		if(g->getTypeStr() == "AIG")
		{
			os << g->getId() * 2 << " " << g->getFaninId(0) << " " << g->getFaninId(1) << endl;
		}
	}
}