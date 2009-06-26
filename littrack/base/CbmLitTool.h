/** CbmLitTool.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 **  Base class for tools. Each tool has to have a uniq name and
 **  to implement 2 functions Initialize() and Finalize().
 **/

#ifndef CBMLITTOOL_H_
#define CBMLITTOOL_H_

#include "CbmLitEnums.h"

#include <string>

class CbmLitTool
{
public:
  CbmLitTool();
  CbmLitTool(const std::string& name);
  virtual ~CbmLitTool();

  // Set and get full identifying name of the concrete CbmLitTool object.
  const std::string& GetName() const { return fName;}
  void SetName(const std::string& name) { fName = name; }

  // Initialization of the CbmLitTool. It allows to complete
  // the initialization that could not be done with a constructor.
  virtual LitStatus Initialize() = 0;

  // Finalization of the CbmLitTool. This method is called before
  // the CbmLitTool is destructed.
  virtual LitStatus Finalize() = 0;

private:
  std::string fName;
};

#endif /* CBMLITTOOL_H_ */
