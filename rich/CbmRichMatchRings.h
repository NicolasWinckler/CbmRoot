/******************************************************************************
*  $Id: CbmRichMatchRings.h,v 1.2 2006/02/22 09:15:46 hoehne Exp $
*
*  Class  : CbmRichMatchRings
*  Description: Task class for matching a reconstructed CbmRichRings with a simulated
*               CbmMCTrack. The matching criterion is a maximal number of common
*               hits/points. The task fills the data class CbmRichRingMatch for
*               each CbmRichRing.
*
*  Author : Supriya Das (following CbmStsMatchTracks)
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichMatchRings.h,v $
*  Revision 1.2  2006/02/22 09:15:46  hoehne
*  store also number of MC hits for each ring (Simeon Lebedev)
*
*  Revision 1.1  2006/01/23 11:42:44  hoehne
*  initial version: task to match reconstructed to MC rings
*
*
*******************************************************************************/

#ifndef CBM_RICH_MATCH_RINGS_H
#define CBM_RICH_MATCH_RINGS_H 1

#include "CbmTask.h"

#include <map>

class TClonesArray;



class CbmRichMatchRings : public CbmTask
{

 public:

  /** Default constructor **/
  CbmRichMatchRings();


  /** Constructor with verbosity level **/
  CbmRichMatchRings(Int_t verbose);


  /** Constructor with name, title and verbosity
   **
   *@param name     Name of taks
   *@param title    Title of task   (default CbmTask)
   *@param verbose  Verbosity level (default 1)
   **/
  CbmRichMatchRings(const char* name, const char* title = "CbmTask",
		    Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmRichMatchRings();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();


 private:

  TClonesArray* fRings;        // Array of CbmRichRings
  TClonesArray* fPoints;       // Array of CbmMCPoints
  TClonesArray* fTracks;       // Array of CbmMCTracks
  TClonesArray* fHits;         // Array of CbmRichHits
  TClonesArray* fMatches;      // Array of CbmRichRingMatch

  /** Map from MCTrackID to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;
  std::map<Int_t, Int_t> fMatchMCMap;

  /** Verbosity level **/
  Int_t fVerbose;


  ClassDef(CbmRichMatchRings,1);

};

#endif
