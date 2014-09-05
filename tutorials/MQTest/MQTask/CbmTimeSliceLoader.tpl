/* 
 * File:   CbmTimeSliceLoader.tpl
 * @since 2014-02-08
 * @author: N. Winckler
 *
 */

////////// Base template class <TCbmDigi,TPayloadOut>



template <typename TCbmDigi, typename TPayloadOut> 
CbmTimeSliceLoader<TCbmDigi,TPayloadOut>::CbmTimeSliceLoader() : FairMQSamplerTask("Load class TCbmDigi"),
fDataConverterTask(new CbmDataConverterTask()), fLoopCounter(0), fNfakedigi(0)
{
    TCbmDigi Digi;
    fDetID=(DetectorId)Digi.GetSystemId();
}


template <typename TCbmDigi, typename TPayloadOut> 
CbmTimeSliceLoader<TCbmDigi,TPayloadOut>::~CbmTimeSliceLoader() 
{
    if(fDigiVector.size()>0) 
        fDigiVector.clear();
    
    if(fDataConverterTask)
    {
        delete fDataConverterTask;
        fDataConverterTask=NULL;
    }
}
//*
template <typename TCbmDigi, typename TPayloadOut> 
InitStatus CbmTimeSliceLoader<TCbmDigi,TPayloadOut>::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    //fCBMTimeSlice = reinterpret_cast<CbmTimeSlice*>(ioman->GetObject(fBranch.c_str())) ;
    stringstream(fBranch) >> fNfakedigi;
    fCBMTimeSlice = reinterpret_cast<CbmTimeSlice*>(ioman->GetObject("TimeSlice."));
    fDataConverterTask->SetNfakeDigi(fNfakedigi);
    
    return kSUCCESS;
}


template <typename TCbmDigi, typename TPayloadOut> 
void CbmTimeSliceLoader<TCbmDigi,TPayloadOut>::Exec(Option_t* opt) 
{         
        //hack to make much time working
        //fLoopCounter=fEventIndex;
        bool printinfo=false;
        if (printinfo)
        {
            MQLOG(INFO)<< "------------------------------------------";
            MQLOG(INFO)<<"Event Index = "<< fEventIndex ;
            MQLOG(INFO)<<"fLoopCounter  = "<< fLoopCounter ;
            fDataConverterTask->SetPrintOption(0,printinfo);
        }
        //if(fLoopCounter==30) while(true){};
        fDataConverterTask->SetCurrentSamplerLoopIndex(fLoopCounter);
        fLoopCounter++;
        /// convert data : CbmRoot -> Fles
        fMicroSlice=fDataConverterTask->BuildMicroSlice(fDetID,fCBMTimeSlice);
        fDigiVector.push_back(fMicroSlice);

        
        
        /// boost serialization and create FairMQ message
        std::ostringstream buffer;
        TPayloadOut OutputArchive(buffer);
        OutputArchive << fDigiVector;
        int size=buffer.str().length();
        fOutput = fTransportFactory->CreateMessage(size);
        std::memcpy(fOutput->GetData(), buffer.str().c_str(), size);
        
        // delete the vector content
        if(fDigiVector.size()>0) 
            fDigiVector.clear();
    
}


