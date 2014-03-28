//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsMatchHits source file            -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

// --- Includes from ROOT
#include "TClonesArray.h"

// --- Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// --- Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmGeoPassivePar.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsHit.h"
#include "CbmStsMatchHits.h"
#include "CbmStsPoint.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "FairGeoVector.h"
#include "FairGeoNode.h"
#include "CbmStsAddress.h"

#include "CbmMatch.h"
#include "CbmLink.h"

#include "TMath.h"

#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

CbmStsMatchHits::CbmStsMatchHits()
    : FairTask("CbmStsMatchHits")
    , fGeoPar(NULL)
    , fDigiPar(NULL)
    , fPassGeo(NULL)
    , fPoints(NULL)
    , fDigis(NULL)
    , fDigiMatches(NULL)
    , fHits(NULL)
    , fDigiScheme(NULL)
    , fTimer()
    , fNEvents(0)
    , fNEventsFailed(0)
    , fTime(0.)
    , fNHits(0.)
    , fNMatched(0.)
    , fNDistant(0.)
    , fNBackgrd(0.)
    , fTargetPos(0., 0., 0.)
    , fNStations(0)
    , fRealistic(kTRUE)
    , fCandMap()
    , fIter()
{
    fDigiScheme = new CbmStsDigiScheme();
}

