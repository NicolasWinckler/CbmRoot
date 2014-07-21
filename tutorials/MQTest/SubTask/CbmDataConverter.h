/* 
 * File:   CbmDataConverter.h
 * Author: winckler
 *
 * Created on July 18, 2014, 7:23 PM
 */

#ifndef CBMDATACONVERTER_H
#define	CBMDATACONVERTER_H

#include <iostream>
#include "FairMQLogger.h"
#include "CbmUnpacker.h"
#include "StorableTimeslice.hpp"
#include "CbmDetectorList.h"
#include "CbmDigi.h"

class CbmDataConverter 
{
public:
    CbmDataConverter();
    //CbmDataConverter(const CbmDataConverter& orig);
    virtual ~CbmDataConverter();
    void Convert(uint8_t DetectorId, const uint8_t* FlesTimeSliceContent);
    
protected:
    void StsConverter(const uint8_t* FlesTimeSliceContent);
    void MuchConverter(const uint8_t* FlesTimeSliceContent);

};

#endif	/* CBMDATACONVERTER_H */

