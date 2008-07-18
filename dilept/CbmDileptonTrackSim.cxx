/****************************************************************************
 *  $Id: CbmDileptonTrackSim.cxx,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackSim
 *  Descripton: This is the data class for identified electrons after
 *              assigining the MC info. 
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackSim.cxx,v $
 *****************************************************************************/

#include "CbmDileptonTrackSim.h"

ClassImp(CbmDileptonTrackSim)
  
  CbmDileptonTrackSim::CbmDileptonTrackSim():CbmDileptonTrack()
{
}

CbmDileptonTrackSim::CbmDileptonTrackSim(TVector3 mom,Bool_t a1, Bool_t a2, Int_t a3, Int_t a4, Int_t a5):CbmDileptonTrack()
{
    fMomentum   = mom;
    isFakeTrack = a1;
    isFakeRing  = a2;
    MCPdg       = a3;
    MCMotherID  = a4;
    MCMotherPdg = a5;
}

CbmDileptonTrackSim::~CbmDileptonTrackSim()
{

}