// -----   Destructor   ----------------------------------------------------
CbmStsMatchHits::~CbmStsMatchHits()
{
    if (fPassGeo)
        delete fPassGeo;
    if (fGeoPar)
        delete fGeoPar;
    if (fDigiPar)
        delete fDigiPar;
    if (fDigiScheme)
        delete fDigiScheme;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmStsMatchHits::Exec(Option_t* opt)
{

    if (fRealistic)
    {
        ExecReal(opt);
        return;
    }

    // Timer
    fTimer.Start();
//    Bool_t warn = kFALSE;

    // Counters
    Int_t nHits = fHits->GetEntriesFast();
    Int_t nNoDigi = 0;
    Int_t nBackgrd = 0;
    Int_t nDistant = 0;
    Int_t nMatched = 0;

    // Loop over all StsHits
    for (Int_t iHit = 0; iHit < nHits; iHit++)
    {
        CbmStsHit* hit = (CbmStsHit*)fHits->At(iHit);
        if (!hit)
        {
            cout << "-W- " << GetName() << "::Exec: Empty hit at index " << iHit << endl;
//            warn = kTRUE;
            continue;
        }

        // Determine sector type and channel numbers
        Int_t iStation = CbmStsAddress::GetElementId(hit->GetAddress(), kStsStation); // hit->GetStationNr();
        Int_t iSector = hit->GetSectorNr();
        CbmStsStation* station = fDigiScheme->GetStationByNr(iStation);
        CbmStsSector* sector = fDigiScheme->GetSector(iStation, iSector);
        Int_t iType = sector->GetType();
        CbmMatch* dMatchF = NULL;
        CbmMatch* dMatchB = NULL;

        // Hit coordinates and errors
        Double_t xH = hit->GetX();
        Double_t yH = hit->GetY();
        Double_t dX = hit->GetDx();
        Double_t dY = hit->GetDy();

        // Get front side DigiMatch corresponding to hit
        Int_t iMatchF = (Int_t)hit->GetFrontDigiId();
        if (iMatchF >= 0)
            dMatchF = (CbmMatch*)fDigiMatches->At(iMatchF);
        if (!dMatchF)
        {
            cout << "-E- " << GetName() << "::Exec: "
                 << "No DigiMatchF for hit " << iHit << endl;
            hit->SetRefId(-1);
            nNoDigi++;
 //           warn = kTRUE;
            continue;
        }

        // Get back side DigiMatch of hit (for strip sensors only)
        if (iType != 1)
        {
            Int_t iMatchB = (Int_t)hit->GetBackDigiId();
            if (iMatchB >= 0)
                dMatchB = (CbmMatch*)fDigiMatches->At(iMatchB);
            if (!dMatchB)
            {
                cout << "-E- " << GetName() << "::Exec: "
                     << "No DigiMatchB for hit " << iHit << endl;
                hit->SetRefId(-1);
                nNoDigi++;
 //               warn = kTRUE;
                continue;
            }
        }

        // Map candidate points to distance to hit
        fCandMap.clear();
        Int_t nPointsF = 0;
        Int_t nPointsB = 0;

        if (iType == 1)
        {
            // Case pixel: Candidates are all points of the digi.
            for (Int_t jMatchF = 0; jMatchF < dMatchF->GetNofLinks(); jMatchF++)
            {
                Int_t iPointF = dMatchF->GetLink(jMatchF).GetEntry();
                if (iPointF < 0)
                    continue;
                nPointsF++;
                // Calculate distance to hit
                CbmStsPoint* point = (CbmStsPoint*)fPoints->At(iPointF);
                if (!point)
                {
                    cout << "-E- " << GetName() << "::Exec: "
                         << "No StsPoint (" << iPointF << ") for pixel hit " << iHit << endl;
   //                 warn = kTRUE;
                    continue;
                }
                // point coordinates in station midplane
                Double_t xP = point->GetX(station->GetZ());
                Double_t yP = point->GetY(station->GetZ());
                Double_t dist = TMath::Sqrt((xP - xH) * (xP - xH) + (yP - yH) * (yP - yH));
                //	cout << "candidate dist = " << dist << endl;
                fCandMap[dist] = iPointF;
            } // front digi loop
        }     // pixel sensor

        else if (iType == 2 || iType == 3)
        {
            // Case strip: Candidates are points corresponding to front
            // and back side digis
            for (Int_t jMatchF = 0; jMatchF < dMatchF->GetNofLinks(); jMatchF++)
            {
                Int_t iPointF = dMatchF->GetLink(jMatchF).GetEntry();
                if (iPointF < 0)
                    continue;
                //	cout << " got pointf index = " << iPointF << endl;
                nPointsF++;
                for (Int_t iMatchB = 0; iMatchB < dMatchB->GetNofLinks(); iMatchB++)
                {
                    Int_t iPointB = dMatchB->GetLink(iMatchB).GetEntry();
                    if (iPointB < 0)
                        continue;
                    //	  cout << " got pointb index = " << iPointB << endl;
                    if (iMatchF == 0)
                        nPointsB++;
                    if (iPointB != iPointF)
                        continue; // chance combination
                    // Calculate distance to hit
                    CbmStsPoint* point = (CbmStsPoint*)fPoints->At(iPointF);
                    if (!point)
                    {
                        cout << "-E- " << GetName() << "::Exec: "
                             << "No StsPoint (" << iPointF << ") for strip hit " << iHit << endl;
      //                  warn = kTRUE;
                        continue;
                    }
                    // point coordinates in station midplane
                    Double_t xP = point->GetX(station->GetZ());
                    Double_t yP = point->GetY(station->GetZ());
                    Double_t dist = TMath::Sqrt((xP - xH) * (xP - xH) + (yP - yH) * (yP - yH));
                    fCandMap[dist] = iPointF;
                } // back digi loop
            }     // front digi loop
        }         // strip sensor

        else
        {
            // Unknown sensor type
            cout << "-E- " << GetName() << "::Exec: Unknwon sensor type " << iType << endl;
            Fatal("Exec", "Unknwon sensor type");
        }

        if (fVerbose > 1)
            cout << "-I- " << GetName() << ": Hit " << iHit << ", type " << iType << ", points " << nPointsF << " / " << nPointsB << ", candidates "
                 << fCandMap.size();

        // Check for at least one candidate point. Else background hit.
        // This can happen for noise digis or for fake combinations
        // of strip digis.
        if (fCandMap.empty())
        {
            hit->SetRefId(-1);
            if (fVerbose > 1)
                cout << ", background " << endl;
            nBackgrd++;
            continue;
        }

        // Select closest point from candidates.
        Double_t distMin = 999999.;
        Int_t iPoint = -1;
        for (fIter = fCandMap.begin(); fIter != fCandMap.end(); fIter++)
        {
            if ((*fIter).first < distMin)
            {
                distMin = (*fIter).first;
                iPoint = (*fIter).second;
            }
        }
        if (iPoint < 0)
        {
            cout << "-E- " << GetName() << "::Exec: "
                 << "No closest point found in candidate map!" << endl;
            Fatal("Exec", "No closest point");
        }
        CbmStsPoint* point = (CbmStsPoint*)fPoints->At(iPoint);
        if (fVerbose > 1)
            cout << ", matched to " << iPoint << ", distance " << distMin << " cm";

        // Check whether closest point is more than 5 sigma away from hit.
        // This should not happen in case of pixel or strip OSU sensors,
        // but can happen for strip GSI sensors due to multiple
        // crossings of front and back side strips.
        Double_t xP = point->GetX(station->GetZ());
        Double_t yP = point->GetY(station->GetZ());
        if (TMath::Abs(xP - xH) > 5. * dX || TMath::Abs(yP - yH) > 5. * dY)
        {
            hit->SetRefId(-2);
            nDistant++;
            if (fVerbose > 1)
                cout << ", distant" << endl;
            if (iType == 1 || iType == 2)
            {
                cout << "-W- " << fName << "::Exec: "
                     << "Distant hit in pixel / strip MSU station" << endl;
                cout << "Hit " << iHit << " coordinates " << xH << ", " << yH << endl;
                cout << "Matched point " << iPoint << ", distance " << distMin << endl;
                cout << "Distance y " << TMath::Abs(xP - xH) << " Error " << dX << endl;
                cout << "Distance y " << TMath::Abs(yP - yH) << " Error " << dY << endl;
            }

            continue;
        }

        // Match closest StsPoint to hit
        hit->SetRefId(iPoint);
        nMatched++;
        if (fVerbose > 1)
            cout << ", good match" << endl;

        // Check whether hit and point are in the same station.
        // Else something has really gone wrong.
        if (TMath::Abs(hit->GetZ() - point->GetZ()) > 1.)
        {
            cout << "-E- " << GetName() << "::Exec: Hit " << iHit << " is at z = " << hit->GetZ() << " cm, but matched point " << iPoint
                 << " is at z = " << point->GetZ() << "cm " << endl;
            Fatal("Exec", "Different stations for hit and point");
        }

    } // Loop over StsHits

    // Event statistics
    fTimer.Stop();

    cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) << fixed << right << fTimer.RealTime() << " s, hits " << nHits << ", matched "
         << nMatched << ", distant " << nDistant << ", background " << setw(6) << nBackgrd << endl;

        fNEvents++;
        fTime += fTimer.RealTime();
        fNHits += Double_t(nHits);
        fNMatched += Double_t(nMatched);
        fNDistant += Double_t(nDistant);
        fNBackgrd += Double_t(nBackgrd);

    static Int_t eventNo = 0;
    cout << "CbmStsMatchHits::Exec: eventNo=" << eventNo++ << std::endl;
}

