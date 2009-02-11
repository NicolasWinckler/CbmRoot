#ifndef PIPE_H
#define PIPE_H

#include "FairDetector.h"
#include "FairModule.h"

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"

class CbmPipe : public FairModule {
  public:
    CbmPipe(const char * name, const char *Title="CBM Pipe");
    CbmPipe();

    virtual ~CbmPipe();
    virtual void ConstructGeometry();
   
  ClassDef(CbmPipe,1) //CBMPIPE

};

#endif //PIPE_H

