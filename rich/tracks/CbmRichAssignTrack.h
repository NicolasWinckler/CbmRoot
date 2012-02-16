/******************************************************************************
*  $Id: CbmRichAssignTrack.h,v 1.1 2006/01/26 09:54:27 hoehne Exp $
*
*  Class  : CbmRichAssignTrack
*  Description: This is the header of the AssignTrack class. This
*               takes a particular Track Assignement Routine
*               to assign found rings and tracks
*               Input:  extrapolated tracks, found rings
*               Output: ring ID in CbmGlobalTrack
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichAssignTrack.h,v $
*  Revision 1.1  2006/01/26 09:54:27  hoehne
*  initial version: assignement of Rich rings and extrapolated tracks (base class, concrete implementation, Task)
*
*
*
*******************************************************************************/

#ifndef CBM_RICH_ASSIGN_TRACK
#define CBM_RICH_ASSIGN_TRACK 1


#include "FairTask.h"

class CbmRichRingTrackAssign;
class TClonesArray;

class CbmRichAssignTrack : public FairTask
{

 public:

  /** Default constructor **/
  CbmRichAssignTrack();


  /** Standard constructor
   *@param assign             Pointer to RICH ring-track ssignement concrete class
   *@param verbose            Verbosity level
   **/
  CbmRichAssignTrack(CbmRichRingTrackAssign* assign, Int_t verbose = 1);


  /** Constructor with name and title
   *@param name            Name of class
   *@param title           Task title
   *@param assign             Pointer to RICH ring-track ssignement concrete class
   *@param verbose            Verbosity level
   **/
  CbmRichAssignTrack(const char* name, const char* title = "FairTask",
		     CbmRichRingTrackAssign* assign = NULL, Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmRichAssignTrack();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** Accessors **/
  CbmRichRingTrackAssign* GetAssign() { return fAssign; };


  /** Set concrete track extrapolation **/
  void UseAssign(CbmRichRingTrackAssign* assign)
     { fAssign = assign;
     };

private:

  CbmRichRingTrackAssign* fAssign;    //! Pointer to RingTrackAssign concrete class
  TClonesArray* fTrackArray;       //! Input array of extrapolated tracks
  TClonesArray* fRingArray;       //! Output array found rings
  TClonesArray* gTrackArray;       //! Input array of global tracks
  Int_t fVerbose;                  //! Verbosity level

  /**
   * \brief Copy constructor.
   */
  CbmRichAssignTrack(const CbmRichAssignTrack&);

  /**
   * \brief Assignment operator.
   */
  CbmRichAssignTrack& operator=(const CbmRichAssignTrack&);

  ClassDef(CbmRichAssignTrack,1);

};

#endif
