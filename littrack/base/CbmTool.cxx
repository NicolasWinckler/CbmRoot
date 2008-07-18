// -------------------------------------------------------------------------
// -----                  CbmTool source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


#include "CbmTool.h"

#include <iostream>

// Constructors
CbmTool::CbmTool():
   fName("")
{

}

CbmTool::CbmTool(const std::string& name):
   fName(name)
{
 
}

// destructor.
CbmTool::~CbmTool()
{

}

ClassImp(CbmTool)

