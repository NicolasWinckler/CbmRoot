/** CbmLitTool.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2007
 *
 * Base class for tools. Each tool has to have a unique name and
 * to implement 2 functions Initialize() and Finalize().
 **/

#ifndef CBMLITTOOL_H_
#define CBMLITTOOL_H_

#include "base/CbmLitEnums.h"

#include <string>

class CbmLitTool
{
public:
	/* Constructor */
	CbmLitTool():fName("") {};

	/* Constructor with name
	 *@param name Name of the tool.
	 */
	CbmLitTool(const std::string& name):fName(name){};

	/* Destructor */
	virtual ~CbmLitTool(){};

	/* Returns name of the tool.
	 * @return Name of the tool.
	 */
	const std::string& GetName() const { return fName;}

	/* Sets name of the tool. */
	void SetName(const std::string& name) { fName = name; }

	/* Initialization of the tool. It allows to complete
	 * the initialization that could not be done with a constructor.
	 * @return Status.
	 */
	virtual LitStatus Initialize() = 0;

	/* Finalization of the tool. This method is called before the destructor is called.
	 * @return Status.
	 */
	virtual LitStatus Finalize() = 0;

private:
	// Name of the concrete tool
	std::string fName;
};

#endif /* CBMLITTOOL_H_ */
