/**
 * CbmMQFileSink.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef FAIRMQFILESINK_H_
#define FAIRMQFILESINK_H_

#include <iostream>

#include "Rtypes.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"

#include "FairMQDevice.h"
#include "FairMQLogger.h"


#include "StorableTimeslice.hpp"


#ifndef __CINT__
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#endif //__CINT__


class TVector3;
class TFile;
class TTree;
class TClonesArray;

template <typename TPayloadIn>
class CbmMQFileSink: public FairMQDevice
{
  public:
    CbmMQFileSink();
    virtual ~CbmMQFileSink();
    virtual void InitOutputFile(TString defaultId = "100");
    
    
    
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
    
    
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & fFlesTimeSlices;
    }
    
  protected:
    virtual void Run();
  private:
    TFile* fOutFile;
    TTree* fTree;
    TClonesArray* fOutput;
    #ifndef __CINT__ // for BOOST serialization
    friend class boost::serialization::access;
    fles::StorableTimeslice fFlesTimeSlices{1,1};
    #endif // for BOOST serialization
};


////////// Template implementation of Run() in CbmMQFileSink.tpl :
#include "CbmMQFileSink.tpl"

#endif /* FAIRMQFILESINK_H_ */
