/* 
 * File:   CbmTimeSliceLoader.tpl
 * @since 2014-02-08
 * @author: N. Winckler
 *
 */

////////// Base template class <T1,T2>



template <typename T1, typename T2> 
CbmTimeSliceLoader<T1,T2>::CbmTimeSliceLoader() : FairMQSamplerTask("Load class T1")
{
    

    // From Jan's example
    fdesc = fles::MicrosliceDescriptor();
    fdesc.hdr_id = 0xDD;
    fdesc.hdr_ver = 0x01;
    fdesc.sys_id = 0xFF;
    fdesc.sys_ver = 1;
    
    
}


template <typename T1, typename T2> 
CbmTimeSliceLoader<T1,T2>::~CbmTimeSliceLoader() 
{
    if(fDigiVector.size()>0) 
        fDigiVector.clear();
}
//*
template <typename T1, typename T2> 
InitStatus CbmTimeSliceLoader<T1,T2>::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    fCBMTimeSlice = reinterpret_cast<CbmTimeSlice*>(ioman->GetObject(fBranch.c_str())) ;

    return kSUCCESS;
}


template <typename T1, typename T2> 
void CbmTimeSliceLoader<T1,T2>::Exec(Option_t* opt) 
{         
        std::ostringstream buffer;
        T2 OutputArchive(buffer);
            //*
        
        std::cout<<"Event Index = "<< fEventIndex <<std::endl;
        // get start time and endtime(=startime + duration)
        std::cout<< "Time interval [" 
                << fCBMTimeSlice->GetStartTime() << ", "
                << fCBMTimeSlice->GetEndTime() << "]"
                << std::endl;
        
        printf("   number of STS points = %i\n",fCBMTimeSlice->GetDataSize(kSTS));
        printf("   number of MUCH points = %i\n",fCBMTimeSlice->GetDataSize(kMUCH));
        
         
        std::cout<< "------------------------------------------"<< std::endl;
        
        bool printStsInfo=false;
        
        
        
        fStsData=fCBMTimeSlice->GetStsData();
        fMuchData=fCBMTimeSlice->GetMuchData();
        
        
        // loop over sts data
        for(unsigned int iDigi = 0; iDigi < fStsData.size(); iDigi++) 
        {
            
            // split individual data member of CbmStsDigi into pieces of N bytes and store them into vector
            vector<uint8_t> StsU8BData_TimeStamp=SplitData<ULong64_t>(fStsData[iDigi].GetTime());
            vector<uint8_t> StsU8BData_Address=SplitData<UInt_t>(fStsData[iDigi].GetAddress());
            vector<uint8_t> StsU8BData_Charge=SplitData<UShort_t>(fStsData[iDigi].GetCharge());
            vector<uint8_t> StsU8BData_SectorNr=SplitData<Int_t>(fStsData[iDigi].GetSectorNr());
            vector<uint8_t> StsU8BData_SystemId= SplitData<Int_t>(fStsData[iDigi].GetSystemId());
            
            // print 
            if(printStsInfo)
            {
               
                // recombine the splitted data to check whether everything is ok
                ULong64_t testmystuff=CombineData<ULong64_t>(StsU8BData_TimeStamp);
                std::cout   << "testmystuff = "     << testmystuff <<std::endl;
                std::cout   << "STS digi Nr "       << iDigi << std::endl;
                std::cout   << " time stamp =  "    << fStsData[iDigi].GetTime() 
                            << " Address = "        << fStsData[iDigi].GetAddress() 
                            << " Charge = "         << fStsData[iDigi].GetCharge()
                            << " Sector Nr = "      << fStsData[iDigi].GetSectorNr()
                            << " System Id = "      << fStsData[iDigi].GetSystemId()
                            << std::endl;
                
            }
            
            // fill Micro slice content 
            // Note:  "start time" and "duration" data members 
            //are implicitely given by the microslice index idx -> interval[idx-1,idx] mus
            
           
            fMicroSliceContent.insert(fMicroSliceContent.end(), 
                    StsU8BData_TimeStamp.begin(), StsU8BData_TimeStamp.end());
            
            fMicroSliceContent.insert(fMicroSliceContent.end(), 
                    StsU8BData_Address.begin(), StsU8BData_Address.end());
            
            fMicroSliceContent.insert(fMicroSliceContent.end(), 
                    StsU8BData_Charge.begin(), StsU8BData_Charge.end());
            
            fMicroSliceContent.insert(fMicroSliceContent.end(), 
                    StsU8BData_SectorNr.begin(), StsU8BData_SectorNr.end());
            
            fMicroSliceContent.insert(fMicroSliceContent.end(), 
                    StsU8BData_SystemId.begin(), StsU8BData_SystemId.end());
            
            
        }
        
        // initialize microslice header
        fles::MicrosliceDescriptor desc_sts = fles::MicrosliceDescriptor();
        desc_sts=fdesc;
        desc_sts.eq_id = (uint16_t)(kSTS);// e.g. sts?
        desc_sts.idx = (uint64_t)(fEventIndex+1);// Microslice index => interval [0,1]mus ?
        desc_sts.size = fMicroSliceContent.size();
        
        fStsMicroSlice.SetHeader(desc_sts);
        fStsMicroSlice.SetData(fMicroSliceContent);
        fDigiVector.push_back(fStsMicroSlice);
        //fMicroSliceContent
        OutputArchive << fDigiVector;
        int size=buffer.str().length();
        fOutput = fTransportFactory->CreateMessage(size);
        std::memcpy(fOutput->GetData(), buffer.str().c_str(), size);

        
        // delete the vector content
        if(fDigiVector.size()>0) 
            fDigiVector.clear();
    
    fStsData.clear();
    fMuchData.clear();
    fMicroSliceContent.clear();
}
