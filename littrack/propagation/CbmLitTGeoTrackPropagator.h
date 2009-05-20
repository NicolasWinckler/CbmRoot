/** CbmLitTGeoTrackPropagator.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2007
 * @version 1.0
 **
 ** LIT track propagator, based on the geometry and navigation from TGeoManager.
 **/

#ifndef CBMLITTGEOTRACKPROPAGATOR_H_
#define CBMLITTGEOTRACKPROPAGATOR_H_

#include "CbmLitTrackPropagator.h"
#include "CbmLitPtrTypes.h"

#include <vector>

#include "TMatrixD.h"

class CbmLitTGeoTrackPropagator: public CbmLitTrackPropagator
{
public:
	/**
	 * Standard constructor.
	 * @param extrapolator Pointer to the LIT track extrapolator class to be used during the propagation.
	 */
	CbmLitTGeoTrackPropagator(
			TrackExtrapolatorPtr extrapolator);

	/** Destructor */
	virtual ~CbmLitTGeoTrackPropagator();

	/**
     * Derived from CbmLitTool. Performs initialization of the tool.
     */
	virtual LitStatus Initialize();

	/**
     * Derived from CbmLitTool. Executed before destructor.
     */
	virtual LitStatus Finalize();

	/**
     * Derived from CbmLitTrackPropagator. Implements the track propagation.
     * @param parIn Input track parameter
     * @param parOut Output track parameter
     * @param zOut z position to propagate to
     * @param pdg PDG code of the particle
     */
	virtual LitStatus Propagate(
	   const CbmLitTrackParam *parIn,
	   CbmLitTrackParam *parOut,
	   Double_t zOut,
	   Int_t pdg);

	/**
     * Derived from CbmLitTrackPropagator. Implements the track propagation.
     * @param parIn Input/output track parameter
     * @param zOut z position to propagate to
     * @param pdg PDG code of the particle
     */
	virtual LitStatus Propagate(
	   CbmLitTrackParam *par,
	   Double_t zOut,
	   Int_t pdg);

	/**
     * Derived from CbmLitTrackPropagator. Getter for the transport matrix.
     * @param F Transport matrix as an std::vector<Double_t>.
     */
	virtual void TransportMatrix(
	   std::vector<Double_t>& F);

	/**
     * Derived from CbmLitTrackPropagator. Getter for the transport matrix.
     * @param F Transport matrix as TMatrixD.
     */
	virtual void TransportMatrix(
	   TMatrixD& F);

	/**
     * If true, than transport matrix will be calculated during the propagation.
     */
	Bool_t IsCalcTransportMatrix() const {return fCalcTransportMatrix;}

	/**
     * Sets whether calculate transport matrix or not.
     * @param calcTransportMatrix If true, than transport matrix will be calculated during the propagation.
     */
	void IsCalcTransportMatrix(Bool_t calcTransportMatrix) {fCalcTransportMatrix = calcTransportMatrix;}

protected:
	/**
     * Updates the transport matrix F=F*newF.
     * @param F Input/output Current transport matrix.
     * @param newF Transport matrix to be included in the current transport matrix.
     */
	void UpdateF(
		TMatrixD& F,
		const TMatrixD& newF);

	/**
     * Check if the track parameters are correct by checking their maximum acceptable values.
     * @param par Track parameter to be checked.
     */
	Bool_t IsParCorrect(
		const CbmLitTrackParam* par);

private:
	TrackExtrapolatorPtr fExtrapolator; // Track extrapolator tool
	GeoNavigatorPtr fNavigator; // Track navigator tool
	MaterialEffectsPtr fMaterial; // Material effects tool

	TMatrixD fFm; // Transport matrix

	Bool_t fDownstream; // if true than downstream propagation
	Int_t fPDG; // PDG of the particle

	Bool_t fCalcTransportMatrix; // if true than transport matrix is calculated

	ClassDef(CbmLitTGeoTrackPropagator,1);
};

#endif //CBMLITTGEOTRACKPROPAGATOR_H_

