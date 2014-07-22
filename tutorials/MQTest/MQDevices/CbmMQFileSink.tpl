/* 
 * File:   CbmMQFileSink.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */




template <typename TPayloadIn>
CbmMQFileSink<TPayloadIn>::CbmMQFileSink() : fStsUnpacker(new CbmDataConverter())
{
        
    
}


template <typename TPayloadIn>
CbmMQFileSink<TPayloadIn>::~CbmMQFileSink()
{
    /*
    fTree->Write();
    fOutFile->Close();

    */
}

/*
template <typename TPayloadIn>
void CbmMQFileSink<TPayloadIn>::Init()
{
    //fStsUnpacker = new CbmStsUnpacker();

}
//*/

template <typename TPayloadIn>
void CbmMQFileSink<TPayloadIn>::InitOutputFile(TString defaultId)
{
    /*
    fOutput = new TClonesArray("FairTestDetectorHit");
    char out[256];
    sprintf(out, "filesink%s.root", defaultId.Data());

    fOutFile = new TFile(out,"recreate");
    fTree = new TTree("MQOut", "Test output");
    fTree->Branch("Output","TClonesArray", &fOutput, 64000, 99);*/
}



template <typename TPayloadIn>
void CbmMQFileSink<TPayloadIn>::Run()
{
  
        LOG(INFO) << ">>>>>>> Run <<<<<<<";

        boost::thread rateLogger(boost::bind(&FairMQDevice::LogSocketRates, this));
        int receivedMsgs = 0;
        size_t bytes_received = 0;


        while ( fState == RUNNING ) 
        {
            FairMQMessage* msg = fTransportFactory->CreateMessage();
            bytes_received = fPayloadInputs->at(0)->Receive(msg);
            
            if (bytes_received) 
            {
                receivedMsgs++;
                std::string msgStr( static_cast<char*>(msg->GetData()), msg->GetSize() );
                std::istringstream ibuffer(msgStr);
                TPayloadIn InputArchive(ibuffer);

                try
                {
                    InputArchive >> fFlesTimeSlices;
                }
                catch (boost::archive::archive_exception& e)
                {
                    LOG(ERROR) << e.what();
                }

                LOG(INFO) << "Message received";
                
                
                 
                
                uint64_t  InputLinkNumber=fFlesTimeSlices.num_components();
                uint64_t  MSlicesNumber_MAX=0;
                
                // In order to fill the root cbm TimeSlice we first get the maximum index (time) 
                // among all components of the fles TimeSlice
                for(unsigned int comp_j = 0; comp_j < InputLinkNumber; ++comp_j)
                {
                    uint64_t MSnum_MAX=fFlesTimeSlices.num_microslices(comp_j);
                    if(MSnum_MAX>MSlicesNumber_MAX) 
                            MSlicesNumber_MAX=MSnum_MAX;
                }
                
                
                
                LOG(INFO) << "InputLinkNumber = "<<InputLinkNumber;
                LOG(INFO) << "MSlicesNumber_MAX = "<<MSlicesNumber_MAX;
                
                //loop over fles MS index (time)
                for(unsigned int i = 0; i < MSlicesNumber_MAX; ++i)
                {
                    //CbmTimeSlice* fTimeSlice = new CbmTimeSlice(fCurrentStartTime, fDuration);
                    
                    for(unsigned int comp_j = 0; comp_j < InputLinkNumber; ++comp_j)
                    {
                        uint64_t  MSlicesNumber_j=fFlesTimeSlices.num_microslices(comp_j);
                        
                        
                        if(i<MSlicesNumber_j)
                        {

                            
                            const fles::MicrosliceDescriptor MSdesc=fFlesTimeSlices.descriptor(comp_j,i);
                            uint64_t MSliceIndex=MSdesc.idx-1;
                            uint16_t eqid=MSdesc.eq_id;
                            uint32_t ContentSize=MSdesc.size;
                            uint8_t Det_id=MSdesc.sys_id;
                            Det_id=2;
                            if(ContentSize>0 && eqid==2)
                            {
                                
                                const uint8_t* ptr_FlesTimeSliceContent;
                                ptr_FlesTimeSliceContent=fFlesTimeSlices.content(2,i);
                                //fStsUnpacker->Convert(Det_id,ptr_FlesTimeSliceContent);
                                fStsUnpacker->Convert(&MSdesc,ptr_FlesTimeSliceContent);
                            }
                        }

                    }//loop k
                }
                
                bytes_received = 0;
            }
            delete msg;
            
        }
        
        cout << "I've received " << receivedMsgs << " messages!" << endl;
        boost::this_thread::sleep(boost::posix_time::milliseconds(5000));
        rateLogger.interrupt();
        rateLogger.join();
    
    
}



