/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/* 
 * File:   CbmDataConverter.h
 * Author: winckler
 *
 * Created on July 18, 2014, 7:23 PM
 */

#ifndef CBMDATACONVERTER_H
#define	CBMDATACONVERTER_H

#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "FairTask.h"
#include "CbmUnpacker.h"
#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"
#include "CbmDetectorList.h"
#include "CbmMicroSlice.h"
#include "CbmStsDigi.h"
#include "CbmMuchDigi.h"
#include "FairMQLogger.h"
class CbmDataConverter : public FairTask
{
public:
    CbmDataConverter();
    virtual ~CbmDataConverter();
    //virtual void Exec(Option_t* opt);
    //virtual InitStatus Init();
    
    CbmMicroSlice GetCbmMicroSlice(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    CbmMicroSlice GetCbmMicroSlice(DetectorId iDet, CbmTimeSlice* CbmTSlice);
    
    CbmMicroSlice GetCbmStsMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmStsDigi> vStsDigi);

    void SetPrintOption(uint32_t DigiToPrint, bool print=true); //temporary
    
    /// stuff to to convert fles microslices to cbm root timeslices
    void CbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    
    /// stuff to handle cbm root timeslices
    void InitCbmTS(Double_t start=0., Double_t duration=1000.);
    void InitCbmTSOutputFile(const char* filename);
    void FillCbmTSTree();
    void SetCbmTSInterval(Double_t start, Double_t duration);
    void WriteTreeToFile();
    
    
protected:
    vector<CbmStsDigi>  StsConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    vector<CbmMuchDigi> MuchConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    
    void  StsCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void  MuchCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    
    
    TFile* fOutFile;
    TTree* fTree;
    CbmTimeSlice* fCBMTimeSlice;
    /** Start time of current time slice [ns] **/
    Double_t fCurrentStartTime;
    /** Duration of time slice [ns] **/
    Double_t fDuration;
    bool fPrint; //temporary
    uint32_t fDigiToPrint; //temporary

    
};

#endif	/* CBMDATACONVERTER_H */

