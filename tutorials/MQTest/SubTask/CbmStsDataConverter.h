/* 
 * File:   CbmStsDataConverter.h
 * Author: winckler
 *
 * Created on July 30, 2014, 11:44 AM
 */

#ifndef CBMSTSDATACONVERTER_H
#define	CBMSTSDATACONVERTER_H

#include <iostream>

#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"

#include "CbmUnpacker.h"
#include "CbmMicroSlice.h"

#include "CbmDetectorList.h"
#include "CbmStsDigi.h"

#include "FairMQLogger.h"

class CbmStsDataConverter {
public:
    CbmStsDataConverter();
    virtual ~CbmStsDataConverter();
    
    CbmMicroSlice GetCbmStsMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmStsDigi> vStsDigi);
    vector<CbmStsDigi>  StsConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void  StsCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice);

    void SetPrintOption(uint32_t DigiToPrint=0, bool print=true); 
    size_t GetDigiPayloadSize(){return fStsDigiPayloadSize;}
    
private:

    bool fPrint; //temporary
    uint32_t fDigiToPrint; //temporary
    size_t fStsDigiPayloadSize;
};

#endif	/* CBMSTSDATACONVERTER_H */

