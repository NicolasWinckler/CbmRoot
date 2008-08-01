#ifndef CBMLITROBUSTANA_H_
#define CBMLITROBUSTANA_H_

#include "TObject.h"

class CbmLitRobustAna
{
public:	
	virtual ~CbmLitRobustAna();
	
	CbmLitRobustAna* Instance();
	
	
protected:
	CbmLitRobustAna();
	
private:
	void CreateHistos();
	
	static CbmLitRobustAna* fInstance;
	
	ClassDef(CbmLitRobustAna, 1)
};

#endif /*CBMLITROBUSTANA_H_*/
