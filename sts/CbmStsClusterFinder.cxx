/**
 * \file CbmStsClusterFinder.cxx
 **/
#include "TClonesArray.h"
#include "TH1S.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "CbmGeoStsPar.h"
#include "CbmStsAddress.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsClusterFinder.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "TMath.h"

using namespace std;

CbmStsClusterFinder::CbmStsClusterFinder()
    : FairTask("CbmStsClusterFinder", 1)
    , fGeoPar(NULL)
    , fDigiPar(NULL)
    , fDigis(NULL)
    , fClusterCandidates(NULL)
    , fClusters(NULL)
    , fDigiScheme(NULL)
    , fDigiMapF()
    , fDigiMapB()
{
}

CbmStsClusterFinder::~CbmStsClusterFinder()
{
    if (fClusterCandidates)
    {
        fClusterCandidates->Delete();
        delete fClusterCandidates;
    }
    if (fClusters)
    {
        fClusters->Delete();
        delete fClusters;
    }
}

void CbmStsClusterFinder::Exec(Option_t* opt)
{
    // Clear output arrays
    fClusters->Delete();
    fClusterCandidates->Delete();

    SortDigis();

    // Find hits in sectors
    Int_t nofStations = fDigiScheme->GetNStations();
    for (Int_t iStation = 0; iStation < nofStations; iStation++)
    {
        CbmStsStation* station = fDigiScheme->GetStation(iStation);
        Int_t nofSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nofSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            if (fDigiMapF.find(sector) == fDigiMapF.end())
            {
                continue;
            }
            const set<Int_t>& frontSet = fDigiMapF[sector];
            FindClusters(iStation + 1, iSector + 1, 0, frontSet);
            if (sector->GetType() == 2 || sector->GetType() == 3)
            {
                if (fDigiMapB.find(sector) == fDigiMapB.end())
                {
                    continue;
                }
            }
            const set<Int_t>& backSet = fDigiMapB[sector];
            FindClusters(iStation + 1, iSector + 1, 1, backSet);
        } // Sector loop
    } // Station loop

    AnalyzeClusters();

    static Int_t eventNo = 0;
    LOG(INFO) << "CbmStsClusterFinder::Exec: eventNo=" << eventNo++ << std::endl;
}

void CbmStsClusterFinder::SetParContainers()
{
    FairRuntimeDb* db = FairRunAna::Instance()->GetRuntimeDb();
    if (NULL == db)
        LOG(FATAL) << "CbmStsClusterFinder::SetParContainers: No runtime database" << FairLogger::endl;
    fGeoPar = (CbmGeoStsPar*)db->getContainer("CbmGeoStsPar");
    fDigiPar = (CbmStsDigiPar*)db->getContainer("CbmStsDigiPar");
}

InitStatus CbmStsClusterFinder::Init()
{
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman)
        LOG(FATAL) << "CbmStsClusterFinder::Init: No FairRootManager" << FairLogger::endl;

    fDigis = (TClonesArray*)ioman->GetObject("StsDigi");
    if (NULL == fDigis)
        LOG(FATAL) << "CbmStsClusterFinder::Init: No StsDigi array" << FairLogger::endl;

    fClusterCandidates = new TClonesArray("CbmStsCluster", 30000);
    ioman->Register("StsClusterCand", "Cluster in STS", fClusterCandidates, kTRUE);

    fClusters = new TClonesArray("CbmStsCluster", 30000);
    ioman->Register("StsCluster", "Cluster in STS", fClusters, kTRUE);

    // Build digitisation scheme
    fDigiScheme = new CbmStsDigiScheme();
    if (!fDigiScheme->Init(fGeoPar, fDigiPar))
        return kERROR;

    // Create sectorwise digi sets
    MakeSets();

    return kSUCCESS;
}

void CbmStsClusterFinder::MakeSets()
{
    fDigiMapF.clear();
    fDigiMapB.clear();
    Int_t nofStations = fDigiScheme->GetNStations();
    for (Int_t iStation = 0; iStation < nofStations; iStation++)
    {
        CbmStsStation* station = fDigiScheme->GetStation(iStation);
        Int_t nofSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nofSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            fDigiMapF[sector] = set<Int_t>();
            if (sector->GetType() == 2 || sector->GetType() == 3)
            {
                fDigiMapB[sector] = set<Int_t>();
            }
        }
    }
}

void CbmStsClusterFinder::SortDigis()
{
    MakeSets();

    const Int_t nofDigis = fDigis->GetEntriesFast();
    for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++)
    {
        const CbmStsDigi* digi = static_cast<const CbmStsDigi*>(fDigis->At(iDigi));
        const Int_t stationNr = CbmStsAddress::GetElementId(digi->GetAddress(), kStsStation);
        const Int_t sectorNr = digi->GetSectorNr();
        const Int_t iSide = CbmStsAddress::GetElementId(digi->GetAddress(), kStsSide);
        CbmStsSector* sector = fDigiScheme->GetSector(stationNr, sectorNr);

        if (iSide == 0)
        {
            if (fDigiMapF.find(sector) == fDigiMapF.end())
            {
                continue;
            }
            fDigiMapF[sector].insert(iDigi);
        }
        else if (iSide == 1)
        {
            if (fDigiMapB.find(sector) == fDigiMapB.end())
            {
                continue;
            }
            fDigiMapB[sector].insert(iDigi);
        }
    }
}

