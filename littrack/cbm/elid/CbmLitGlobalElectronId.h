/** CbmLitReconstructionQa.h
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#ifndef CBMLITGLOBALELECTRONID_H_
#define CBMLITGLOBALELECTRONID_H_


#include "TObject.h"

class CbmRichElectronIdAnn;
class TClonesArray;
class CbmGlobalTrack;

class CbmLitGlobalElectronId: public TObject
{
public:
	CbmLitGlobalElectronId();
	virtual ~CbmLitGlobalElectronId();

   /* Initialize TClonesArrays*/
	void Init();

   /* Identify electron in RICH detector.
    *@param globalTrack pointer to global track
    *@param momentum momentum of the track
    *@return true if track is identified as electron otherwise return false*/
	Bool_t IsRichElectron(
			const CbmGlobalTrack* globalTrack,
			Double_t momentum);

   /* Identify electron in TRD detector.
    *@param globalTrack pointer to global track
    *@param momentum momentum of the track
    *@return true if track is identified as electron otherwise return false*/
	Bool_t IsTrdElectron(
			const CbmGlobalTrack* globalTrack,
			Double_t momentum);

   /* Identify electron in TOF detector.
    *@param globalTrack pointer to global track
    *@param momentum momentum of the track
    *@return true if track is identified as electron otherwise return false*/
	Bool_t IsTofElectron(
			const CbmGlobalTrack* globalTrack,
			Double_t momentum);

private:
    Double_t fRichAnnCut;
    Bool_t fRichUseAnn;
    Double_t fRichMeanA;
    Double_t fRichMeanB;
    Double_t fRichRmsA;
    Double_t fRichRmsB;
    Double_t fRichRmsCoeff;
    Double_t fRichDistCut;
    Double_t fTrdAnnCut;

    CbmRichElectronIdAnn* fRichElIdAnn;

    TClonesArray* fRichRings;
    TClonesArray* fTrdTracks;
    TClonesArray* fTofHits;

    ClassDef(CbmLitGlobalElectronId, 1);
};

#endif /* CBMLITGLOBALELECTRONID_H_ */
