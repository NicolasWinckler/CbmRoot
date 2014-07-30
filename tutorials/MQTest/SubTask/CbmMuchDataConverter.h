/* 
 * File:   CbmMuchDataConverter.h
 * Author: winckler
 *
 * Created on July 30, 2014, 11:44 AM
 */

#ifndef CBMMUCHDATACONVERTER_H
#define	CBMMUCHDATACONVERTER_H


#include <iostream>

#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"

#include "CbmUnpacker.h"
#include "CbmMicroSlice.h"

#include "CbmDetectorList.h"
#include "CbmMuchDigi.h"

#include "FairMQLogger.h"

class CbmMuchDataConverter 
{
public:
    CbmMuchDataConverter();
    virtual ~CbmMuchDataConverter();
    
    vector<CbmMuchDigi> MuchConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void  MuchCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice);
    CbmMicroSlice GetCbmMuchMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmMuchDigi> vMuchDigi);

    void SetPrintOption(uint32_t DigiToPrint=0, bool print=true); 
    size_t GetDigiPayloadSize(){return fMuchDigiPayloadSize;}
private:
    bool fPrint; 
    uint32_t fDigiToPrint; 
    size_t fMuchDigiPayloadSize;
};

#endif	/* CBMMUCHDATACONVERTER_H */

