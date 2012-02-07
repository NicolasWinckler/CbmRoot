/**
* \file CbmRichRingFitterEllipseTau.h
*
* \brief Here the ring is fitted with Taubin algorithm from
*  A. Ayriyan, G. Ososkov, N. Chernov
*
* \author Alexander Ayriyan and Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#ifndef CBMRICHRINGFITTERELLIPSETAUH
#define CBMRICHRINGFITTERELLIPSETAUH

#include <vector>
#include "CbmRichRingFitterEllipseBase.h"

using std::vector;

#define MAX_NOF_HITS 400

/**
* \class CbmRichRingFitterEllipseTau
*
* \brief Here the ring is fitted with Taubin algorithm from
*  A. Ayriyan, G. Ososkov, N. Chernov
*
* \author Alexander Ayriyan and Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/
class CbmRichRingFitterEllipseTau : public CbmRichRingFitterEllipseBase
{
public:

   /**
    *\brief Default constructor.
    */
   CbmRichRingFitterEllipseTau();

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichRingFitterEllipseTau();

   /**
    * \brief Fit ring using hit coordinates from vectors.
    * \param[in] ring RICH ring to be fitted.
    * \param[in] hitX Vector of x coordinates of hits.
    * \param[in] hitY Vector of y coordinates of hits.
    */
   void DoFit(
         CbmRichRing *ring,
         const vector<Double_t>& hitX,
         const vector<Double_t>& hitY);

   /**
    * \brief Fit ring.
    * \param[in,out] ring RICH ring to be fitted.
    */
   void DoFit(
         CbmRichRing *ring);

   /**
    * \brief Transform fitted curve to ellipse parameters.
    * \param[in,out] ring RICH ring.
    */
	void TransEllipse(
	      CbmRichRing *ring);

	/**
	 * \brief Initialize all matrices.
	 */
	void InitMatrices();

	/**
	 * \brief Perform Taubin method.
	 */
	void Taubin();

	/**
	 * \brief Invert 5x5 matrix.
	 */
	void Inv5x5();

	/**
	 * \brief Matrices multiplication.
	 */
	void AMultB(
	      const Double_t * const ap,
	      Int_t na, Int_t ncolsa,
	      const Double_t * const bp,
	      Int_t nb,
	      Int_t ncolsb,
	      Double_t *cp);

	/**
	 * \brief Jacobi method.
	 */
	void Jacobi(
	      Double_t a[5][5],
	      Double_t d[5],
	      Double_t v[5][5]);

	/**
	 * \brief Find eigenvalues.
	 */
	void Eigsrt(
	      Double_t d[5],
	      Double_t v[5][5]);

private:
	Double_t fM[36];
	Double_t fP[25];
	Double_t fQ[25];
	Double_t fZ[MAX_NOF_HITS*6];
	Double_t fZT[MAX_NOF_HITS*6];

	Double_t fAlgPar[6];

	vector<Double_t> fX; // vector of X coordinates
	vector<Double_t> fY; // vector of Y coordinates

   ClassDef(CbmRichRingFitterEllipseTau,1);
};

#endif
