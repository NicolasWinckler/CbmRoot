/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/

/* 
 * File:   CbmDataConverter.cxx
 * Author: winckler
 * 
 * Created on July 18, 2014, 7:23 PM
 */

#include "CbmDataConverter.h"

CbmDataConverter::CbmDataConverter() : FairTask("CbmDataConverter"),
        fOutFile(NULL),
        fTree(NULL),
        fCBMTimeSlice(NULL),
        fCurrentStartTime (0.),
        fDuration (1000.),
        fPrint(false),
        fDigiToPrint(0)
{
        fStsDigiPayloadSize=sizeof(ULong64_t)+sizeof(UInt_t)+sizeof(UShort_t)+sizeof(Int_t)+sizeof(Int_t);
        fMuchDigiPayloadSize=sizeof(Int_t)+sizeof(Int_t)+sizeof(Int_t)+sizeof(Int_t);// TODO : implement MUCH part
}


CbmDataConverter::~CbmDataConverter() 
{
    delete fCBMTimeSlice;
}

/*
void Exec(Option_t* opt)
{

}
// /
InitStatus Init()
{

 return kSUCCESS;
}
// */

void CbmDataConverter::InitCbmTS(Double_t start, Double_t duration)
{
    fCurrentStartTime=start;
    fDuration=duration;
    if(!fCBMTimeSlice)
        fCBMTimeSlice = new CbmTimeSlice(fCurrentStartTime,fDuration);
    else
        fCBMTimeSlice->Reset(fCurrentStartTime, fDuration);
}

void CbmDataConverter::InitCbmTSOutputFile(const char* filename)
{
    
    fOutFile = new TFile(filename,"recreate");
    if(fCBMTimeSlice)
    {
        fTree = new TTree("CbmMQOut", "Test output");
        fTree->Branch("TimeSlice.","CbmTimeSlice", &fCBMTimeSlice, 64000, 99);
    }
    else
    {
        fCBMTimeSlice = new CbmTimeSlice(fCurrentStartTime,fDuration);
        fTree = new TTree("CbmMQOut", "Test output");
        fTree->Branch("TimeSlice.","CbmTimeSlice", &fCBMTimeSlice, 64000, 99);
    }
    
}

void CbmDataConverter::SetCbmTSInterval(Double_t start, Double_t duration)
{
    fCurrentStartTime=start;
    fDuration=duration;
    fCBMTimeSlice->Reset(fCurrentStartTime, fDuration);
    
}

void CbmDataConverter::FillCbmTSTree() 
{
  fTree->Fill();
  // --- Reset time slice with new time interval
  fCurrentStartTime += fDuration;
  fCBMTimeSlice->Reset(fCurrentStartTime, fDuration);

}


void CbmDataConverter::WriteTreeToFile()
{
    fCBMTimeSlice->Reset(0.,0.);
    fTree->Write();
    fOutFile->Close();
}



void CbmDataConverter::CbmTSFiller(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent)
{
    uint8_t DetectorId=MSdesc->sys_id;
    uint32_t ContentSize=MSdesc->size;
    if(ContentSize>0)
    {
        switch (DetectorId) 
        {

        case kSTS:
            StsCbmTSFiller(MSdesc, FlesTimeSliceContent);
          break;
        case kMUCH:
            MuchCbmTSFiller(MSdesc, FlesTimeSliceContent);
          break;
        default:
          break;
        }
    }
}


void CbmDataConverter::PrintCbmMicroSlice(const CbmMicroSlice* MSlice, uint32_t DigiToPrint)
{
    SetPrintOption(DigiToPrint);
    //std::vector<uint8_t> MSliceData=MSlice->GetData();
    const fles::MicrosliceDescriptor desc=MSlice->GetHeader();
    uint8_t DetectorId=MSlice->GetHeader().sys_id;
    uint32_t ContentSize=MSlice->GetHeader().size;
    //uint8_t DetectorId=MSdesc->sys_id;
    //uint32_t ContentSize=MSdesc->size;
    if(ContentSize>0)
    {
        switch (DetectorId) 
        {

        case kSTS:
            StsConverter(&desc, MSlice->GetData().data());
          break;
        case kMUCH:
            MuchConverter(&desc, MSlice->GetData().data());
          break;
        default:
          break;
        }
    }
}






