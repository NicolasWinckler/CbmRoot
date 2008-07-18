// -------------------------------------------------------------------------
// -----                   CbmMuchFindTracks header file               -----
// -----                  Created 01/10/07  by A. Lebedev              -----
// -----                                                               -----
// -------------------------------------------------------------------------


/** CbmMuchFindTracks
 *@author A.Lebedev <Andrey.Lebedev@gsi.de>
 **
 ** Task class for track finding in the MUCH.
 ** Input: TClonesArray of CbmMuchHit
 ** Output: TClonesArray of CbmMuchTrack
 **
 ** Uses as track finding algorithm classes derived from CbmMuchTrackFinder.
 **/


#ifndef CbmMuchFindTracks_H
#define CbmMuchFindTracks_H 1


#include "CbmTask.h"

class CbmMuchTrackFinder;
class TClonesArray;

class CbmMuchFindTracks : public CbmTask
{

 public:

  /** Default constructor **/
  CbmMuchFindTracks();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   *@param finder Pointer to STS track finder concrete class
   **/
  CbmMuchFindTracks( const char* name, 
                     const char* title = "CbmTask", 
                     CbmMuchTrackFinder* finder = NULL);


  /** Destructor **/
  virtual ~CbmMuchFindTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end **/
  virtual void Finish();


  /** SetParContainers **/
  virtual void SetParContainers();


  /** Accessors **/
  CbmMuchTrackFinder* GetFinder() { return fFinder; };
  Int_t GetNofTracks()           { return fNofTracks; };


  /** Set concrete track finder **/
  void UseFinder(CbmMuchTrackFinder* finder) { fFinder = finder; };



 private:

  CbmMuchTrackFinder* fFinder;    // Pointer to TrackFinder concrete class
  TClonesArray* fMuchHitArray;    // Input array of TRD hits
  TClonesArray* fTrackArray;     // Output array of CbmTrdTracks 
  Int_t fNofTracks;              // Number of tracks created


  ClassDef(CbmMuchFindTracks,1);

};

#endif
