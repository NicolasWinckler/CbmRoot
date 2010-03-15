#ifndef ICALDETCONTFACT_H
#define ICALDETCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class IcalDetContFact: public FairContFact {
private:
	void setAllContainers();
public:
	IcalDetContFact();
	~IcalDetContFact() {
	}
	FairParSet* createContainer(FairContainer*);
ClassDef( IcalDetContFact,0);
};

#endif  /* !ICALDETCONTFACT_H */
