/** CbmLitFindGlobalTracksIdeal.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** CBM task class for ideal global track reconstruction.
 ** Combines local track segments to a global track based on MC information.
 ** Local ideal or real tracking must be run before.
 ** Also track matching tasks must be run before.
 ** Produces objects of type CbmGlobalTrack.
 **/

#ifndef CBMLITFINDGLOBALTRACKSIDEAL_H_
#define CBMLITFINDGLOBALTRACKSIDEAL_H_

#include "FairTask.h"

#include <map>

class TClonesArray;

class CbmLitFindGlobalTracksIdeal : public FairTask
{
public:
	/** Default constructor */
	CbmLitFindGlobalTracksIdeal();

	/** Destructor */
	virtual ~CbmLitFindGlobalTracksIdeal();

    /**
     * Derived from FairTask. Executed before starting event-by-event execution.
     */
	virtual InitStatus Init();

	/**
     * Derived from FairTask. Executed on each event.
     * @param opt Options
     */
	virtual void Exec(Option_t* opt);

private:
    /**
     * Derived from FairTask. Executed after all events are processed.
     */
	virtual void Finish();

	/**
     * Determines the CBM detector setup, based on TGeoManager stored in the input MC file.
     */
	void DetermineSetup();

    /**
     * Reads necessary data branches from the input data files.
     */
	void ReadDataBranches();

    /**
     * Fills map from <MC track index> to <reconstructed STS track index>
     */
	void FillMapSts();

    /**
     * Fills map from <MC track index> to <reconstructed TRD track index>
     */
	void FillMapTrd();

    /**
     * Fills map from <MC track index> to <reconstructed MUCH track index>
     */
	void FillMapMuch();

    /**
     * Fills map from <MC track index> to <TOF hit index>
     */
	void FillMapTof();

    /**
     * Creates output CbmGlobalTracks and writes them to the output array.
     */
	void CreateGlobalTracks();

	Bool_t fIsElectronSetup; // If "electron" setup detected than true
	Bool_t fIsSts; // If STS detected than true
	Bool_t fIsTrd; // If TRD detected than true
	Bool_t fIsMuch; // If MUCH detected than true
	Bool_t fIsTof; // If TOF detected than true

	// Pointers to data arrays
	TClonesArray* fMCTracks; // CbmMCTrack
	TClonesArray* fStsMatches; // CbmStsTrackMatch
	TClonesArray* fMuchMatches; // CbmMuchTrackMatch
	TClonesArray* fTrdMatches; // CbmTrdTrackMatch
	TClonesArray* fTofMCPoints; // CbmTofPoint
	TClonesArray* fTofHits; // CbmTofHit
	TClonesArray* fGlobalTracks; //output: CbmGlobalTrack

	// Maps for reconstructed tracks
	// <MC track index, reconstructed track index>
	std::map<Int_t, Int_t> fMcStsMap; // STS
	std::map<Int_t, Int_t> fMcTrdMap; // TRD
	std::map<Int_t, Int_t> fMcMuchMap; // MUCH
	std::map<Int_t, Int_t> fMcTofMap; //TOF

	Int_t fEventNo; //event counter

	ClassDef(CbmLitFindGlobalTracksIdeal, 1);
};

#endif /* CBMLITFINDGLOBALTRACKSIDEAL_H_ */
