// -------------------------------------------------------------------------
// -----                  CbmSttMatchTracks header file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -----          Adapted for STT 13/03/08  by A. Zinchenko            -----
// -------------------------------------------------------------------------


/** CbmSttMatchTracks.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for matching a reconstructed CbmSttTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class CbmSttTrackMatch for
 ** each CbmSttTrack.
 **/


#ifndef CBMSTTMATCHTRACKS_H
#define CBMSTTMATCHTRACKS_H 1

#include "FairTask.h"

#include <map>

class TClonesArray;

class CbmSttMatchTracks : public FairTask
{

 public:

  /** Default constructor **/
  CbmSttMatchTracks();

  /** Constructor with verbosity level **/
  CbmSttMatchTracks(Int_t verbose);

  /** Constructor with name, title and verbosity
   **
   *@param name     Name of taks
   *@param title    Title of task   (default FairTask)
   *@param verbose  Verbosity level (default 1)
   **/
  CbmSttMatchTracks(const char* name, const char* title = "FairTask",
		    Int_t verbose = 1);

  /** Destructor **/
  virtual ~CbmSttMatchTracks();

  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Finishing */
  virtual void Finish();

 private:

  TClonesArray* fTracks;       // Array of CbmSttTracks
  TClonesArray* fPoints;       // Array of FairMCPoints
  TClonesArray* fHits;         // Array of CbmSttHits
  TClonesArray* fMatches;      // Array of CbmSttTrackMatch

  /** Map from MCTrackID to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  /** Verbosity level **/
  Int_t fVerbose;
  
  CbmSttMatchTracks(const CbmSttMatchTracks&);
  CbmSttMatchTracks& operator=(const CbmSttMatchTracks&);

  ClassDef(CbmSttMatchTracks,1);

};

#endif
