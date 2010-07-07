// -------------------------------------------------------------------------
// -----                  CbmTrdMatchTracks header file                -----
// -----                  Created 01/12/05  by V. Friese               -----
// -------------------------------------------------------------------------
/** CbmTrdMatchTracks.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for matching a reconstructed CbmTrdTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class CbmTrackMatch for
 ** each CbmTrdTrack.
 **
 ** Updated 18/06/10 by A.Lebedev to support digis in TRD.
 **/

#ifndef CBMTRDMATCHTRACKS_H
#define CBMTRDMATCHTRACKS_H 1

#include "FairTask.h"

class TClonesArray;

class CbmTrdMatchTracks: public FairTask {
public:
	/** Default constructor **/
	CbmTrdMatchTracks();

	/** Constructor with verbosity level **/
	CbmTrdMatchTracks(Int_t verbose);

	/** Constructor with name, title and verbosity
	 **
	 *@param name     Name of taks
	 *@param title    Title of task   (default FairTask)
	 *@param verbose  Verbosity level (default 1)
	 **/
	CbmTrdMatchTracks(const char* name, const char* title = "FairTask",
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

        CbmTrdMatchTracks& operator=(const  CbmTrdMatchTracks&);
        CbmTrdMatchTracks(const  CbmTrdMatchTracks&);        
                
	/** Execution in case of smearing hit producer **/
	virtual void ExecSmearing(Option_t* opt);

	/** Execution in case of digi hit producer **/
	virtual void ExecDigi(Option_t* opt);

	/** Execution in  case of clustering **/
        virtual void ExecCluster(Option_t* opt);

	TClonesArray* fTracks; // Array of CbmTrdTracks
	TClonesArray* fPoints; // Array of FairMCPoints
	TClonesArray* fHits; // Array of CbmTrdHits
        TClonesArray* fClusters; // Array of CbmTrdCluster
	TClonesArray* fDigiMatches; // Array of CbmTrdDigiMatch;
	TClonesArray* fMatches; // Array of CbmTrackMatch

	Int_t fNofHits; // Total number of hits
	Int_t fNofTrueHits; // Total number of true hits
	Int_t fNofWrongHits; // Total number of wrong hits
	Int_t fNofFakeHits; // Total number of fake hits

	/** Event counter **/
	Int_t fNEvents;

	/** Verbosity level **/
	Int_t fVerbose;

	/** If true than matching will be done using digis **/
	Bool_t fUseDigis;

	/** If true than matching will be done using clusters **/
        Bool_t fUseClusters;

ClassDef(CbmTrdMatchTracks,1);
};

#endif