void CbmStsClusterFinder::FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, const set<Int_t>& digiSet)
{
    CbmStsCluster* clusterCand = NULL;

    Int_t lastDigiNr = -1;
    Int_t lastChannelId = -1;
    Double_t lastDigiSig = 100000.;
    Int_t clusterMaxNr = -1;
    Int_t clusterMaxPos = -1;
    Double_t clusterMaxSig = -1.;
    Int_t startChannelId = 0;

    set<Int_t>::const_iterator it = digiSet.begin();
    set<Int_t>::const_iterator it_end = digiSet.end();
    for (; it != it_end; ++it)
    {
        Int_t digiIndex = (*it);
        const CbmStsDigi* digi = static_cast<const CbmStsDigi*>(fDigis->At(digiIndex));

        Int_t channelId = CbmStsAddress::GetElementId(digi->GetAddress(), kStsChannel);
        Double_t digiSig = digi->GetCharge();

        if (lastChannelId == -1)
        {
            Int_t size = fClusterCandidates->GetEntriesFast();
            clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
            clusterCand->SetAddress(digi->GetAddress());
            startChannelId = channelId;
        }

        if (channelId == lastChannelId + 1)
        {
            // if the signal is larger that last one and the previous one is smaller than maximum
            if (digiSig > lastDigiSig && lastDigiSig < clusterMaxSig && digiSig != clusterMaxSig)
            {
                Int_t size = fClusterCandidates->GetEntriesFast();
                clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
                clusterCand->SetAddress(digi->GetAddress());
                clusterCand->AddDigi(lastDigiNr);

                clusterMaxPos = -1;
                clusterMaxSig = -1.;
                startChannelId = channelId;
            }
        }
        else if (lastChannelId >= 0)
        {
            Int_t size = fClusterCandidates->GetEntriesFast();
            clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
            clusterCand->SetAddress(digi->GetAddress());

            clusterMaxPos = -1;
            clusterMaxSig = -1.;
            startChannelId = channelId;
        }

        if (digiSig > clusterMaxSig)
        {
            clusterMaxNr = digiIndex;
            clusterMaxPos = channelId;
            clusterMaxSig = digiSig;
        }

        Int_t clusterWidth = lastChannelId - startChannelId + 1;
        if (clusterWidth < 90)
        {
            clusterCand->AddDigi(digiIndex);
        }
        else
        {
            Int_t size = fClusterCandidates->GetEntriesFast();
            clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
            clusterCand->SetAddress(digi->GetAddress());
            clusterCand->AddDigi(digiIndex);
            clusterWidth = lastChannelId - startChannelId + 1;

            clusterMaxPos = -1;
            clusterMaxSig = -1.;
            startChannelId = channelId;
        }

        lastDigiNr = digiIndex;
        lastChannelId = channelId;
        lastDigiSig = digiSig;
    }
}

void CbmStsClusterFinder::AnalyzeClusters()
{
    Int_t nofClusterCandidates = fClusterCandidates->GetEntriesFast();
    for (Int_t iclus = 0; iclus < nofClusterCandidates; iclus++)
    {
        AnalyzeCluster(iclus);
    }
}

void CbmStsClusterFinder::AnalyzeCluster(Int_t clusterId)
{
    const CbmStsCluster* clusterCand = static_cast<const CbmStsCluster*>(fClusterCandidates->At(clusterId));

    Double_t chanNr = 0;
    Double_t chanADC = 0.;
    Double_t sumW = 0;
    Double_t sumWX = 0;
    Double_t error = 0;
    Int_t nofDigis = clusterCand->GetNofDigis();
    for (Int_t i = 0; i < nofDigis; ++i)
    {
        CbmStsDigi* digi = (CbmStsDigi*)fDigis->At(clusterCand->GetDigi(i));
        chanNr = (Double_t)(CbmStsAddress::GetElementId(digi->GetAddress(), kStsChannel));
        chanADC = digi->GetCharge();
        sumW += chanADC;
        sumWX += chanNr * chanADC;
        error += (chanADC * chanADC);
    }

    if (sumW > 50.)
    {
        Int_t size = fClusters->GetEntriesFast();
        CbmStsCluster* cluster = new ((*fClusters)[size]) CbmStsCluster();

        for (Int_t i = 0; i < nofDigis; ++i)
        {
           Int_t digiIndex = clusterCand->GetDigi(i);
            const CbmStsDigi* digi = static_cast<const CbmStsDigi*>(fDigis->At(digiIndex));
            if (i == 0)
            {
                cluster->SetAddress(digi->GetAddress());
                cluster->SetSectorNr(digi->GetSectorNr());
            }
            cluster->AddDigi(digiIndex);
        }
        cluster->SetMean(sumWX / sumW);
        cluster->SetMeanError((1. / (sumW)) * TMath::Sqrt(error));
    }
}

void CbmStsClusterFinder::Finish()
{
}

ClassImp(CbmStsClusterFinder)
