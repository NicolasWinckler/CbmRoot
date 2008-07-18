// -------------------------------------------------------------------------
// -----                  CbmTool header file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------


/** CbmTool.h
 *@author A.Lebedev <alebedev@jinr.ru>
 **
 **  Base class for all the tools. Each tool has to have a uniq name and
 **  to implement 2 functions Initialize() and Finalize(). 
 **/  

#include <string>

#ifndef CBMTOOL_H
#define CBMTOOL_H

#include "CbmPropertyMgr.h"
#include "CbmLitStatusCode.h"

#include "TObject.h"


class CbmTool 
//: public TObject 
{

public:

  // Constructor
  CbmTool();
  CbmTool(const std::string& name);

  // Standard destructor.
  virtual ~CbmTool();

  // Set and get full identifying name of the concrete CbmTool object.
  const std::string& GetToolName() const { return fName;};
  void SetToolName(const std::string& name) { fName = name; };
  
  // Initialization of the CbmTool. It allows to complete  
  // the initialization that could not be done with a constructor.
  virtual LitStatus Initialize() = 0;
  
  // Finalization of the CbmTool. This method is called before
  // the CbmTool is destructed.
  virtual LitStatus Finalize() = 0;
  
  CbmPropertyMgr& Properties() { return fPropertyMgr; };

private:

  std::string fName;
  
  CbmPropertyMgr fPropertyMgr;
  
  ClassDef(CbmTool, 1);
};

#endif