// -------------------------------------------------------------------------

// -----   Public method ExecReal   --------------------------------------------
void CbmStsMatchHits::ExecReal(Option_t* opt)
{

    // Timer
    fTimer.Start();
    Bool_t warn = kFALSE;

    // Counters
    Int_t nHits = fHits->GetEntriesFast();
    Int_t nNoDigi = 0;
    Int_t nBackgrd = 0;
    Int_t nDistant = 0;
    Int_t nMatched = 0;

    Int_t nofStsHits = fHits->GetEntriesFast();
    Int_t nofStsPoints = fPoints->GetEntriesFast();
    // cout << "there are " << nofStsPoints << " points and " << nofStsHits << " hits." << endl;
    Int_t hitStationLimits[2][100];

    // Workaround. TODO.
    fNStations = 8;

    for (Int_t ist = 0; ist < fNStations; ist++)
    {
        hitStationLimits[0][ist] = -1;
        hitStationLimits[1][ist] = -1;
    }

    // check for limits of hit indices on different stations...
    for (Int_t ihit = 0; ihit < nofStsHits; ihit++)
    {
        CbmStsHit* stsHit = (CbmStsHit*)fHits->At(ihit);
        stsHit->SetRefId(-1);
        if (hitStationLimits[0][CbmStsAddress::GetElementId(stsHit->GetAddress(), kStsStation) - 1] == -1)
            hitStationLimits[0][CbmStsAddress::GetElementId(stsHit->GetAddress(), kStsStation) - 1] = ihit;
        CbmStsHit* stsHitBack = (CbmStsHit*)fHits->At(nofStsHits - ihit - 1);
        if (hitStationLimits[1][CbmStsAddress::GetElementId(stsHitBack->GetAddress(), kStsStation) - 1] == -1)
        {
            hitStationLimits[1][CbmStsAddress::GetElementId(stsHitBack->GetAddress(), kStsStation) - 1] = nofStsHits - ihit;
        }
    }
    // for ( Int_t istat = 0 ; istat < fNStations ; istat++ )
    //   cout << "station " << istat << " hits from " << hitStationLimits[0][istat] << " to " << hitStationLimits[1][istat] << endl;

    for (Int_t ipnt = 0; ipnt < nofStsPoints; ipnt++)
    {
        CbmStsPoint* stsPoint = (CbmStsPoint*)fPoints->At(ipnt);

        // This is a workaround. Has to be worked out when new digi scheme is implemented. TODO.
        Double_t zPoint = stsPoint->GetZIn();
        Int_t stationNr = TMath::Nint((zPoint - 30.) / 10.);

        // This does not work with new geometries since the StsGeoPar are not filled.
        // Int_t stationNr = fStationNrFromMcId[stsPoint->GetDetectorID()]];

        Int_t startHit = hitStationLimits[0][stationNr];
        Int_t finalHit = hitStationLimits[1][stationNr];

        if (startHit == -1 && finalHit == -1)
            continue;
        if (startHit < 0 || finalHit < 0 || startHit > fHits->GetEntriesFast() || finalHit > fHits->GetEntriesFast())
            continue;

        Int_t NofMatched = 0;
        Double_t GoodHit[100];
        Int_t GoodHitIndex[nofStsHits];
        for (Int_t ihit = startHit; ihit < finalHit; ihit++)
        {
            CbmStsHit* stsHit = (CbmStsHit*)fHits->At(ihit);
            if (NULL == stsHit)
            {
                std::cout << "CbmStsMatchHits::ExecReal: CbmStsHit NULL pointer: ihit=" << ihit << " "
                          << "startHit=" << startHit << " finalHit=" << finalHit << std::endl;
                continue;
            }
            Double_t xH = stsHit->GetX();
            Double_t yH = stsHit->GetY();
            Double_t xP = stsPoint->GetX(stsHit->GetZ());
            Double_t yP = stsPoint->GetY(stsHit->GetZ());
            if ((TMath::Abs(stsHit->GetX() - stsPoint->GetX(stsHit->GetZ())) < .01) && (TMath::Abs(stsHit->GetY() - stsPoint->GetY(stsHit->GetZ())) < .04))
            {
                Double_t dist = TMath::Sqrt((xP - xH) * (xP - xH) + (yP - yH) * (yP - yH));
                //	cout << "matching "
                //	     << "X: " << stsHit->GetX() << " - " << stsPoint->GetX(stsHit->GetZ())
                //	     << "Y: " << stsHit->GetY() << " - " << stsPoint->GetY(stsHit->GetZ()) << endl;
                //		cout << "candidate dist = " << dist << endl;

                //	cout << "setting ref index = " << stsHit->GetRefIndex() << " (max sts pnt = " << nofStsPoints << ")" << endl;
                nMatched++;
                NofMatched++;
                GoodHitIndex[NofMatched] = ihit;
                GoodHit[NofMatched] = dist;
                //         cout <<" NofMatched = "<<NofMatched<<" dist = "<<dist<<flush;
            }
        }

        Int_t k = NofMatched;
        if (NofMatched > 1)
        {
            for (Int_t i = NofMatched - 1; i > 0; i--)
            {

                if (GoodHit[i] < GoodHit[k])
                {
                    k = i;
                }
            }
        }
        if (NofMatched > 0)
        {
            CbmStsHit* stsHit = (CbmStsHit*)fHits->At(GoodHitIndex[k]);
            stsHit->SetRefId(ipnt);
        }
    }

    // Event statistics
    fTimer.Stop();
    if (warn)
        cout << "- ";
    else
        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) << fixed << right << fTimer.RealTime() << " s, hits " << nHits << ", matched "
         << nMatched << ", distant " << nDistant << ", background " << setw(6) << nBackgrd << endl;

    // Run statistics
    if (warn)
        fNEventsFailed++;
    else
    {
        fNEvents++;
        fTime += fTimer.RealTime();
        fNHits += Double_t(nHits);
        fNMatched += Double_t(nMatched);
        fNDistant += Double_t(nDistant);
        fNBackgrd += Double_t(nBackgrd);
    }

    static Int_t eventNo = 0;
    LOG(INFO) << "CbmStsMatchHits::Exec: eventNo=" << eventNo++ << std::endl;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmStsMatchHits::SetParContainers()
{

    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
        Fatal("SetParContainers", "No analysis run");
    FairRuntimeDb* db = run->GetRuntimeDb();
    if (!db)
        Fatal("SetParContainers", "No runtime database");

    // Get STS geometry and digitisation parameter container
    fPassGeo = (CbmGeoPassivePar*)db->getContainer("CbmGeoPassivePar");
    fGeoPar = (CbmGeoStsPar*)db->getContainer("CbmGeoStsPar");
    fDigiPar = (CbmStsDigiPar*)db->getContainer("CbmStsDigiPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmStsMatchHits::Init()
{

    // Reset counters
    fNEvents = fNEventsFailed = 0;
    fTime = fNHits = fNMatched = fNDistant = fNBackgrd = 0.;

    // Get input arrays
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "No FairRootManager");
    fPoints = (TClonesArray*)ioman->GetObject("StsPoint");
    if (!fPoints)
    {
        cout << "-E- " << GetName() << "::Init: No STSPoint array!" << endl;
        return kERROR;
    }
    fDigis = (TClonesArray*)ioman->GetObject("StsDigi");
    if (!fDigis)
    {
        cout << "-E- " << GetName() << "::Init: No StsDigi array!" << endl;
        return kERROR;
    }
    fDigiMatches = (TClonesArray*)ioman->GetObject("StsDigiMatch");
    if (!fDigiMatches)
    {
        cout << "-E- " << GetName() << "::Init: No StsDigiMatch array!" << endl;
        return kERROR;
    }
    fHits = (TClonesArray*)ioman->GetObject("StsHit");
    if (!fHits)
    {
        cout << "-E- " << GetName() << "::Init: No StsHit array!" << endl;
        return kERROR;
    }

    InitStatus geoStatus = GetGeometry();
    if (geoStatus != kSUCCESS)
    {
        cout << "-E- " << GetName() << "::Init: Error in reading geometry!" << endl;
        return geoStatus;
    }

    // Build digitisation scheme

    if (fDigiScheme->Init(fGeoPar, fDigiPar))
    {

        if (fVerbose == 1 || fVerbose == 2)
            fDigiScheme->Print(kFALSE);
        else if (fVerbose > 2)
            fDigiScheme->Print(kTRUE);
        cout << "-I- " << fName << "::Init: "
             << "STS digitisation scheme succesfully initialised" << endl;
        cout << "    Stations: " << fDigiScheme->GetNStations() << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " << fDigiScheme->GetNChannels()
             << endl;
        return kSUCCESS;
    }

    return kERROR;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsMatchHits::ReInit()
{

    // Clear digitisation scheme
    fDigiScheme->Clear();

    // Build new digitisation scheme
    if (fDigiScheme->Init(fGeoPar, fDigiPar))
        return kSUCCESS;

    InitStatus geoStatus = GetGeometry();
    if (geoStatus != kSUCCESS)
    {
        cout << "-E- " << GetName() << "::Init: Error in reading geometry!" << endl;
        return geoStatus;
    }

    return kERROR;
}
// -------------------------------------------------------------------------

// -----   Private method GetGeometry   ------------------------------------
InitStatus CbmStsMatchHits::GetGeometry()
{
    cout << "-W- " << endl;
    // Get target geometry
    if (!fPassGeo)
    {
        cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!" << endl;
        fTargetPos.SetXYZ(0., 0., 0.);
        return kERROR;
    }
    TObjArray* passNodes = fPassGeo->GetGeoPassiveNodes();
    if (!passNodes)
    {
        cout << "-W- " << GetName() << "::GetGeometry: No passive node array" << endl;
        fTargetPos.SetXYZ(0., 0., 0.);
        return kERROR;
    }
    /*  FairGeoNode* target = (FairGeoNode*) passNodes->FindObject("targ");
    if ( ! target ) {
      cout << "-E- " << GetName() << "::GetGeometry: No target node"
       << endl;
      fTargetPos.SetXYZ(0., 0., 0.);
      return kERROR;
    }
    FairGeoVector targetPos = target->getLabTransform()->getTranslation();
    FairGeoVector centerPos = target->getCenterPosition().getTranslation();
    Double_t targetX = targetPos.X() + centerPos.X();
    Double_t targetY = targetPos.Y() + centerPos.Y();
    Double_t targetZ = targetPos.Z() + centerPos.Z();
    fTargetPos.SetXYZ(targetX, targetY, targetZ);*/

    // Get STS geometry
    if (!fGeoPar)
    {
        cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!" << endl;
        fNStations = 0;
        return kERROR;
    }
    TObjArray* stsNodes = fGeoPar->GetGeoSensitiveNodes();
    if (!stsNodes)
    {
        cout << "-E- " << GetName() << "::GetGeometry: No STS node array" << endl;
        fNStations = 0;
        return kERROR;
    }
    Int_t tempNofStations = stsNodes->GetEntries();
    cout << "Nodes in STS: " << tempNofStations << endl;

    cout << "There are " << tempNofStations << " nodes" << (tempNofStations > 10 ? "!!!" : "") << endl;

    TString geoNodeName;
    fNStations = 0;
    TString stationNames[1000];
    for (Int_t ist = 0; ist < tempNofStations; ist++)
    {
        FairGeoNode* stsNode = (FairGeoNode*)stsNodes->At(ist);
        if (!stsNode)
        {
            cout << "-W- CbmStsDigiScheme::Init: station#" << ist << " not found among sensitive nodes " << endl;
            continue;
        }
        geoNodeName = stsNode->getName();
        //    TArrayD* params = stsNode->getParameters();

        Bool_t stationKnown = kFALSE;
        // check if the node belongs to some station, save the MCId and outer radius
        for (Int_t ikst = 0; ikst < fNStations; ikst++)
            if (geoNodeName.Contains(stationNames[ikst]))
            {
                fStationNrFromMcId[stsNode->getMCid()] = ikst;
                stationKnown = kTRUE;
            }

        if (stationKnown)
            continue;

        // if not known, register it and save MCId
        fStationNrFromMcId[stsNode->getMCid()] = fNStations;

        // it will work only if the node name is organized as:
        // for station name is "stsstationXX", where XX is the station number (f.e. XX=07 for station number 7)
        // for sector  name is "stsstationXXanythingHereToDistinguishDifferentSectors"
        geoNodeName.Remove(12, geoNodeName.Length() - 12);
        stationNames[fNStations] = geoNodeName.Data();
        fNStations++;

        cout << "station #" << fNStations << " has MCID = " << stsNode->getMCid() << " and name " << stsNode->GetName() << endl;

        //    fStationsMCId[fNStations] = stsNode->getMCid(); // not used
    }
    cout << "There are " << fNStations << " stations" << endl;

    return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void CbmStsMatchHits::Finish()
{

    cout << endl;
    cout << "============================================================" << endl;
    cout << "=====   " << GetName() << ": Run summary " << endl;
    cout << "===== " << endl;
    cout << "===== Good events   : " << setw(6) << fNEvents << endl;
    cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
    cout << "===== Average time  : " << setprecision(4) << setw(8) << right << fTime / Double_t(fNEvents) << " s" << endl;
    cout << "===== " << endl;
    cout << "===== Hits per event  : " << fixed << setprecision(0) << fNHits / Double_t(fNEvents) << endl;
    cout << setprecision(2);
    cout << "===== Matched hits    : " << fixed << setw(6) << right << fNMatched / fNHits * 100. << " %" << endl;
    cout << "===== Distant hits    : " << fixed << setw(6) << right << fNDistant / fNHits * 100. << " %" << endl;
    cout << "===== Background hits : " << fixed << setw(6) << right << fNBackgrd / fNHits * 100. << " % " << endl;
    cout << "============================================================" << endl;
}
// -------------------------------------------------------------------------

ClassImp(CbmStsMatchHits)
