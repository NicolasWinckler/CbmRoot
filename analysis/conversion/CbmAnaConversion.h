/**
* \file CbmAnaConversion.h
*
* \brief Optimization of the RICH geometry.
*
* \author Tariq Mahmoud<t.mahmoud@gsi.de>
* \date 2014
**/

#ifndef CBM_ANA_CONVERSION
#define CBM_ANA_CONVERSION

#include "FairTask.h"
class TH1;
class TH2;
class TH1D;
class TH2D;
class TClonesArray;
class CbmRichRing;
class TCanvas;

#include <vector>
#include <map>

using namespace std;

/**
* \class CbmAnaConversion
*
* \brief Optimization of the RICH geometry.
*
* \author Tariq Mahmoud<t.mahmoud@gsi.de>
* \date 2014
**/
class CbmAnaConversion : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
	CbmAnaConversion();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmAnaConversion();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
		   Option_t* option);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();



private:
   TH1D * fhGammaZ;

   TClonesArray* fRichPoints;
   TClonesArray* fMcTracks;
   TClonesArray* fStsTracks;
   TClonesArray* fStsTrackMatches;
   TClonesArray* fGlobalTracks;

   Int_t fEventNum;



   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();


   /**
    * \brief Copy constructor.
    */
   CbmAnaConversion(const CbmAnaConversion&);

   /**
    * \brief Assignment operator.
    */
   CbmAnaConversion& operator=(const CbmAnaConversion&);


	ClassDef(CbmAnaConversion,1)
};

#endif

