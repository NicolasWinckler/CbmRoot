/* 
 * File:   CbmStsDataConverter.h
 * Author: winckler
 *
 * Created on July 30, 2014, 11:44 AM
 */

#ifndef CBMSTSDATACONVERTER_H
#define	CBMSTSDATACONVERTER_H

#include "CbmDigiConverter.h"
#include "CbmStsDigi.h"

class CbmStsDataConverter : public CbmDigiConverter
{
public:
    CbmStsDataConverter();
    virtual ~CbmStsDataConverter();
    
    CbmMicroSlice GetCbmStsMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmStsDigi> vStsDigi);
    vector<CbmStsDigi>  StsConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void  StsCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice);

    //virtual void PrintInfo(const fles::MicrosliceDescriptor* MSdesc, const CbmDigi* Digi, uint32_t iDigi);
    
    virtual void PrintDigiContent(const CbmDigi* Digi);
    size_t GetDigiPayloadSize(){return fStsDigiPayloadSize;}
    
protected:
    CbmDigi* ConvertFlesPtrToDigi(uint32_t *offset, const uint8_t* FlesTimeSliceContent);

private:
    
    size_t fStsDigiPayloadSize;
};

#endif	/* CBMSTSDATACONVERTER_H */

