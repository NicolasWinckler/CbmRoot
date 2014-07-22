/* 
 * File:   CbmDataConverter.h
 * Author: winckler
 *
 * Created on July 18, 2014, 7:23 PM
 */

#ifndef CBMDATACONVERTER_H
#define	CBMDATACONVERTER_H

#include <iostream>

#include "CbmUnpacker.h"
#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"
#include "CbmDetectorList.h"
#include "CbmMicroSlice.h"
#include "CbmStsDigi.h"
#include "CbmMuchDigi.h"
#include "FairMQLogger.h"
class CbmDataConverter 
{
public:
    CbmDataConverter();
    //CbmDataConverter(const CbmDataConverter& orig);
    virtual ~CbmDataConverter();
    void Convert(uint8_t DetectorId, const uint8_t* FlesTimeSliceContent);
    void Convert(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    
    void Convert(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice);
    
protected:
    void StsConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice);
    
    vector<CbmStsDigi>  StsConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void StsConverter(const uint8_t* FlesTimeSliceContent);
    void MuchConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void MuchConverter(const uint8_t* FlesTimeSliceContent);
    CbmMicroSlice GetCbmMicroSlice(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    
    CbmTimeSlice* fCBMTimeSlice;
    //unsigned int fStsMSDigiSize;
    //unsigned int fMuchMSDigiSize;

};

#endif	/* CBMDATACONVERTER_H */

