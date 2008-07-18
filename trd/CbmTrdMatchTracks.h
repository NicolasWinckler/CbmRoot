// -------------------------------------------------------------------------
// -----                  CbmTrdMatchTracks header file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmTrdMatchTracks.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for matching a reconstructed CbmTrdTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class CbmTrdTrackMatch for
 ** each CbmTrdTrack.
 **/


#ifndef CBMTRDMATCHTRACKS_H
#define CBMTRDMATCHTRACKS_H 1


#include "CbmTask.h"

#include <map>

class TClonesArray;



class CbmTrdMatchTracks : public CbmTask
{

 public:

  /** Default constructor **/
  CbmTrdMatchTracks();


  /** Constructor with verbosity level **/
  CbmTrdMatchTracks(Int_t verbose);


  /** Constructor with name, title and verbosity
   **
   *@param name     Name of taks
   *@param title    Title of task   (default CbmTask)
   *@param verbose  Verbosity level (default 1)
   **/
  CbmTrdMatchTracks(const char* name, const char* title = "CbmTask",
		    Int_t verbose = 1);


  /** Destructor **/
  virtual ~CbmTrdMatchTracks();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();


 private:

  TClonesArray* fTracks;       // Array of CbmTrdTracks
  TClonesArray* fPoints;       // Array of CbmMCPoints
  TClonesArray* fHits;         // Array of CbmTrdHits
  TClonesArray* fMatches;      // Array of CbmTrdTrackMatch

  /** Map from MCTrackID to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  /** Verbosity level **/
  Int_t fVerbose;
  



  ClassDef(CbmTrdMatchTracks,1);

};

#endif
