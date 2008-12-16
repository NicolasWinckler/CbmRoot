#ifndef CBMLITTOOLFACTORY_H_
#define CBMLITTOOLFACTORY_H_

class CbmLitTrackPropagator;
class CbmLitTrackUpdate;
class CbmLitTrackFitter;
class CbmLitTrackSelection;

#include "TObject.h"

#include <string>

class CbmLitToolFactory
{
	
public:
	virtual ~CbmLitToolFactory();
	   
	static CbmLitToolFactory* Instance();
	
	CbmLitTrackPropagator* CreateTrackPropagator(const std::string& name);
	CbmLitTrackUpdate* CreateTrackUpdate(const std::string& name);
	CbmLitTrackFitter* CreateTrackFitter(const std::string& name);
	CbmLitTrackSelection* CreateTrackSelection(const std::string& name);
	

protected:
	CbmLitToolFactory();

private:
	static CbmLitToolFactory* fInstance;
	
	ClassDef(CbmLitToolFactory, 1);	
};

#endif /*CBMLITTOOLFACTORY_H_*/
