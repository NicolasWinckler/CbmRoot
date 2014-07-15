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


#include "FairMQLogger.h"

template <typename TPolicyTask>
class CbmMicroSliceMerger: public FairMQDevice 
{
public:
    CbmMicroSliceMerger();
    CbmMicroSliceMerger(unsigned int ComponentNumber);
    virtual ~CbmMicroSliceMerger();
    void SetTask(TPolicyTask* task);
    void SetMicroSliceNum(uint64_t MSNumber);
    void SetTimeSliceIdx(uint64_t TSIndex);
    
protected:

    virtual void Init();
    virtual void Run();
    fles::StorableTimeslice fFlesTimeSlices{1, 1};
    unsigned int fComponentNumber;

private:
    //FairMQProcessorTask* fProcessorTask;
    TPolicyTask* fProcessorTask;
    
};


#include "CbmMicroSliceMerger.tpl"

#endif	/* CBMMICROSLICEMERGER_H */

