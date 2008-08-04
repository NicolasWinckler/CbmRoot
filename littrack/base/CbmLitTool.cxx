// -------------------------------------------------------------------------
// -----                  CbmLitTool source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------

#include "CbmLitTool.h"

CbmLitTool::CbmLitTool():
   fName("")
{
}

CbmLitTool::CbmLitTool(const std::string& name):
   fName(name)
{
}

CbmLitTool::~CbmLitTool()
{
}

ClassImp(CbmLitTool)
