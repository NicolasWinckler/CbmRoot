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
            LOG(INFO) << "* MS index = "   << MSliceIndex;
            LOG(INFO) << "* Content size = "   << ContentSize;
            LOG(INFO) << "* Data of first digi in current microslice: ";
            LOG(INFO) << "* TimeStamp = " << Digi_Timestamp <<" ns";
            LOG(INFO) << "* Address = "   << Digi_Adress;
            LOG(INFO) << "* Charge = "    << Digi_Charge;
            LOG(INFO) << "* SectorNr = "  << Digi_SectorNr;
            LOG(INFO) << "* SystemId = "  << Digi_SystemId;
            LOG(INFO) << "*****************";
            
        }
        iDigi++;
    }
    
}

vector<CbmStsDigi>  CbmDataConverter::StsConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t start=0;
    uint32_t end=0;
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
        //Int_t Digi_SystemId=CombineData<Int_t>(vSystemId_sts);
        CbmStsDigi StsDigi(Digi_Adress, Digi_Timestamp, Digi_Charge, Digi_SectorNr);
        StsData.push_back(StsDigi);
    }
    
    return StsData;
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
                        << CbmTSlice->GetDataSize(kSTS) 
                        <<std::endl;
                        int truc=sizeof(CbmStsDigi)*CbmTSlice->GetDataSize(kSTS);
                        std::cout<<"SIZE OF sts digi = "<< sizeof(CbmStsDigi) <<std::endl;
                        std::cout<<"SIZE OF CONTENT = "<< truc <<std::endl;
        }

        // define fles MicroSlice header specific to sts detector
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kSTS);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kSTS);// detector id
        //MSdesc.size -> define in GetCbmStsMicroSlice
        vector<CbmStsDigi> StsData=CbmTSlice->GetStsData();
        MicroSlice=GetCbmStsMicroSlice(&MSdesc, StsData);

      break;
    }
    case kMUCH:
    {
        if(fPrint)
            {
                std::cout   << "   number of MUCH points = "
                            << CbmTSlice->GetDataSize(kMUCH) 
                            <<std::endl;
            }
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kMUCH);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kMUCH);// detector id
        vector<CbmMuchDigi> MuchData=CbmTSlice->GetMuchData();
        //MSdesc.size -> define in GetCbmStsMicroSlice
        //MicroSlice=GetCbmMuchMicroSlice(&MSdesc, MuchData);
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
            ULong64_t testmystuff=CombineData<ULong64_t>(StsData_TimeStamp);
            std::cout   << "testmystuff = "     << testmystuff <<std::endl;
            std::cout   << "STS digi Nr "       << iDigi << std::endl;
            std::cout   << " time stamp =  "    << vStsDigi[iDigi].GetTime() 
                        << " Address = "        << vStsDigi[iDigi].GetAddress() 
                        << " Charge = "         << vStsDigi[iDigi].GetCharge()
                        << " Sector Nr = "      << vStsDigi[iDigi].GetSectorNr()
                        << " System Id = "      << vStsDigi[iDigi].GetSystemId()
                        << std::endl;
            
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
    
    // initialize microslice header
    //fles::MicrosliceDescriptor desc_sts = fles::MicrosliceDescriptor();
    //desc_sts=fdesc;
    //desc_sts.eq_id = (uint16_t)(kSTS);// input link id (component)
    //desc_sts.sys_id = (uint16_t)(kSTS);// detector id
    //desc_sts.idx = (uint64_t)(fEventIndex);// Microslice index => interval [0,1]mus 
    //desc_sts.size = MicroSliceContent.size();
    MSdesc->size = MicroSliceContent.size();

    //MicroSlice.SetHeader(desc_sts);
    MicroSlice.SetHeader(*MSdesc);
    MicroSlice.SetData(MicroSliceContent);

    return MicroSlice;
}








void CbmDataConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
}

