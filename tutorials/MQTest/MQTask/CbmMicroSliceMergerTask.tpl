/* 
 * File:   CbmMicroSliceMergerTask.tpl
 * Author: winckler
 *
 * Created on May 22, 2014, 9:08 PM
 */


template <typename TPayloadIn, typename TPayloadOut>
CbmMicroSliceMergerTask<TPayloadIn, TPayloadOut>::CbmMicroSliceMergerTask() :
        fMaxMicroSliceNumber(0),
        fTimeSliceIndex(0),
        fMaxComponentNumber(0),
        fMicroSliceIndex(0),
        fComponentIndex(0),
        fTSReady(false)
{
    
}




template <typename TPayloadIn, typename TPayloadOut> 
CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::~CbmMicroSliceMergerTask()
{
        
}




template <typename TPayloadIn, typename TPayloadOut> 
InitStatus CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::Init()
{
        
        // temporary : component are input link and not detector id, but for the moment 
        // we use it here as component = detector id
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=0 -> empty
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=1 -> empty
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=2 -> sts
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=3 -> empty
        fFlesTimeSlices.append_component(fMaxMicroSliceNumber, fTimeSliceIndex);// component c=4 -> empty
        
        
        
        // detector list in CbmDetectorList.h
        //kREF,0      // Reference plane  
        //kMVD,1      // Micro-Vertex Detector
        //kSTS,2      // Silicon Tracking System
        //kRICH,3     // Ring-Imaging Cherenkov Detector
        //kMUCH,4     // Muon detetcion system
        //kTRD,5      // Transition Radiation Detector
        //kTOF,      // Time-of-flight Detector
        //kECAL,     // EM-Calorimeter
        //kPSD,      // Projectile spectator detector
        //kSTT,      // Straw Tube Tracker (obsolete)
        //kFHODO,    // Fibre Hodoskope (for beam tests)
        //kTutDet,   // Dummy for tutorials
        //kNOFDETS // Number of elements (e.g. for loops)
        
        return kSUCCESS;
}


template <typename TPayloadIn, typename TPayloadOut> 
void InitFlesTimeSlice(uint64_t num_microslices, uint64_t index = UINT64_MAX)
{
    
}

template <typename TPayloadIn, typename TPayloadOut> 
void CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::Exec(FairMQMessage* msg, Option_t* opt)
{ 
        
        fTSReady=false;
        //prepare boost input archive
        std::string msgStr( static_cast<char*>(msg->GetData()), msg->GetSize() );
        std::istringstream ibuffer(msgStr);
        TPayloadIn InputArchive(ibuffer);
        try
        {
             InputArchive >> fDigiVector;// get input Archive
        }
        catch (boost::archive::archive_exception e)
        {
             LOG(ERROR) << e.what();
        }

        
        LOG(INFO) << "---------------------------------------";
        
        fles::MicrosliceDescriptor desc;
        std::vector<uint8_t> MSliceData;
        uint64_t index=0;
        uint16_t eqid=0;// component (input link)
        uint32_t ContentSize=0;
        // Merge Microslices into timeslices
        for (unsigned int i = 0; i < fDigiVector.size(); ++i) 
        {            
            CbmMicroSlice MSlice=fDigiVector[i];
            desc=MSlice.GetHeader();
            index=desc.idx;
            eqid=desc.eq_id;
            ContentSize=desc.size;
            MSliceData=MSlice.GetData();
            LOG(INFO) << "Micro Slice Index = "<<index;
            LOG(INFO) << "Input link = "<<eqid;
            LOG(INFO) << "Content size = "<<ContentSize;
            fFlesTimeSlices.append_microslice(eqid, index, desc, MSliceData.data());
            
        }
        
        
        
        LOG(INFO) << "MSliceData.size() = "<<MSliceData.size();
        
        /// temporary : to check whether we get data 
        bool printdata=true;
        if(printdata && MSliceData.size()>0)
        {
            
            std::vector<uint8_t> vectTimestamp_sts;     // ULong64_t
            std::vector<uint8_t> vectAdress_sts;        // UInt_t
            std::vector<uint8_t> vectCharge_sts;        // UShort_t
            std::vector<uint8_t> vectSectorNr_sts;      // Int_t
            std::vector<uint8_t> vectSystemId_sts;      // Int_t
            
            /// get data of first digi of current microslice
            
            // time stamp
            unsigned int start=0;
            unsigned int end=sizeof(ULong64_t);
            for (unsigned int i = start; i < end; ++i)
                vectTimestamp_sts.push_back(MSliceData.at(i));
            
            // adress
            start=end;
            end+=sizeof(UInt_t);
            for (unsigned int i = start; i < end; ++i)
                vectAdress_sts.push_back(MSliceData.at(i));
            
            // charge
            start=end;
            end+=sizeof(UShort_t);
            for (unsigned int i = start; i < end; ++i)
                vectCharge_sts.push_back(MSliceData.at(i));
            
            // sectorNr
            start=end;
            end+=sizeof(Int_t);
            for (unsigned int i = start; i < end; ++i)
                vectSectorNr_sts.push_back(MSliceData.at(i));
            
            // systemId
            start=end;
            end+=sizeof(Int_t);
            for (unsigned int i = start; i < end; ++i)
                vectSystemId_sts.push_back(MSliceData.at(i));
            
            
            ULong64_t tempTimestamp=CombineData<ULong64_t>(vectTimestamp_sts);
            UInt_t tempAdress=CombineData<UInt_t>(vectAdress_sts);
            UShort_t tempCharge=CombineData<UShort_t>(vectCharge_sts);
            Int_t tempSectorNr=CombineData<Int_t>(vectSectorNr_sts);
            Int_t tempSystemId=CombineData<Int_t>(vectSystemId_sts);
            
            LOG(INFO) << "*****************";
            LOG(INFO) << "* Data of first digi in current microslice:";
            LOG(INFO) << "* TimeStamp = " << tempTimestamp <<" ns";
            LOG(INFO) << "* Address = "   << tempAdress;
            LOG(INFO) << "* Charge = "    << tempCharge;
            LOG(INFO) << "* SectorNr = "  << tempSectorNr;
            LOG(INFO) << "* SystemId = "  << tempSystemId;
            LOG(INFO) << "*****************";
            
        }
            

        LOG(INFO) << "* SystemId = "  << index;
        if(index==fMaxMicroSliceNumber-1)
            fTSReady=true;
        else 
            fTSReady=false;
        
        
        if(fTSReady)
        {
            LOG(INFO) <<"Send time slice to sink";
            //prepare boost output archive
            std::ostringstream obuffer;
            TPayloadOut OutputArchive(obuffer); 
            OutputArchive << fFlesTimeSlices;
            int outputSize=obuffer.str().length();
            msg->Rebuild(outputSize);
            std::memcpy(msg->GetData(), obuffer.str().c_str(), outputSize);
        }
    
        
}





template <typename TPayloadIn, typename TPayloadOut> 
bool CbmMicroSliceMergerTask<TPayloadIn,TPayloadOut>::MsgReadyToSend()
{
    return fTSReady;
}



















