
/* 
 * File:   CbmMicroSliceMerger.h
 * Author: winckler
 *
 * Created on May 22, 2014, 5:06 PM
 */

#ifndef CBMMICROSLICEMERGER_H
#define	CBMMICROSLICEMERGER_H
#include "FairMQDevice.h"
#include "StorableTimeslice.hpp"
#include "CbmMicroSlice.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "MQLogger.h"


template <typename TPolicyTask>
class CbmMicroSliceMerger: public FairMQDevice 
{
public:
    
    //using CbmProcessor<TPolicyTask>::fProcessorTask;
    
    
    CbmMicroSliceMerger(); 
    virtual ~CbmMicroSliceMerger();
    void SetTask(TPolicyTask* task);
    
    
    
    void SetMicroSliceNum(uint64_t MSNumber)
    {
        fProcessorTask->SetMicroSliceNumber(MSNumber);
    }
    
    void SetTimeSliceIdx(uint64_t TSIndex)
    {
        fProcessorTask->SetTimeSliceIndex(TSIndex);
    }
    

protected:
    
    virtual void Init();
    virtual void Run();
    uint64_t fTSIndex;
    TPolicyTask* fProcessorTask; 
    
    fles::StorableTimeslice fFlesTimeSlices{1, 1};
       
};

#include "CbmMicroSliceMerger.tpl"

#endif	/* CBMMICROSLICEMERGER_H */

