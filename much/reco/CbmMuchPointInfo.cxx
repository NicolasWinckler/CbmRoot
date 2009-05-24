// -------------------------------------------------------------------------
// -----                    CbmMuchPointInfo source file               -----
// -----                   Created 21/11/07 by E. Kryshen              -----
// -------------------------------------------------------------------------


#include "CbmMuchPointInfo.h"


ClassImp(CbmMuchPointInfo)

// -------------------------------------------------------------------------
CbmMuchPointInfo::CbmMuchPointInfo():TObject(){};
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMuchPointInfo::CbmMuchPointInfo
    (Int_t pdgCode, Int_t motherPdg, Double_t kine, Double_t length, Int_t stationId):
     TObject(),fPdgCode(pdgCode),fMotherPdg(motherPdg),fKine(kine),fLength(length),fStationId(stationId),fCharge(0),fS(0),fNPads(0){}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchPointInfo::Print(){
    printf("PointInfo:pdg=%i,kine=%6.4f,length=%4.2f,st_id=%i,charge=%i\n",
           fPdgCode,fKine,fLength,fStationId,fCharge);

}
// -------------------------------------------------------------------------