void CbmDataConverter::StsCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint64_t MSliceIndex=MSdesc->idx;
    uint32_t start=0;
    uint32_t end=0;
    uint32_t iDigi=0;
    while(end<ContentSize)
    {
        std::vector<uint8_t> vTimestamp_sts;     // ULong64_t
        std::vector<uint8_t> vAdress_sts;        // UInt_t
        std::vector<uint8_t> vCharge_sts;        // UShort_t
        std::vector<uint8_t> vSectorNr_sts;      // Int_t
        std::vector<uint8_t> vSystemId_sts;      // Int_t

        /// get data of first digi of current microslice

        // time stamp
        start=end;
        end+=sizeof(ULong64_t);
        for (uint32_t k = start; k < end; ++k)
            vTimestamp_sts.push_back(*(FlesTimeSliceContent+k));

        // adress
        start=end;
        end+=sizeof(UInt_t);
        for (uint32_t k = start; k < end; ++k)
            vAdress_sts.push_back(*(FlesTimeSliceContent+k));

        // charge
        start=end;
        end+=sizeof(UShort_t);
        for (uint32_t k = start; k < end; ++k)
            vCharge_sts.push_back(*(FlesTimeSliceContent+k));

        // sectorNr
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vSectorNr_sts.push_back(*(FlesTimeSliceContent+k));

        // systemId
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vSystemId_sts.push_back(*(FlesTimeSliceContent+k));

        /// reconvert and fill digi in time slices
        ULong64_t Digi_Timestamp=CombineData<ULong64_t>(vTimestamp_sts);
        UInt_t Digi_Adress=CombineData<UInt_t>(vAdress_sts);
        UShort_t Digi_Charge=CombineData<UShort_t>(vCharge_sts);
        Int_t Digi_SectorNr=CombineData<Int_t>(vSectorNr_sts);
        Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_sts);
        CbmStsDigi StsDigi(Digi_Adress, Digi_Timestamp, Digi_Charge, Digi_SectorNr);
        fCBMTimeSlice->InsertData(&StsDigi);
        
        if(fPrint && iDigi==fDigiToPrint)
        {
            
            LOG(INFO) << "*****************";
            LOG(INFO) << "* Header in current microslice: ";
            LOG(INFO) << "* SystemId = "  << Digi_SystemId;
            LOG(INFO) << "* MS index = "   << MSliceIndex;
            LOG(INFO) << "* Content size = "   << ContentSize;
            LOG(INFO) << "* Data of first digi in current microslice: ";
            LOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
            LOG(INFO) << "* Address = "   << Digi_Adress;
            LOG(INFO) << "* Charge = "    << Digi_Charge;
            LOG(INFO) << "* SectorNr = "  << Digi_SectorNr;
            LOG(INFO) << "*****************";
            
        }
        iDigi++;
    }
    
}



