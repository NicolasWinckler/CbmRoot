/**
 * \file CbmStsCluster.h
 * \author V.Friese <v.friese@gsi.de>
 * \since 28.08.06
 * \brief Data class for STS clusters.
 *
 * Updated 25/06/2008 by R. Karabowicz.
 * Updated 04/03/2014 by A. Lebedev <andrey.lebedev@gsi.de>
 **/

#ifndef CBMSTSCLUSTER_H
#define CBMSTSCLUSTER_H 1

#include "CbmCluster.h"

/**
 * \class CbmStsCluster
 * \brief Data class for STS clusters.
 **/
class CbmStsCluster : public CbmCluster
{
public:
    /**
     * \brief Default constructor
     **/
    CbmStsCluster();

    /**
     * \brief Destructor
     **/
    virtual ~CbmStsCluster();

    /**   Sector number
     **    Kept for bacjwrad compatibility
     **/
    void SetSectorNr(Int_t sectorNr)
    {
        fSectorNr = sectorNr;
    }
    void SetMean(Double_t mean)
    {
        fMean = mean;
    }
    void SetMeanError(Double_t meanError)
    {
        fMeanError = meanError;
    }

    /** Accessors **/
    Double_t GetMean() const
    {
        return fMean;
    }
    Double_t GetMeanError() const
    {
        return fMeanError;
    }
    /**   Sector number
     **    Kept for bacjwrad compatibility
     **/
    Int_t GetSectorNr() const
    {
        return fSectorNr;
    }

private:
    // This two parameters are kept for backward compatibility only.
    Double_t fMean; // FIXME: Modify clustering algorithm and remove this parameter.
    Double_t fMeanError; // FIXME: Modify clustering algorithm and remove this parameter.

    // TODO: fSectorNr is here for backward compatibility with classes
    // using CbmStsDigiScheme. Should be eventually removed.
    Int_t fSectorNr; ///< Number of sector within station

    ClassDef(CbmStsCluster, 2);
};

#endif
