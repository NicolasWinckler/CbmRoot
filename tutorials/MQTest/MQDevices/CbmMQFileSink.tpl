/* 
 * File:   CbmMQFileSink.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */




template <typename TPayloadIn>
CbmMQFileSink<TPayloadIn>::CbmMQFileSink()
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
                catch (boost::archive::archive_exception e)
                {
                    LOG(ERROR) << e.what();
                }

                LOG(INFO) << "Message received";
                
                for(unsigned int i = 0; i < 16; ++i)
                {
                    LOG(INFO) << "---------------------------------------";
                    const fles::MicrosliceDescriptor MSdesc=fFlesTimeSlices.descriptor(2,i);
                    uint64_t MSliceIndex=MSdesc.idx-1;
                    uint16_t eqid=MSdesc.eq_id;
                    uint32_t ContentSize=MSdesc.size;
                    
                    LOG(INFO) << "Micro Slice Index = "<<MSliceIndex;
                    LOG(INFO) << "Input link = "<<eqid;
                    LOG(INFO) << "Content size = "<<ContentSize;
                    
                    if(ContentSize>0)
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
                        for (unsigned int k = start; k < end; ++k)
                            vectTimestamp_sts.push_back(*(fFlesTimeSlices.content(2,i)+k));

                        // adress
                        start=end;
                        end+=sizeof(UInt_t);
                        for (unsigned int k = start; k < end; ++k)
                            vectAdress_sts.push_back(*(fFlesTimeSlices.content(2,i)+k));

                        // charge
                        start=end;
                        end+=sizeof(UShort_t);
                        for (unsigned int k = start; k < end; ++k)
                            vectCharge_sts.push_back(*(fFlesTimeSlices.content(2,i)+k));

                        // sectorNr
                        start=end;
                        end+=sizeof(Int_t);
                        for (unsigned int k = start; k < end; ++k)
                            vectSectorNr_sts.push_back(*(fFlesTimeSlices.content(2,i)+k));

                        // systemId
                        start=end;
                        end+=sizeof(Int_t);
                        for (unsigned int k = start; k < end; ++k)
                            vectSystemId_sts.push_back(*(fFlesTimeSlices.content(2,i)+k));

                        
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



