/** @file CbmStsTrackStatus.h
 **
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 4.12.2013
 **/


#ifndef CBMSTSTRACKSTATUS_H
#define CBMSTSTRACKSTATUS_H


/** @class CbmStsTrackStatus
 ** @brief Stores status of track during transport.
 ** Auxiliary for CbmSts.
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 04.12.2013
 ** @version 1.0
 **/
class CbmStsTrackStatus {

	public:

		/** Constructor  **/
		CbmStsTrackStatus() : fAddress(0), fTrackId(0),
		                      fX(0.), fY(0.), fZ(0.),
		                      fPx(0.), fPy(0.), fPz(0.),
		                      fTime(0.), fLength(0.) {		}


		/** Destructor  **/
		~CbmStsTrackStatus() { };


		/** Reset all parameters to zero **/
		void Reset() {
			fAddress = 0;
			fTrackId = 0;
			fX       = 0.;
			fY       = 0.;
			fZ       = 0.;
			fPx      = 0.;
			fPy      = 0.;
			fPz      = 0.;
			fTime    = 0.;
			fLength  = 0.;
		}


		UInt_t   fAddress;    ///< Unique address
		Int_t    fTrackId;    ///< MCTrack index
		Double_t fX;          ///< x position [cm]
		Double_t fY;          ///< x position [cm]
		Double_t fZ;          ///< x position [cm]
		Double_t fPx;         ///< Momentum x component [GeV]
		Double_t fPy;         ///< Momentum x component [GeV]
		Double_t fPz;         ///< Momentum x component [GeV]
		Double_t fTime;       ///< Time since track creation [ns]
		Double_t fLength;     ///< Length since track creation [cm]

};




#endif /* CBMSTSTRACKSTATUS_H */