void CbmDataConverter::MuchCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint64_t MSliceIndex=MSdesc->idx;
    uint32_t start=0;
    uint32_t end=0;
    uint32_t iDigi=0;
    while(end<ContentSize)
    {
        std::vector<uint8_t> vTimestamp_much;     // ULong64_t
        std::vector<uint8_t> vAdress_much;        // UInt_t
        std::vector<uint8_t> vCharge_much;        // UShort_t
        std::vector<uint8_t> vSystemId_much;      // Int_t

        /// get data of first digi of current microslice

        // time stamp
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vTimestamp_much.push_back(*(FlesTimeSliceContent+k));

        // adress
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vAdress_much.push_back(*(FlesTimeSliceContent+k));

        // charge
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vCharge_much.push_back(*(FlesTimeSliceContent+k));

        // systemId
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vSystemId_much.push_back(*(FlesTimeSliceContent+k));

        /// reconvert and fill digi in time slices
        Int_t Digi_Timestamp=CombineData<Int_t>(vTimestamp_much);
        Int_t Digi_Adress=CombineData<Int_t>(vAdress_much);
        Int_t Digi_Charge=CombineData<Int_t>(vCharge_much);
        Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_much);
        CbmMuchDigi MuchDigi(Digi_Adress, Digi_Charge,  Digi_Timestamp);
        fCBMTimeSlice->InsertData(&MuchDigi);
        
        if(fPrint && iDigi==fDigiToPrint)
        {
            
            LOG(INFO) << "*****************";
            LOG(INFO) << "* Header in current microslice: ";
            LOG(INFO) << "* SystemId = "  << Digi_SystemId;
            LOG(INFO) << "* MS index = "   << MSliceIndex;
            LOG(INFO) << "* Content size = "   << ContentSize;
            LOG(INFO) << "* Data of first digi in current microslice: ";
            LOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
            LOG(INFO) << "* Address = "   << Digi_Adress;
            LOG(INFO) << "* Charge = "    << Digi_Charge;
            LOG(INFO) << "*****************";
            
        }
        iDigi++;
    }
    
}



vector<CbmStsDigi>  CbmDataConverter::StsConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint64_t MSliceIndex=MSdesc->idx;
    uint32_t start=0;
    uint32_t end=0;
    uint32_t iDigi=0;
    vector<CbmStsDigi> StsData;
    while(end<ContentSize)
    {
        std::vector<uint8_t> vTimestamp_sts;     // ULong64_t
        std::vector<uint8_t> vAdress_sts;        // UInt_t
        std::vector<uint8_t> vCharge_sts;        // UShort_t
        std::vector<uint8_t> vSectorNr_sts;      // Int_t
        std::vector<uint8_t> vSystemId_sts;      // Int_t

        /// get data of first digi of current microslice

        // time stamp
        start=end;
        end+=sizeof(ULong64_t);
        for (uint32_t k = start; k < end; ++k)
            vTimestamp_sts.push_back(*(FlesTimeSliceContent+k));

        // adress
        start=end;
        end+=sizeof(UInt_t);
        for (uint32_t k = start; k < end; ++k)
            vAdress_sts.push_back(*(FlesTimeSliceContent+k));

        // charge
        start=end;
        end+=sizeof(UShort_t);
        for (uint32_t k = start; k < end; ++k)
            vCharge_sts.push_back(*(FlesTimeSliceContent+k));

        // sectorNr
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vSectorNr_sts.push_back(*(FlesTimeSliceContent+k));

        // systemId
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vSystemId_sts.push_back(*(FlesTimeSliceContent+k));


        ULong64_t Digi_Timestamp=CombineData<ULong64_t>(vTimestamp_sts);
        UInt_t Digi_Adress=CombineData<UInt_t>(vAdress_sts);
        UShort_t Digi_Charge=CombineData<UShort_t>(vCharge_sts);
        Int_t Digi_SectorNr=CombineData<Int_t>(vSectorNr_sts);
        Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_sts);
        CbmStsDigi StsDigi(Digi_Adress, Digi_Timestamp, Digi_Charge, Digi_SectorNr);
        StsData.push_back(StsDigi);



        if(fPrint && iDigi==fDigiToPrint)
        {
            
            LOG(INFO) << "*****************";
            LOG(INFO) << "* Header in current microslice: ";
            LOG(INFO) << "* SystemId = "  << Digi_SystemId;
            LOG(INFO) << "* MS index = "   << MSliceIndex;
            LOG(INFO) << "* Content size = "   << ContentSize;
            LOG(INFO) << "* Data of first digi in current microslice: ";
            LOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
            LOG(INFO) << "* Address = "   << Digi_Adress;
            LOG(INFO) << "* Charge = "    << Digi_Charge;
            LOG(INFO) << "* SectorNr = "  << Digi_SectorNr;
            LOG(INFO) << "*****************";
            
        }
        iDigi++;

    }
    
    return StsData;
}






