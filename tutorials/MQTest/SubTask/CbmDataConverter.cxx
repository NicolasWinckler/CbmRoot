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
        fStsConverter(new CbmStsDataConverter()),
        fMuchConverter(new CbmMuchDataConverter()),
        fCurrentStartTime (0.),
        fDuration (1000.),
        fPrint(false),
        fDigiToPrint(0)
{
        fStsDigiPayloadSize=fStsConverter->GetDigiPayloadSize();
        fMuchDigiPayloadSize=fMuchConverter->GetDigiPayloadSize();
}


CbmDataConverter::~CbmDataConverter() 
{
    if(fCBMTimeSlice)
        delete fCBMTimeSlice;
    
    if(fMuchConverter)
        delete fMuchConverter;
    if(fStsConverter)
        delete fStsConverter;
    
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
            fStsConverter->StsCbmTSFiller(MSdesc, FlesTimeSliceContent,fCBMTimeSlice);
          break;
        case kMUCH:
            fMuchConverter->MuchCbmTSFiller(MSdesc, FlesTimeSliceContent,fCBMTimeSlice);
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
            fStsConverter->StsConverter(&desc, MSlice->GetData().data());
          break;
        case kMUCH:
            fMuchConverter->MuchConverter(&desc, MSlice->GetData().data());
          break;
        default:
          break;
        }
    }
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
        MSdesc.offset=0;
        vector<CbmStsDigi> StsData=CbmTSlice->GetStsData();
        MicroSlice=fStsConverter->GetCbmStsMicroSlice(&MSdesc, StsData);

      break;
    }
    case kMUCH:
    {
        bool fakeMuchDigi=true;
        if(fPrint)
        {
            std::cout   << "   number of MUCH points = "
                        << CbmTSlice->GetDataSize(kMUCH) <<std::endl;
            if(fakeMuchDigi)
                std::cout   << "   Warning: dummy MUCH digi created for test"
                            <<std::endl;
        }

        // define fles MicroSlice header specific to MUCH detector
        MSdesc.sys_ver = 1;
        MSdesc.eq_id = (uint16_t)(kMUCH);// input link id (component)
        MSdesc.sys_id = (uint16_t)(kMUCH);// detector id
        //MSdesc.size = (uint32_t)(fMuchDigiPayloadSize*CbmTSlice->GetDataSize(kMUCH));
        uint32_t NMuchPoints;
        if(fakeMuchDigi)
            NMuchPoints=100;
        else
            NMuchPoints=(uint32_t)CbmTSlice->GetDataSize(kMUCH);
        
        MSdesc.size = (uint32_t)(fMuchDigiPayloadSize)*NMuchPoints;
        MSdesc.offset=0;
        // loop below temporary
        vector<CbmMuchDigi> MuchData;
        if(fakeMuchDigi)
        {
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
        }
        else 
        {
            MuchData=CbmTSlice->GetMuchData();
        }
        MicroSlice=fMuchConverter->GetCbmMuchMicroSlice(&MSdesc, MuchData);

        
      break;
    }
    default:
      break;
    }
    

    return MicroSlice;
}


void CbmDataConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
    fStsConverter->SetPrintOption(fDigiToPrint,fPrint);
    fMuchConverter->SetPrintOption(fDigiToPrint,fPrint);
}

