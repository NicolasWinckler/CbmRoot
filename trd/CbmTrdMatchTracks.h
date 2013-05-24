/**
 * \file CbmTrdMatchTracks.h
 * \author V.Friese <v.friese@gsi.de>
 * \date 01/12/05
 *
 * Task class for matching a reconstructed CbmTrdTrack with a simulated
 * CbmMCTrack. The matching criterion is a maximal number of common
 * hits/points. The task fills the data class CbmTrackMatch for
 * each CbmTrdTrack.
 *
 * Updated 18/06/10 by A.Lebedev to support digis in TRD.
 * Updated 12/04/11 by A.Lebedev to support clusters in TRD
 **/

#ifndef CBMTRDMATCHTRACKS_H
#define CBMTRDMATCHTRACKS_H 1

#include "FairTask.h"

class TClonesArray;

class CbmTrdMatchTracks: public FairTask
{
public:
	/**
	 * \brief Default constructor.
	 **/
	CbmTrdMatchTracks();

	/**
	 * \brief Destructor.
	 **/
	virtual ~CbmTrdMatchTracks();

	/**
	 * \brief Inherited from FairTask.
	 **/
	virtual InitStatus Init();

	/**
	 * \brief Inherited from FairTask.
	 **/
	virtual void Exec(
			Option_t* opt);

	/**
	 * \brief Inherited from FairTask.
	 */
	virtual void Finish();

private:
                
	/**
	 * \brief Execution in case of smearing hit producer.
	 **/
	virtual void ExecSmearing(
			Option_t* opt);

	/**
	 * \brief Execution in case of digitization and clustering.
	 **/
    virtual void ExecCluster(
    		Option_t* opt);

    /**
     * \Print statistics to output.
     */
    void PrintStatistics();

	TClonesArray* fTracks; // Array of CbmTrdTracks
	TClonesArray* fPoints; // Array of FairMCPoints
	TClonesArray* fHits; // Array of CbmTrdHits
   TClonesArray* fClusters; // Array of CbmTrdCluster
	TClonesArray* fDigiMatches; // Array of CbmTrdDigiMatch
	TClonesArray* fTrackMatches; // Array of CbmTrackMatch

	Int_t fNofHits; // Average number of hits in track
	Int_t fNofTrueHits; // Average number of true hits in track
	Int_t fNofWrongHits; // Average number of wrong hits in track
	Double_t fPercentageTrueHits; // Average percent of true hits in track
	Double_t fPercentageWrongHits; // Average percent of wrong hits in track
	Int_t fNofMCTracksPerRecoTrack; // Average number of MC tracks per one reconstructed track
	Int_t fNofTracks; // Total number of reconstructed tracks

   CbmTrdMatchTracks(const CbmTrdMatchTracks&);
   CbmTrdMatchTracks& operator=(const CbmTrdMatchTracks&);

    ClassDef(CbmTrdMatchTracks, 2);
};

#endif
