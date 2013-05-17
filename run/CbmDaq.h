/** @file CbmDaq.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 **/


#ifndef CBMDAQ_H
#define CBMDAQ_H 1


#include "FairTask.h"


class CbmDaqBuffer;
class CbmDigi;
class CbmTimeSlice;


/** @class CbmDaq
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 ** @brief CBM task class for filling digis from the CbmDaqBuffer into
 ** timeslice containers
 **/
class CbmDaq : public FairTask
{
  public:

    /**   Constructor
     ** @param timeSliceSize  Duration of time slices [ns]
     **/
    CbmDaq(Double_t timeSliceSize = 1000.);


    /**   Destructor   **/
    ~CbmDaq();


    /**   Task execution   **/
    virtual void Exec(Option_t* opt);


    /**   Task initialisation   **/
    virtual InitStatus Init();


  private:

    /** Start time of current time slice [ns] **/
    Double_t fCurrentStartTime;

    /** Duration of time slice [ns] **/
    Double_t fDuration;

    /** Flag whether empty timeslices should be filled to the tree **/
    // TODO: Implement functionality
    Bool_t fStoreEmptySlices;

    /** Pointer to current time slice **/
    CbmTimeSlice* fTimeSlice;

    /** Pointer to CbmDaqBuffer instance  **/
    CbmDaqBuffer* fBuffer;
    

    /** Close the current time slice
     ** The current slice is filled to the tree. It is then reset
     ** to the next time slice interval.
     */
    void CloseTimeSlice();


    /** Fill data from the buffer into the current time slice **/
    void FillTimeSlice();


    /** At end of run: Process the remaining data in the CbmDaqBuffer  **/
    virtual void Finish();

    CbmDaq(const CbmDaq&);
    CbmDaq& operator=(const CbmDaq&);
    
    ClassDef (CbmDaq,1);

};

#endif /* CBMDAQ_H */