vector<CbmMuchDigi> CbmDataConverter::MuchConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint64_t MSliceIndex=MSdesc->idx;
    uint32_t start=0;
    uint32_t end=0;
    uint32_t iDigi=0;
    vector<CbmMuchDigi> MuchData;
    while(end<ContentSize)
    {
        std::vector<uint8_t> vTimestamp_much;     // ULong64_t
        std::vector<uint8_t> vAdress_much;        // UInt_t
        std::vector<uint8_t> vCharge_much;        // UShort_t
        std::vector<uint8_t> vSystemId_much;      // Int_t

        /// get data of first digi of current microslice

        // time stamp
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vTimestamp_much.push_back(*(FlesTimeSliceContent+k));

        // adress
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vAdress_much.push_back(*(FlesTimeSliceContent+k));

        // charge
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vCharge_much.push_back(*(FlesTimeSliceContent+k));

        // systemId
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vSystemId_much.push_back(*(FlesTimeSliceContent+k));

        /// reconvert and fill digi in time slices
        Int_t Digi_Timestamp=CombineData<Int_t>(vTimestamp_much);
        Int_t Digi_Adress=CombineData<Int_t>(vAdress_much);
        Int_t Digi_Charge=CombineData<Int_t>(vCharge_much);
        Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_much);
        CbmMuchDigi MuchDigi(Digi_Adress, Digi_Charge,  Digi_Timestamp);
        MuchData.push_back(MuchDigi);
        
        if(fPrint && iDigi==fDigiToPrint)
        {
            
            LOG(INFO) << "*****************";
            LOG(INFO) << "* Header in current microslice: ";
            LOG(INFO) << "* SystemId = "  << Digi_SystemId;
            LOG(INFO) << "* MS index = "   << MSliceIndex;
            LOG(INFO) << "* Content size = "   << ContentSize;
            LOG(INFO) << "* Data of first digi in current microslice: ";
            LOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
            LOG(INFO) << "* Address = "   << Digi_Adress;
            LOG(INFO) << "* Charge = "    << Digi_Charge;
            LOG(INFO) << "*****************";
            
        }
        iDigi++;
    }
    return MuchData;
}




CbmMicroSlice CbmDataConverter::GetCbmMicroSlice(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent)
{
    CbmMicroSlice MicroSlice;
    vector<uint8_t> MicroSliceContent;
    fles::MicrosliceDescriptor MS_header = *MSdesc;
    uint32_t ContentSize=MS_header.size;
    
    for(uint32_t k=0; k<ContentSize;k++)
    {
        MicroSliceContent.push_back(*(FlesTimeSliceContent+k));
    }
    
    MicroSlice.SetHeader(MS_header);
    MicroSlice.SetData(MicroSliceContent);
    
    return MicroSlice;
}

