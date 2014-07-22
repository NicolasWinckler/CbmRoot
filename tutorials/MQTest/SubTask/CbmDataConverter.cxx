/* 
 * File:   CbmDataConverter.cpp
 * Author: winckler
 * 
 * Created on July 18, 2014, 7:23 PM
 */

#include "CbmDataConverter.h"

CbmDataConverter::CbmDataConverter() 
{
    fCBMTimeSlice=NULL;
    //fStsMSDigiSize=sizeof(ULong64_t)+sizeof(UInt_t)+sizeof(UShort_t)+sizeof(Int_t)+sizeof(Int_t);
    //fMuchMSDigiSize=0;
}

//CbmDataConverter::CbmDataConverter(const CbmDataConverter& orig) {
//}

CbmDataConverter::~CbmDataConverter() {
}


void CbmDataConverter::Convert(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice)
{
    uint8_t DetectorId=MSdesc->sys_id;
    uint32_t ContentSize=MSdesc->size;
    if(ContentSize>0)
    {
        switch (DetectorId) 
        {

        case kSTS:
          //if ( index < fStsData.size() ) digi = &(fStsData[index]);
            StsConverter(MSdesc, FlesTimeSliceContent,CbmRootTimeSlice);
          break;
        case kMUCH:
          //if ( index < fMuchData.size() ) digi = &(fMuchData[index]);
          break;
        default:
          break;
        }
    }
}
    


void CbmDataConverter::Convert(const fles::MicrosliceDescriptor* MSdesc, const uint8_t* FlesTimeSliceContent)
{
    //uint32_t ContentSize=MSdesc->size;
    uint8_t DetectorId=MSdesc->sys_id;
    
    switch (DetectorId) 
    {

    case kSTS:
      //if ( index < fStsData.size() ) digi = &(fStsData[index]);
        StsConverter(MSdesc, FlesTimeSliceContent);
      break;
    case kMUCH:
      //if ( index < fMuchData.size() ) digi = &(fMuchData[index]);
      break;
    default:
      break;
    }
    
}



void CbmDataConverter::Convert(uint8_t DetectorId, const uint8_t* FlesTimeSliceContent)
{
    
    switch (DetectorId) 
    {

    case kSTS:
      //if ( index < fStsData.size() ) digi = &(fStsData[index]);
        StsConverter(FlesTimeSliceContent);
      break;
    case kMUCH:
      //if ( index < fMuchData.size() ) digi = &(fMuchData[index]);
      break;
    default:
      break;
    }
    
}





void CbmDataConverter::StsConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t start=0;
    uint32_t end=0;
    vector<CbmStsDigi> StsData;
    while(start<ContentSize)
    {
        std::vector<uint8_t> vectTimestamp_sts;     // ULong64_t
        std::vector<uint8_t> vectAdress_sts;        // UInt_t
        std::vector<uint8_t> vectCharge_sts;        // UShort_t
        std::vector<uint8_t> vectSectorNr_sts;      // Int_t
        std::vector<uint8_t> vectSystemId_sts;      // Int_t

        /// get data of first digi of current microslice

        // time stamp
        start=end;
        end+=sizeof(ULong64_t);
        for (uint32_t k = start; k < end; ++k)
            vectTimestamp_sts.push_back(*(FlesTimeSliceContent+k));

        // adress
        start=end;
        end+=sizeof(UInt_t);
        for (uint32_t k = start; k < end; ++k)
            vectAdress_sts.push_back(*(FlesTimeSliceContent+k));

        // charge
        start=end;
        end+=sizeof(UShort_t);
        for (uint32_t k = start; k < end; ++k)
            vectCharge_sts.push_back(*(FlesTimeSliceContent+k));

        // sectorNr
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vectSectorNr_sts.push_back(*(FlesTimeSliceContent+k));

        // systemId
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vectSystemId_sts.push_back(*(FlesTimeSliceContent+k));


        ULong64_t tempTimestamp=CombineData<ULong64_t>(vectTimestamp_sts);
        UInt_t tempAdress=CombineData<UInt_t>(vectAdress_sts);
        UShort_t tempCharge=CombineData<UShort_t>(vectCharge_sts);
        Int_t tempSectorNr=CombineData<Int_t>(vectSectorNr_sts);
        Int_t tempSystemId=CombineData<Int_t>(vectSystemId_sts);
        CbmStsDigi StsDigi(tempAdress, tempTimestamp, tempCharge, tempSectorNr);
        //StsData.push_back(StsDigi);
        CbmRootTimeSlice->InsertData(&StsDigi);
         
    }
    
    /*
    LOG(INFO) << "*****************";
    LOG(INFO) << "* Data of first digi in current microslice: ";
    LOG(INFO) << "* TimeStamp = " << tempTimestamp <<" ns";
    LOG(INFO) << "* Address = "   << tempAdress;
    LOG(INFO) << "* Charge = "    << tempCharge;
    LOG(INFO) << "* SectorNr = "  << tempSectorNr;
    LOG(INFO) << "* SystemId = "  << tempSystemId;
    LOG(INFO) << "*****************";
    */
}



