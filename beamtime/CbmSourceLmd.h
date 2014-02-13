// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                             CbmSourceLmd                          -----
// -----                    Created 13.12.2013 by V. Friese                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef CBMSOURCELMD_H
#define CBMSOURCELMD_H

#include "FairSource.h"
#include "CbmDetectorList.h"

#include "roc/Message.h"
#include "roc/Iterator.h"

#include <map>
#include "TString.h"

class CbmDigi;
class CbmDaqMap;
class CbmTbDaqBuffer;
class CbmTbEvent;
class TClonesArray;

class CbmSourceLmd : public FairSource
{
  public:
    CbmSourceLmd();
    CbmSourceLmd(const char *);
    CbmSourceLmd(const CbmSourceLmd& source);
    virtual ~CbmSourceLmd();

    virtual Bool_t Init();
    virtual Int_t ReadEvent();
    virtual void Close();

    virtual void Reset();

    void SetReadInTimeStep(ULong_t step) { fReadInTimeStep = step;  }
    void SetPersistence(Bool_t persistence = kTRUE) { fPersistence = persistence; }

    void AddFile(const char * fname) { fInputFileName = fname;}

 private:

    // --- Settings
    TString  fInputFileName;    ///< Name of lmd input file
    ULong_t  fReadInTimeStep;   ///< Time step in which data from input are read
    Bool_t   fPersistence;      ///< Flag for file storage of output arrays
    Double_t fEventTimeWindow;  ///< Time window for association of digi to event

    // --- Auxiliary classes
    CbmTbDaqBuffer* fBuffer;        ///< Digi buffer instance
    CbmDaqMap* fDaqMap;             ///< Mapping from electronics to detectors
    roc::Iterator* fRocIter;        ///< ROC iterator


    // --- Status
    ULong_t fTimeStart;
    ULong_t fTimeStop;
    ULong_t fCurrentEpochTime;     /** Time stamp of current epoch **/
    UInt_t fStartEpoch;             ///< Number of first epoch
    std::map<Int_t, uint32_t> fCurrentEpoch;  /** Current epoch marker for each ROC **/
    ULong_t fCurrentBufferTime;     ///< Time limit of current buffer
    roc::Message*  fCurrentMessage;      //! Current ROC message
    ULong_t  fTimeBufferFill;   ///< Time up to which the buffer was filled
    Double_t fTimeBufferOut;    ///< Time up to which data from the buffer can be retrieved

    // --- Event status
    CbmTbEvent* fCurrentEvent;    ///< Pointer to current event
    CbmDigi* fCurrentDigi;        ///< Pointer to current digi read from buffer


    // --- Counters
    Int_t fNofMessages;          ///< Number of processed ROC messages
    Int_t fNofEpochs;            ///< Number of processed ROC epochs messages
    Int_t fNofEvents;            ///< Number of built events
    Int_t fNofMessType[8];       ///< Number of message per message type
    Int_t fNofMessRoc[20][5];    ///< Number of messages per ROC and NXYTER
    Int_t fNofHitMsg[kNOFDETS];  ///< Number of hit messages per detector system
    Int_t fNofDigis[kNOFDETS];   ///< Number of created digis per detector system
    Int_t fNofAux;               ///< Number of AUX messages


    // --- Output arrays
    TClonesArray* fEvents;          ///< Output array of CbmTbEvent
    TClonesArray* fHodoDigis;       ///< Output array of CbmHodoDigi
    TClonesArray* fStsDigis;        ///< Output array of CbmStsDigiLight
    TClonesArray* fMuchDigis;       ///< Output array of CbmMuchDigi
    TClonesArray* fAuxDigis;        ///< Output array of CbmAuxDigi





    void ProcessEpochMarker();
    void ProcessHodoMessage();
    void ProcessMuchMessage();
    void ProcessStsMessage();

    /** Fill the buffer from the input
     ** @param time  Maximal time of input data
     ** @return kFALSE if end of input is reachedÉ
     **/
    Bool_t FillBuffer(ULong_t time);


    /** Get next digi from the buffer.
     ** If necessary, refill the buffer.
     ** @return Pointer to next digi object. NULL if no more available.
     **/
    CbmDigi* GetNextData();



    ClassDef(CbmSourceLmd, 0)
};


#endif
