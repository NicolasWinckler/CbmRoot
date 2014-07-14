/* 
 * File:   CbmMicroSliceMergerTask.h
 * Author: winckler
 *
 * Created on May 22, 2014, 9:00 PM
 */

#ifndef CBMMICROSLICEMERGERTASK_H
#define	CBMMICROSLICEMERGERTASK_H

#include <iostream>
#include <sstream>

#ifndef __CINT__ // Boost serialization 
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif //__CINT__

#include "FairMQProcessorTask.h"
#include "FairMQLogger.h"

#include "StorableTimeslice.hpp"
#include "CbmMicroSlice.h"

template <typename TPayloadIn, typename TPayloadOut>
class CbmMicroSliceMergerTask : public FairMQProcessorTask
{
public:
    CbmMicroSliceMergerTask();
    virtual ~CbmMicroSliceMergerTask();
    
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(FairMQMessage* msg, Option_t* opt);
    
    bool TSReady();
    
    void SetMicroSliceNumber(uint64_t MaxMicroSliceNumber)
    {fMaxMicroSliceNumber=MaxMicroSliceNumber;}
    
    void SetComponentNumber(unsigned int MaxComponentNumber)
        {fMaxComponentNumber=MaxComponentNumber;}
    
    uint64_t GetMaxMicroSliceNumber()
        {return fMaxMicroSliceNumber;}
    unsigned int GetMaxComponentNumber()
        {return fMaxComponentNumber;}
    unsigned int GetMicroSliceIndex()
        {return fMicroSliceIndex;}
    unsigned int GetComponentIndex()
        {return fComponentIndex;}
    
    
    void InitFlesTimeSlice(uint64_t num_microslices, uint64_t index = UINT64_MAX);
    
    
    
    // boost serialize function
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fDigiVector;
        ar & fFlesTimeSlices;
    }
    
    
    // split data member of type U1 in N pieces of 1 byte
    template<typename U1>
    std::vector<uint8_t> SplitData(U1 DataToSplit)
    {
        U1 Input=DataToSplit;
        std::size_t InputTypeSize=sizeof(U1);
        uint8_t *Data_ptr = (uint8_t*)&Input;
        
        std::vector<uint8_t> output;
        for(unsigned int i=0;i<InputTypeSize;i++)
        {
            output.push_back(Data_ptr[i]);
        }
    
        return output;
    }
    
    // reconstruct data
    template<typename U2>
    U2 CombineData(std::vector<uint8_t> input)
    {
        
        assert( sizeof(U2)==input.size());
        
        uint64_t combinedData=0;
        for(unsigned int i=0;i<input.size();i++)
        {
            uint64_t temp=0;
            temp=(uint64_t)input[i];
            if(i>0)
                temp<<=8*i;
            combinedData|=temp;
            
            
        }
        
        U2 output=(U2)combinedData;
        return output;
    }
    
    
    
private:

    uint64_t fMaxMicroSliceNumber;
    uint64_t fTimeSliceIndex;
    uint64_t fMaxComponentNumber;
    unsigned int fMicroSliceIndex;
    unsigned int fComponentIndex;
    bool fTSReady;
    std::vector<CbmMicroSlice> fDigiVector;
    #ifndef __CINT__ 
    friend class boost::serialization::access;
    fles::StorableTimeslice fFlesTimeSlices{1, 1};
    #endif
};


#include "CbmMicroSliceMergerTask.tpl"

#endif	/* CBMMICROSLICEMERGERTASK_H */

