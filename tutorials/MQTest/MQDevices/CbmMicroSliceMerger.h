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
#include "FairMQProcessorTask.h"
#include "CbmMicroSliceMergerTask.h"


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>



typedef boost::archive::binary_iarchive TBoostBinPayloadIn; // boost binary format
typedef boost::archive::text_iarchive TBoostTextPayloadIn; // boost text format
typedef boost::archive::binary_oarchive TBoostBinPayloadOut; // boost binary format
typedef boost::archive::text_oarchive TBoostTextPayloadOut; // boost text format
typedef CbmMicroSliceMergerTask<TBoostBinPayloadIn, TBoostBinPayloadOut> TProcessorTask;



class CbmMicroSliceMerger: public FairMQDevice 
{
public:
    CbmMicroSliceMerger();
    CbmMicroSliceMerger(unsigned int ComponentNumber);
    virtual ~CbmMicroSliceMerger();
    void SetTask(TProcessorTask* task);
    
protected:

    virtual void Init();
    virtual void Run();
    fles::StorableTimeslice fFlesTimeSlices{1, 1};
    unsigned int fComponentNumber;

private:
    //FairMQProcessorTask* fProcessorTask;
    TProcessorTask* fProcessorTask;
    
};

#endif	/* CBMMICROSLICEMERGER_H */

