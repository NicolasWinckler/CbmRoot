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

void CbmDataConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
}

