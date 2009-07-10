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
	   myf zOut,
	   int pdg);

	/**
     * Derived from CbmLitTrackPropagator. Implements the track propagation.
     * @param parIn Input/output track parameter
     * @param zOut z position to propagate to
     * @param pdg PDG code of the particle
     */
	virtual LitStatus Propagate(
	   CbmLitTrackParam *par,
	   myf zOut,
	   int pdg);

	/**
     * Derived from CbmLitTrackPropagator. Getter for the transport matrix.
     * @param F Transport matrix as an std::vector<myf>.
     */
	virtual void TransportMatrix(
	   std::vector<myf>& F);

	/**
     * If true, than transport matrix will be calculated during the propagation.
     */
	bool IsCalcTransportMatrix() const {return fCalcTransportMatrix;}

	/**
     * Sets whether calculate transport matrix or not.
     * @param calcTransportMatrix If true, than transport matrix will be calculated during the propagation.
     */
	void IsCalcTransportMatrix(bool calcTransportMatrix) {fCalcTransportMatrix = calcTransportMatrix;}

protected:
	/**
     * Updates the transport matrix F = newF * F.
     * @param F Input/output Current transport matrix.
     * @param newF Transport matrix to be included in the current transport matrix.
     */
	void UpdateF(
			std::vector<myf>& F,
			const std::vector<myf>& newF);

	/**
     * Check if the track parameters are c	orrect by checking their maximum acceptable values.
     * @param par Track parameter to be checked.
     */
	bool IsParCorrect(
		const CbmLitTrackParam* par);

private:
	TrackExtrapolatorPtr fExtrapolator; // Track extrapolator tool
	GeoNavigatorPtr fNavigator; // Track navigator tool
	MaterialEffectsPtr fMaterial; // Material effects tool

	std::vector<myf> fFm; // Transport matrix

	bool fDownstream; // if true than downstream propagation
	int fPDG; // PDG of the particle

	bool fCalcTransportMatrix; // if true than transport matrix is calculated
};

#endif //CBMLITTGEOTRACKPROPAGATOR_H_