vector<CbmStsDigi>  CbmDataConverter::StsConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
{
    uint32_t ContentSize=MSdesc->size;
    uint32_t start=0;
    uint32_t end=0;
    vector<CbmStsDigi> StsData;
    int ii=0;
    while(start<ContentSize)
    {
        std::vector<uint8_t> vectTimestamp_sts;     // ULong64_t
        std::vector<uint8_t> vectAdress_sts;        // UInt_t
        std::vector<uint8_t> vectCharge_sts;        // UShort_t
        std::vector<uint8_t> vectSectorNr_sts;      // Int_t
        std::vector<uint8_t> vectSystemId_sts;      // Int_t

        /// get data of first digi of current microslice

        // time stamp
        start=end;
        end+=sizeof(ULong64_t);
        for (uint32_t k = start; k < end; ++k)
            vectTimestamp_sts.push_back(*(FlesTimeSliceContent+k));

        // adress
        start=end;
        end+=sizeof(UInt_t);
        for (uint32_t k = start; k < end; ++k)
            vectAdress_sts.push_back(*(FlesTimeSliceContent+k));

        // charge
        start=end;
        end+=sizeof(UShort_t);
        for (uint32_t k = start; k < end; ++k)
            vectCharge_sts.push_back(*(FlesTimeSliceContent+k));

        // sectorNr
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vectSectorNr_sts.push_back(*(FlesTimeSliceContent+k));

        // systemId
        start=end;
        end+=sizeof(Int_t);
        for (uint32_t k = start; k < end; ++k)
            vectSystemId_sts.push_back(*(FlesTimeSliceContent+k));


        ULong64_t tempTimestamp=CombineData<ULong64_t>(vectTimestamp_sts);
        UInt_t tempAdress=CombineData<UInt_t>(vectAdress_sts);
        UShort_t tempCharge=CombineData<UShort_t>(vectCharge_sts);
        Int_t tempSectorNr=CombineData<Int_t>(vectSectorNr_sts);
        Int_t tempSystemId=CombineData<Int_t>(vectSystemId_sts);
        CbmStsDigi StsDigi(tempAdress, tempTimestamp, tempCharge, tempSectorNr);
        StsData.push_back(StsDigi);

        //*
        ii++;
        if(ii==1)
        {
            
            LOG(INFO) << "*****************";
            LOG(INFO) << "* Data of first digi in current microslice: ";
            LOG(INFO) << "* TimeStamp = " << tempTimestamp <<" ns";
            LOG(INFO) << "* Address = "   << tempAdress;
            LOG(INFO) << "* Charge = "    << tempCharge;
            LOG(INFO) << "* SectorNr = "  << tempSectorNr;
            LOG(INFO) << "* SystemId = "  << tempSystemId;
            LOG(INFO) << "*****************";
            
        }
        //*/
    }
    
    
    return StsData;
}

void CbmDataConverter::StsConverter(const uint8_t* FlesTimeSliceContent)
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
        vectTimestamp_sts.push_back(*(FlesTimeSliceContent+k));

    // adress
    start=end;
    end+=sizeof(UInt_t);
    for (unsigned int k = start; k < end; ++k)
        vectAdress_sts.push_back(*(FlesTimeSliceContent+k));

    // charge
    start=end;
    end+=sizeof(UShort_t);
    for (unsigned int k = start; k < end; ++k)
        vectCharge_sts.push_back(*(FlesTimeSliceContent+k));

    // sectorNr
    start=end;
    end+=sizeof(Int_t);
    for (unsigned int k = start; k < end; ++k)
        vectSectorNr_sts.push_back(*(FlesTimeSliceContent+k));

    // systemId
    start=end;
    end+=sizeof(Int_t);
    for (unsigned int k = start; k < end; ++k)
        vectSystemId_sts.push_back(*(FlesTimeSliceContent+k));


    ULong64_t tempTimestamp=CombineData<ULong64_t>(vectTimestamp_sts);
    UInt_t tempAdress=CombineData<UInt_t>(vectAdress_sts);
    UShort_t tempCharge=CombineData<UShort_t>(vectCharge_sts);
    Int_t tempSectorNr=CombineData<Int_t>(vectSectorNr_sts);
    Int_t tempSystemId=CombineData<Int_t>(vectSystemId_sts);
    //CbmStsDigi StsDigi(tempAdress, tempTimestamp, tempCharge, tempSectorNr);
    
     LOG(INFO) << "*****************";
    LOG(INFO) << "* Data of first digi in current microslice: ";
    LOG(INFO) << "* TimeStamp = " << tempTimestamp <<" ns";
    LOG(INFO) << "* Address = "   << tempAdress;
    LOG(INFO) << "* Charge = "    << tempCharge;
    LOG(INFO) << "* SectorNr = "  << tempSectorNr;
    LOG(INFO) << "* SystemId = "  << tempSystemId;
    LOG(INFO) << "*****************";
    
    
}



void CbmDataConverter::MuchConverter(const uint8_t* FlesTimeSliceContent)
{
    
    
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