CbmMicroSlice CbmDataConverter::GetCbmMicroSlice(DetectorId iDet, CbmTimeSlice* CbmTSlice)
{
    CbmMicroSlice MicroSlice;

    if(fPrint)
    {
        
        std::cout<< "Time interval in ns [" 
                    << CbmTSlice->GetStartTime() << ", "
                    << CbmTSlice->GetEndTime() << "]"
                    << std::endl;
    }
    
    // initialize fles MicroSlice header common to all detectors
    fles::MicrosliceDescriptor MSdesc = fles::MicrosliceDescriptor();
    MSdesc.hdr_id = 0xDD;// From Jan's example
    MSdesc.hdr_ver = 0x01;
    
    MSdesc.idx=(uint64_t)(CbmTSlice->GetStartTime()/1000.0);// assume 1mus interval

    switch (iDet) 
    {

    case kSTS:
    {
        if(fPrint)
        {
            std::cout   << "   number of STS points = "
                        << CbmTSlice->GetDataSize(kSTS) <<std::endl;
        }

        // define fles MicroSlice header specific to STS detector
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kSTS);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kSTS);// detector id
        MSdesc.size = (uint32_t)(fStsDigiPayloadSize*CbmTSlice->GetDataSize(kSTS));
        vector<CbmStsDigi> StsData=CbmTSlice->GetStsData();
        MicroSlice=GetCbmStsMicroSlice(&MSdesc, StsData);

      break;
    }
    case kMUCH:
    {
        if(fPrint)
        {
            std::cout   << "   number of MUCH points = "
                        << CbmTSlice->GetDataSize(kMUCH) <<std::endl;
        }

        // define fles MicroSlice header specific to MUCH detector
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kMUCH);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kMUCH);// detector id
        //MSdesc.size = (uint32_t)(fMuchDigiPayloadSize*CbmTSlice->GetDataSize(kMUCH));
        uint32_t NMuchPoints=100;
        MSdesc.size = (uint32_t)(fMuchDigiPayloadSize)*NMuchPoints;
        //vector<CbmMuchDigi> MuchData=CbmTSlice->GetMuchData();
        //MicroSlice=GetCbmMuchMicroSlice(&MSdesc, MuchData);

        // loop below temporary
        vector<CbmMuchDigi> MuchData;
        for(uint32_t iMuchDigi=0;iMuchDigi<NMuchPoints;iMuchDigi++)
        {
            Int_t Muchaddress=(Int_t)(rand() % 100 + 1);
            Int_t Muchcharge=(Int_t)(rand() % 100 + 1);
            Int_t Muchtime=(Int_t)(CbmTSlice->GetStartTime()+(Double_t)(rand() % 100 + 1));
            CbmMuchDigi muchdigi(Muchaddress, Muchcharge, Muchtime);
            //std::cout<<"Muchtime= " << Muchtime<<std::endl;
            //std::cout<<"Muchcharge= " << Muchcharge<<std::endl;
            //std::cout<<"Muchaddress= " << Muchaddress<<std::endl;

            MuchData.push_back(muchdigi);

        }
        MicroSlice=GetCbmMuchMicroSlice(&MSdesc, MuchData);

        
      break;
    }
    default:
      break;
    }
    

    return MicroSlice;
}

CbmMicroSlice CbmDataConverter::GetCbmStsMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmStsDigi> vStsDigi)
{

    CbmMicroSlice MicroSlice;
    vector<uint8_t> MicroSliceContent;
    //fles::MicrosliceDescriptor desc_sts = *MSdesc;

    // loop over sts data
    for(unsigned int iDigi = 0; iDigi < vStsDigi.size(); iDigi++) 
    {
        
        // split individual data member of CbmStsDigi into pieces of N bytes and store them into vector
        vector<uint8_t> StsData_TimeStamp=SplitData<ULong64_t>(vStsDigi[iDigi].GetTime());
        vector<uint8_t> StsData_Address=SplitData<UInt_t>(vStsDigi[iDigi].GetAddress());
        vector<uint8_t> StsData_Charge=SplitData<UShort_t>(vStsDigi[iDigi].GetCharge());
        vector<uint8_t> StsData_SectorNr=SplitData<Int_t>(vStsDigi[iDigi].GetSectorNr());
        vector<uint8_t> StsData_SystemId= SplitData<Int_t>(vStsDigi[iDigi].GetSystemId());
        
        // print 
        if(fPrint && iDigi==fDigiToPrint)
        {
            // recombine the splitted data to check whether everything is ok
            std::cout   << "*******************************************"<< std::endl;
            std::cout   << "* Content of STS digi Nr "      << iDigi <<" in CbmRoot Time Slice:" << std::endl;
            std::cout   << "* System Id = "      << vStsDigi[iDigi].GetSystemId()   << std::endl;
            std::cout   << "* time stamp =  "    << vStsDigi[iDigi].GetTime()       << " ns" <<std::endl;
            std::cout   << "* Address = "        << vStsDigi[iDigi].GetAddress()    << std::endl;
            std::cout   << "* Charge = "         << vStsDigi[iDigi].GetCharge()     << std::endl;
            std::cout   << "* Sector Nr = "      << vStsDigi[iDigi].GetSectorNr()   << std::endl;
            std::cout   << "*******************************************"<< std::endl;
        }
        
        // fill Micro slice content 
        // Note:  "start time" and "duration" data members 
        //are implicitely given by the microslice index idx -> interval[idx-1,idx] mus
       
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_TimeStamp.begin(), StsData_TimeStamp.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_Address.begin(), StsData_Address.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_Charge.begin(), StsData_Charge.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_SectorNr.begin(), StsData_SectorNr.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                StsData_SystemId.begin(), StsData_SystemId.end());
        
        
    }
    
    if(MicroSliceContent.size()!=MSdesc->size)
    {
        LOG(ERROR)<<"MicroSlice content does not match with expected CbmRoot Time slice content";
        MSdesc->size = MicroSliceContent.size();
    }

    MicroSlice.SetHeader(*MSdesc);
    MicroSlice.SetData(MicroSliceContent);

    return MicroSlice;
}



