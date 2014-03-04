/**
 * \file CbmStsClusterFinder.h
 * \author Volker Friese <v.friese@gsi.de>
 * \since 11.09.06
 * \brief CBM task class for finding clusters in the STS
 *
 * Updated 26/06/2008 by R. Karabowicz.
 * Updated 03/03.2014 by A. Lebedev <andrey.lebedev@gsi.de>
 *
 **/

#ifndef CBMSTSCLUSTERFINDER_H
#define CBMSTSCLUSTERFINDER_H 1

#include "FairTask.h"

#include <map>
#include <set>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;

/**
 * \class CbmStsClusterFinder
 * \brief CBM task class for finding clusters in the STS
 **/
class CbmStsClusterFinder : public FairTask
{
public:
    /**
     * \brief Default constructor.
     **/
    CbmStsClusterFinder();

    /**
     * \brief Destructor.
     **/
    virtual ~CbmStsClusterFinder();

    /**
     * \brief Inherited from FairTask.
     **/
    virtual InitStatus Init();

    /**
     * \brief Inherited from FairTask.
     **/
    virtual void Exec(Option_t* opt);

    /**
     * \brief Inherited from FairTask.
     **/
    virtual void Finish();

    /**
     * \brief Inherited from FairTask.
     **/
    virtual void SetParContainers();

private:
    void MakeSets();

    void AnalyzeClusters();

    void AnalyzeCluster(Int_t clusterId);

    void SortDigis();

    void FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, const set<Int_t>& digiSet);

    CbmStsClusterFinder(const CbmStsClusterFinder&);
    CbmStsClusterFinder operator=(const CbmStsClusterFinder&);

    TClonesArray* fDigis;             ///> Input array of CbmStsDigi
    /// FIXME: Why do we need a second array with cluster candidates which is written to file?
    /// FIXME: This array is not used anywhere outside this class.
    TClonesArray* fClusterCandidates; ///> Input array of CbmStsCluster
    TClonesArray* fClusters;          ///> Input array of CbmStsCluster

    CbmGeoStsPar* fGeoPar;         ///> Geometry parameters
    CbmStsDigiPar* fDigiPar;       ///> Digitisation parameters
    CbmStsDigiScheme* fDigiScheme; ///> Digitisation scheme

    /// FIXME: I was not able to understand why the pointer address maps to set?
    /// FIXME: Much better to use ID of the sector or something similar.
    std::map<const CbmStsSector*, std::set<Int_t> > fDigiMapF; ///> sector digis (front)
    std::map<const CbmStsSector*, std::set<Int_t> > fDigiMapB; ///> sector digis (back)

    ClassDef(CbmStsClusterFinder, 1);
};

#endif
