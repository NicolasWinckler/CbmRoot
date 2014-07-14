/* 
 * File:   CbmMicroSliceMerger.h
 * Author: winckler
 *
 * Created on May 22, 2014, 5:06 PM
 */

#ifndef CBMMICROSLICEMERGER_H
#define	CBMMICROSLICEMERGER_H
#include "CbmProcessor.h"

template <typename TPolicyTask>
class CbmMicroSliceMerger: public CbmProcessor<TPolicyTask>
{
public:
    
    using CbmProcessor<TPolicyTask>::fProcessorTask;
    
    CbmMicroSliceMerger() : CbmProcessor<TPolicyTask>(){}
    virtual ~CbmMicroSliceMerger(){}
    void SetMicroSliceNum(uint64_t MSNumber)
    {
        fProcessorTask->SetMicroSliceNumber(MSNumber);
    }
    
    void SetTimeSliceIdx(uint64_t TSIndex)
    {
        fProcessorTask->SetTimeSliceIndex(TSIndex);
    }
    
    
};

#endif	/* CBMMICROSLICEMERGER_H */

