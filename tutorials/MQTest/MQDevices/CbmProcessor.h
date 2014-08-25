/* 
 * File:   CbmProcessor.h
 * Author: winckler
 *
 * Created on July 16, 2014, 10:48 AM
 */

#ifndef CBMPROCESSOR_H
#define	CBMPROCESSOR_H

#include "FairMQDevice.h"
#include "StorableTimeslice.hpp"
#include "CbmMicroSlice.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "FairMQLogger.h"

template <typename TPolicyTask>
class CbmProcessor: public FairMQDevice 
{
public:
    CbmProcessor();
    virtual ~CbmProcessor();
    void SetTask(TPolicyTask* task);
    
    
protected:

    virtual void Init();
    virtual void Run();
    fles::StorableTimeslice fFlesTimeSlices{1, 1};
    TPolicyTask* fProcessorTask;
    
};
#include "CbmProcessor.tpl"

#endif	/* CBMPROCESSOR_H */

