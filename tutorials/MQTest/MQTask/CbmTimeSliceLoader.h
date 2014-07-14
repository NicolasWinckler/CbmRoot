
#ifndef CBMTIMESLICELOADER_H
#define	CBMTIMESLICELOADER_H

#include "FairMQSamplerTask.h"

#include "FairMQLogger.h"
#include <boost/timer/timer.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <iostream> 
#include <iterator>
#include <algorithm>
#include <cassert>
#include "CbmTimeSlice.h"
#include "CbmStsDigi.h"
#include "CbmMuchDigi.h"

#include "StorableTimeslice.hpp"
#include "CbmMicroSlice.h"

#if __cplusplus >= 201103L
#include <type_traits>
#include <array>

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////


////////// Base template header <T1,T2>
template <typename T1, typename T2>
class CbmTimeSliceLoader : public FairMQSamplerTask
{ 
public : 
 
    CbmTimeSliceLoader();
    virtual ~CbmTimeSliceLoader();
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fDigiVector;
    }
    
    template<typename U1>
    vector<uint8_t> SplitData(U1 DataToSplit)
    {
        U1 Input=DataToSplit;
        std::size_t InputTypeSize=sizeof(U1);
        uint8_t *Data_ptr = (uint8_t*)&Input;
        
        vector<uint8_t> output;
        for(unsigned int i=0;i<InputTypeSize;i++)
        {
            output.push_back(Data_ptr[i]);
        }
    
        return output;
    }
    
    template<typename U2>
    U2 CombineData(vector<uint8_t> input)
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
    
private :
    CbmTimeSlice* fCBMTimeSlice;
    friend class boost::serialization::access;
    vector<T1> fDigiVector;
    vector<CbmStsDigi> fStsData;
    vector<CbmMuchDigi> fMuchData;
    
    CbmMicroSlice fStsMicroSlice;
    fles::MicrosliceDescriptor fdesc;
    vector<uint8_t> fMicroSliceContent;
}; 





////////// Template implementation is in CbmTimeSliceLoader.tpl :
#include "CbmTimeSliceLoader.tpl"

#endif /* CBMTIMESLICELOADER_H */

