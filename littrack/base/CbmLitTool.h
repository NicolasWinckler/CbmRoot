/** CbmLitTool.h
 *@author A.Lebedev <alebedev@jinr.ru>
 *@since 2007
 **
 ** Base class for tools. Each tool has to have a unique name and
 ** to implement 2 functions Initialize() and Finalize().
 **/

#ifndef CBMLITTOOL_H_
#define CBMLITTOOL_H_

#include "CbmLitEnums.h"

#include <string>

class CbmLitTool
{
public:
	/* Constructor */
	CbmLitTool();

	/* Constructor with name
	 *@param name Name of the tool
	 */
	CbmLitTool(const std::string& name);

	/* Destructor */
	virtual ~CbmLitTool();

	/* Sets name of the concrete CbmLitTool object.*/
	const std::string& GetName() const { return fName;}

	/* Gets name of the concrete CbmLitTool object. */
	void SetName(const std::string& name) { fName = name; }

	/* Initialization of the CbmLitTool. It allows to complete
	 * the initialization that could not be done with a constructor.
	 */
	virtual LitStatus Initialize() = 0;

	/* Finalization of the CbmLitTool. This method is called before
	* the CbmLitTool is destructed.
	*/
	virtual LitStatus Finalize() = 0;

private:
	// Name of the concrete tool
	std::string fName;
};

#endif /* CBMLITTOOL_H_ */
