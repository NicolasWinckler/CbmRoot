/*
 * CbmDaq.h
 *
 *  Created on: 20.07.2012
 *      Author: friese
 */

#ifndef CBMDAQ_H_
#define CBMDAQ_H_

#include "FairTask.h"

class CbmDaqBuffer;
class CbmDigi;
class CbmTimeSlice;

class CbmDaq : public FairTask
{
  public:
    CbmDaq();
    ~CbmDaq();


    virtual void Exec(Option_t* opt);

    virtual InitStatus Init();


  private:

    Double_t fCurrentStartTime;

    Double_t fDuration;

    Bool_t fStoreEmptySlices;

    CbmTimeSlice* fTimeSlice;

    CbmDaqBuffer* fBuffer;
    
    ClassDef (CbmDaq,1);
};

#endif /* CBMDAQ_H_ */
