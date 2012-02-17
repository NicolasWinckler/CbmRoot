

/******************************************************************************
*  $Id: CbmRichExtrapolateTracks.h,v 1.2 2006/01/30 10:59:11 hoehne Exp $
*
*  Class  : CbmRichExtrapolateTracks
*  Description: This is the header of the ExtrapolateTracks class. This
*               takes a particular Track Extrapolation Routine
*               to create Points in a z-Plane in the RICH
*               Input:  either only MC simulation or track array
*               Output: TClonesArray of FairTrackParam
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichExtrapolateTracks.h,v $
*  Revision 1.2  2006/01/30 10:59:11  hoehne
*  changes due to bug fix in filling RichPointArray (see Extrapolation methods)
*
*  Revision 1.1  2006/01/26 09:53:21  hoehne
*  initial version for track extrapolation (base class + concrete implementations + task) to z-plane in RICH
*
*
*
*******************************************************************************/

#ifndef CBM_RICH_EXTRAPOLATE_TRACKS
#define CBM_RICH_EXTRAPOLATE_TRACKS 1


#include "FairTask.h"

class CbmRichTrackExtrapolation;
class TClonesArray;

class CbmRichExtrapolateTracks : public FairTask
{

 public:

  /** Default constructor **/
  CbmRichExtrapolateTracks();


  /** Standard constructor
   *@param extrapolation      Pointer to RICH track extrapolation concrete class
   *@param Zpos               z-Position for extrapolation [cm]
   *@param verbose            Verbosity level
   **/
  CbmRichExtrapolateTracks(CbmRichTrackExtrapolation* extrapolation,
                           Double_t Zpos = 300., Int_t verbose = 1);


  /** Constructor with name and title
   *@param name            Name of class
   *@param title           Task title
   *@param extrapolation   Pointer to track extrapolation concrete class
   *@param Zpos               z-Position for extrapolation
   *@param verbose         Verbosity level
   **/
  CbmRichExtrapolateTracks(const char* name, const char* title = "FairTask",
		   CbmRichTrackExtrapolation* extrapolation = NULL,
		   Double_t Zpos = 300., Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmRichExtrapolateTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** Accessors **/
  CbmRichTrackExtrapolation* GetExtrapolation() { return fExtrapolation; };


  /** Set concrete track extrapolation **/
  void UseExtrapolation(CbmRichTrackExtrapolation* extrapolation, Double_t Zpos)
     { fExtrapolation = extrapolation;
       fZ = Zpos;
     };



 private:

  CbmRichTrackExtrapolation* fExtrapolation;    //! Pointer to TrackExtrapolation concrete class
  TClonesArray* gTrackArray;       //! Input array of global tracks
  TClonesArray* fTrackParamArray;  //! Output array of FairTrackParam
  Double_t fZ;                     //! z-Position for Extrapolation [cm]
  Int_t fVerbose;                  //! Verbosity level
  Int_t fNofTracks;

  /**
   * \brief Copy constructor.
   */
  CbmRichExtrapolateTracks(const CbmRichExtrapolateTracks&);

  /**
   * \brief Assignment operator.
   */
  CbmRichExtrapolateTracks& operator=(const CbmRichExtrapolateTracks&);

  ClassDef(CbmRichExtrapolateTracks,1);

};

#endif
