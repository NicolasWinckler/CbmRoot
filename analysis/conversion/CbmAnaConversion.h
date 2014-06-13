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
class TH3;
class TH1D;
class TH2D;
class TH2I;
class TH3D;
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
   
   virtual void TomographyMC();
   //virtual void TomographyReco();
   virtual void Probability();
   virtual void InvariantMassTest();
   virtual int GetTest();
   virtual int GetNofEvents();



private:
   TH1D * fhGammaZ;
   TH1D * fhGammaZ_selected;
   TH2D * fTest;
   
   TH1D * fhNofElPrim;
   TH1D * fhNofElSec;
   TH1D * fhNofElAll;
   
   TH3D * fhTomography;
   TH2D * fhTomography_XZ;
   TH2D * fhTomography_YZ;
   TH2D * fhTomography_RICH_beampipe;
   
   TH1D * fhConversion;
   TH1D * fhConversion_prob;
   
   TH1D * fhInvariantMass_test;

   TClonesArray* fRichPoints;
   TClonesArray* fMcTracks;
   TClonesArray* fStsTracks;
   TClonesArray* fStsTrackMatches;
   TClonesArray* fGlobalTracks;

   Int_t fEventNum;
   
   int testint;



   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();
   
   vector<TH1*> fHistoList; //list of all histograms


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