CbmMicroSlice CbmDataConverter::GetCbmMuchMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmMuchDigi> vMuchDigi)
{

    CbmMicroSlice MicroSlice;
    vector<uint8_t> MicroSliceContent;

    // loop over Much data
    for(unsigned int iDigi = 0; iDigi < vMuchDigi.size(); iDigi++) 
    {
        
        // split individual data member of CbmMuchDigi into pieces of N bytes and store them into vector
        vector<uint8_t> MuchData_TimeStamp=SplitData<Int_t>(vMuchDigi[iDigi].GetTime());
        vector<uint8_t> MuchData_Address=SplitData<Int_t>(vMuchDigi[iDigi].GetAddress());
        vector<uint8_t> MuchData_Charge=SplitData<Int_t>(vMuchDigi[iDigi].GetAdc());
        vector<uint8_t> MuchData_SystemId= SplitData<Int_t>(vMuchDigi[iDigi].GetSystemId());
        
        // print 
        if(fPrint && iDigi==fDigiToPrint)
        {
            // recombine the splitted data to check whether everything is ok
            std::cout   << "*******************************************"<< std::endl;
            std::cout   << "* System Id = "      << vMuchDigi[iDigi].GetSystemId()   << std::endl;
            std::cout   << "* Content of MUCH digi Nr "      << iDigi <<" in CbmRoot Time Slice:" << std::endl;
            std::cout   << "* time stamp =  "    << vMuchDigi[iDigi].GetTime()       << " ns" <<std::endl;
            std::cout   << "* Address = "        << vMuchDigi[iDigi].GetAddress()    << std::endl;
            std::cout   << "* Charge = "         << vMuchDigi[iDigi].GetAdc()     << std::endl;
            std::cout   << "*******************************************"<< std::endl;
        }
        
        // fill Micro slice content 
        // Note:  "start time" and "duration" data members 
        //are implicitely given by the microslice index idx -> interval[idx-1,idx] mus
       
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_TimeStamp.begin(), MuchData_TimeStamp.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_Address.begin(), MuchData_Address.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_Charge.begin(), MuchData_Charge.end());
        
        MicroSliceContent.insert(MicroSliceContent.end(), 
                MuchData_SystemId.begin(), MuchData_SystemId.end());
        
        
    }
    
    if(MicroSliceContent.size()!=MSdesc->size)
    {
        LOG(ERROR)<<"MicroSlice content "<< MicroSliceContent.size() 
        << " does not match with expected CbmRoot Time slice content : "<<MSdesc->size;
        MSdesc->size = MicroSliceContent.size();
    }

    MicroSlice.SetHeader(*MSdesc);
    MicroSlice.SetData(MicroSliceContent);

    return MicroSlice;
}





void CbmDataConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
}

