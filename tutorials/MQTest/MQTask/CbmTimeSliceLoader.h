
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
#include "CbmUnpacker.h"

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

