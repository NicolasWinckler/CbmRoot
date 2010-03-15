/** CbmTrdHitsQa.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2010
 * @version 1.0
 **
 ** The task check TRD hit quality.
 **/

#ifndef CBMTRDHITSQA_H_
#define CBMTRDHITSQA_H_

#include "FairTask.h"

#include <vector>

class TClonesArray;
class TH1F;
class TH2F;
class TList;

class CbmTrdHitsQa : public FairTask
{
public:
	/* Constructor */
	CbmTrdHitsQa();

	/* Destructor */
	virtual ~CbmTrdHitsQa();

	/* Inherited from FairTask */
	virtual InitStatus Init();

	/* Inherited from FairTask */
	virtual void Exec(
			Option_t* opt);

	/* Inherited from FairTask */
	virtual void Finish();

	/* Inherited from FairTask */
	virtual void SetParContainers();

	/* Sets output directory for images
	 * @param dir Directory name
	 */
	void SetOutputDir(const std::string& dir) {fOutputDir = dir;}

private:
    /**
     * Reads necessary data branches from the input data files.
     */
	void ReadDataBranches();

    /**
     * Creates the histograms.
     */
	void CreateHistos();

	/**
	 * Fills histograms for hits
	 */
	void ProcessHits();

	/**
	 * Write histograms to the output file
	 */
	void WriteToFile();

	/*
	 * Draws histograms
	 */
	void Draw();

	TClonesArray* fTrdHits; // CbmTrdHit array
	TClonesArray* fTrdPoints; // CbmTrdPoint array

	Int_t fNofPlanes; // Number of planes in TRD

	// Histograms
	std::vector<TH1F*> fhX; // X coordinate distribution
	std::vector<TH1F*> fhY; // Y coordinate distribution
	std::vector<TH1F*> fhDx; // X error distribution
	std::vector<TH1F*> fhDy; // Y error distribution
	std::vector<TH2F*> fhXY; // XY coordinates distribution

	std::vector<TH1F*> fhResX; // residual for X coordinate
	std::vector<TH1F*> fhResY; // residual for Y coordinate
	std::vector<TH1F*> fhPullX; // pull for X coordinate
	std::vector<TH1F*> fhPullY; // pull for Y coordinate

	TList* fHistoList; // List of histograms

	std::string fOutputDir; // Output directory for images

	Int_t fEventNo;

	ClassDef(CbmTrdHitsQa, 1);
};

#endif /* CBMTRDHITSQA_H_ */
