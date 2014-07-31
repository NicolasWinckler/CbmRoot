/* 
 * File:   CbmDigiConverter.h
 * Author: winckler
 *
 * Created on July 30, 2014, 6:36 PM
 */

#ifndef CBMDIGICONVERTER_H
#define	CBMDIGICONVERTER_H


#include <iostream>
#include <iostream>

#include "StorableTimeslice.hpp"
#include "CbmTimeSlice.h"

#include "CbmDigiConverter.h"
#include "CbmUnpacker.h"
#include "CbmMicroSlice.h"

#include "CbmDetectorList.h"

#include "MQLogger.h"



class CbmDigiConverter 
{
public:
    CbmDigiConverter();
    virtual ~CbmDigiConverter();
    
    void SetPrintOption(uint32_t DigiToPrint, bool print);
    void PrintFlesMicroSliceHeader(const fles::MicrosliceDescriptor* MSdesc);
    
    
protected:
    virtual CbmDigi* ConvertFlesPtrToDigi2(uint32_t *offset, const uint8_t* FlesTimeSliceContent)=0;
    bool fPrint;
    uint32_t fDigiToPrint;
};

#endif	/* CBMDIGICONVERTER_H */

