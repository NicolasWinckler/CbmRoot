/** @file CbmTofTrack.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.10.2013
 **/

#ifndef CBMTOFTRACK_H
#define CBMTOFTRACK_H 1


#include "TObject.h"
#include "TMath.h"
#include "FairTrackParam.h"
#include "tof/CbmTofHit.h"


/** @class CbmTofTrack
 ** @brief Provides information on attaching a TofHit to a GlobalTrack
 ** @author V.Friese <v.friese@gsi.de>
 **/
class CbmTofTrack : public TObject {

  public:

	/**   Constructor   **/
	CbmTofTrack();


	/** Constructor with parameters
	 ** @param trackIndex   Array index of global track
	 ** @param hitIndex     Array index of TOF hit
	 ** @param trackLength  Track length from primary vertex to TOF
	 ** @param trackPar     Parameters of track at TOF
	 ** @param pidHypo      PID hypothesis for track extrapolation
	 **/
	CbmTofTrack(Int_t trackIndex, Int_t hitIndex, Double_t trackLength,
			    const FairTrackParam& trackPar, Int_t pidHypo) :
			    	fGlbTrack(trackIndex),
			    	fTofHit(hitIndex),
			    	fTrackPar(trackPar),
			    	fTrackLength(trackLength),
			    	fPidHypo(pidHypo) {
	}


	/**   Destructor   **/
	virtual ~CbmTofTrack();

	/**  PID hypothesis for track extrapolation to TOF **/
	Int_t    GetPidHypo() const { return fPidHypo; }

	/**  Index of TOF hit **/
	Int_t    GetTofHitIndex() const { return fTofHit; }

	/**  Error of track x coordinate at TOF  **/
	Double_t GetTrackDx() const
		{ return TMath::Sqrt( fTrackPar.GetCovariance(1,1) ); }

	/**  Error of track x coordinate at TOF  **/
    Double_t GetTrackDy() const
    	{ return TMath::Sqrt( fTrackPar.GetCovariance(2,2) ); }

	/**  Index of global track **/
	Int_t    GetTrackIndex()  const { return fGlbTrack; }

	/**  Track length from primary vertex to TOF **/
	Double_t GetTrackLength() const { return fTrackLength; }

 	/**  Track parameters at TOF **/
	const FairTrackParam* GetTrackParameter() const { return &fTrackPar; }

	/**  Track x position at TOF  **/
    Double_t GetTrackX() const { return fTrackPar.GetX(); }

	/**  Track y position at TOF  **/
    Double_t GetTrackY() const { return fTrackPar.GetY(); }



  private:

    Int_t          fGlbTrack;     ///< Index of global track
	Int_t          fTofHit;       ///< Index of TofHit
	Double32_t     fTrackLength;  ///< Track length from primary vertex to TOF [cm]
	FairTrackParam fTrackPar;     ///< Track parameters at z of TofHit
	Int_t          fPidHypo;      ///< PID hypothesis used for track extrapolation


	ClassDef(CbmTofTrack, 1);

};

#endif /* CBMTOFTRACK_H */
