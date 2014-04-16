/**
* \file CbmRichGeoOpt.h
*
* \brief Optimization of the RICH geometry.
*
* \author Tariq Mahmoud<t.mahmoud@gsi.de>
* \date 2014
**/

#ifndef CBM_RICH_GEO_OPT
#define CBM_RICH_GEO_OPT

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
* \class CbmRichGeoOpt
*
* \brief Optimization of the RICH geometry.
*
* \author Tariq Mahmoud<t.mahmoud@gsi.de>
* \date 2014
**/
class CbmRichGeoOpt : public FairTask
{

public:
   /**
    * \brief Standard constructor.
    */
	CbmRichGeoOpt();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmRichGeoOpt();

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

   TClonesArray* fRichPoints;
   TClonesArray* fMcTracks;
   TClonesArray* fRefPoints;

   Int_t fEventNum;



   /**
    * \brief Initialize histograms.
    */
   void InitHistograms();


   /**
    * \brief Copy constructor.
    */
   CbmRichGeoOpt(const CbmRichGeoOpt&);

   /**
    * \brief Assignment operator.
    */
   CbmRichGeoOpt& operator=(const CbmRichGeoOpt&);


	ClassDef(CbmRichGeoOpt,1)
};

#endif

