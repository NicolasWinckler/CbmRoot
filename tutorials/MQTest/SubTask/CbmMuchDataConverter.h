/* 
 * File:   CbmMuchDataConverter.h
 * Author: winckler
 *
 * Created on July 30, 2014, 11:44 AM
 */

#ifndef CBMMUCHDATACONVERTER_H
#define	CBMMUCHDATACONVERTER_H

#include "CbmDigiConverter.h"
#include "CbmMuchDigi.h"


class CbmMuchDataConverter : public CbmDigiConverter
{
public:
    CbmMuchDataConverter();
    virtual ~CbmMuchDataConverter();
    
    vector<CbmMuchDigi> MuchConverter(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent);
    void  MuchCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice);
    CbmMicroSlice GetCbmMuchMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmMuchDigi> vMuchDigi);

    
    virtual void PrintDigiContent(const CbmDigi* Digi);
    size_t GetDigiPayloadSize(){return fMuchDigiPayloadSize;}
protected:
    virtual std::shared_ptr<CbmDigi> ConvertFlesPtrToDigi(uint32_t *offset, const uint8_t* FlesTimeSliceContent);

private:
    size_t fMuchDigiPayloadSize;
};

#endif	/* CBMMUCHDATACONVERTER_H */

