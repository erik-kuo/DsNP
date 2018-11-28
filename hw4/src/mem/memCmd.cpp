/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
  //check options
  vector<string> options;
  if(!CmdExec::lexOptions(option, options))
   return CMD_EXEC_ERROR;

  if(options.empty())
   return CmdExec::errorOption(CMD_OPT_MISSING, "");

  size_t numObjects = 0, arraySize = 0;
  int num = 0, arr = 0;
  for (size_t i = 0, n = options.size(); i < n; ++i)
  {
    if(myStrNCmp("-Array", options[i], 2) == 0)
    {
      if(arraySize)
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      i++;
      if(i >= n)
        return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
      if(!myStr2Int(options[i], arr))
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      if(arr <= 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      arraySize = (size_t) arr;
    }
    else
    {
      if(numObjects)
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      if(!myStr2Int(options[i], num))
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      if(num <= 0)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
      numObjects = (size_t) num;
    }
  }
  if(numObjects == 0)
    return CmdExec::errorOption(CMD_OPT_MISSING, "");
  //check options done

  //exec cmd
  try
  {
    if(arraySize == 0)
      mtest.newObjs(numObjects);
    else
      mtest.newArrs(numObjects, arraySize);
  } catch(bad_alloc) {return CMD_EXEC_ERROR;}
   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
  // TODO
  //check options 
  vector<string> options;
  if(!CmdExec::lexOptions(option, options))
    return CMD_EXEC_ERROR;

  if(options.empty())
   return CmdExec::errorOption(CMD_OPT_MISSING, "");

  size_t objId = -1, numRandId = 0;
  bool arr = false;
  int id = -1, num = 0;
  string mayBeIllegalOpt;
  for(size_t i = 0, n = options.size(); i < n; ++i)
  {
    if(myStrNCmp("-Array", options[i], 2) == 0)
    {
      if(arr)
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
      arr = true;
    }
    else
    {
      if(myStrNCmp("-Index", options[i], 2) == 0)
      {
        if(objId != -1)
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
        if(numRandId)
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
        ++i;
        if(i >= n)
          return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
        if(!myStr2Int(options[i], id))
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
        if(id < 0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
        objId = (size_t)id;
        mayBeIllegalOpt = options[i];
      }
      else if(myStrNCmp("-Random", options[i], 2) == 0)
      {
        if(numRandId)
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
        if(objId != -1)
          return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
        mayBeIllegalOpt = options[i];
        ++i;
        if(i >= n)
          return CmdExec::errorOption(CMD_OPT_MISSING, options[i-1]);
        if(!myStr2Int(options[i], num))
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
        if(num <= 0)
          return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
        numRandId = (size_t)num;
      }
      else
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
    }
  }

  if(objId != -1)
  {
    if(arr)
    {
      if(mtest.getArrListSize() <= objId)
      {
        cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!"<<endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, mayBeIllegalOpt);
      }
      mtest.deleteArr(objId);
    }
    else
    {
      if(mtest.getObjListSize() <= objId)
      {
        cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!"<<endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, mayBeIllegalOpt);
      }
      mtest.deleteObj(objId);
    }
  }
  else if(numRandId)
  {
    if(arr)
    {
      if(mtest.getArrListSize() == 0)
      {
        cerr << "Size of array list is 0!!"<<endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, mayBeIllegalOpt);
      }
      for(size_t i = 0; i < numRandId; ++i)
        mtest.deleteArr(rnGen(mtest.getArrListSize()));
    }
    else
    {
      if(mtest.getObjListSize() == 0)
      {
        cerr << "Size of object list is 0!!"<<endl;
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, mayBeIllegalOpt);
      }
      for(size_t i = 0; i < numRandId; ++i)
        mtest.deleteObj(rnGen(mtest.getObjListSize()));
    }
  }
  else
    return CmdExec::errorOption(CMD_OPT_MISSING, "");

  return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


