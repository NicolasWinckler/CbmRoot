// -----------------------------------------------------------------------
// -----                     CbmTrdRecoQa                       -----
// -----               Created 24.02.07 F. Uhlig                     -----
// -----------------------------------------------------------------------


/** CbmTrdRecoQa
 **
 *@author F.Uhlig <F.Uhlig@gsi.de>
 **
 ** Task class for performance checks concerning the PIDs 
 ** from different methods in the TRD Track.
 ** Fill histograms for all three different PID methods (Wkn, 
 ** Likelihood, Neural network) which are implemented so far.
 ** Fill booked spectra for each TRD Track.
 ** 
 **/


#ifndef CBMTRDRECOQA_H
#define CBMTRDRECOQA_H 1

#include "FairTask.h"

#include <map>
using std::map;

#include <vector>
using std::vector;

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;
class TClonesArray;
class TH1D;
class TH2D;
class TH2I;
class TGraph;
class TGraphErrors;
class TCanvas;


class CbmTrdRecoQa : public FairTask
{
 public:

  /* Defaul constructor */
  CbmTrdRecoQa();

  /* Standard constructor */
  CbmTrdRecoQa(const char* name,
	       const char* title = "FairTask");

  /* Destructor */
  virtual ~CbmTrdRecoQa();

  /* Initialisation */
  InitStatus Init();
  InitStatus ReInit();
  void SetParContainers();
  /* Execution */
  virtual void Exec(Option_t* option);

  /* Finish at the end of each event */
  virtual void Finish();


 private:

  /* Data branches*/
  TClonesArray* fClusters;
  TClonesArray* fHits;
  TClonesArray* fMCPoints;
  TClonesArray* fDigis;

  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdGeoHandler* fGeoHandler; //!

  /* Prepare the QA histograms */
  void PrepareHistograms();

  //Int_t SecRowToGlobalRow(Int_t secRow);


  /* Write the QA histograms to file*/
  void WriteHistograms();
  
  CbmTrdRecoQa(const CbmTrdRecoQa&);
  CbmTrdRecoQa& operator=(const CbmTrdRecoQa&);

  std::map<Int_t, TCanvas*> fModuleMap;
  std::map<Int_t, TGraph*> fModuleMapPoint;
  std::map<Int_t, TH2D*> fModuleMapDigi;
  std::map<Int_t, TH2I*> fModuleMapCluster;
  std::map<Int_t, TGraphErrors*> fModuleMapHit;
  // map<moduleAdress, <v[0]=Mc; v[1]=digi; v[3]=cluster; v[4]=hit> >
  ClassDef(CbmTrdRecoQa, 1)
    };

#endif
