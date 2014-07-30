/* 
 * File:   CbmStsDataConverter.cxx
 * Author: winckler
 * 
 * Created on July 30, 2014, 11:44 AM
 */

#include "CbmStsDataConverter.h"

CbmStsDataConverter::CbmStsDataConverter() : fPrint(false),fDigiToPrint(0)
{
    fStsDigiPayloadSize=sizeof(ULong64_t)+sizeof(UInt_t)+sizeof(UShort_t)+sizeof(Int_t)+sizeof(Int_t);
}

CbmStsDataConverter::~CbmStsDataConverter() 
{
}


void CbmStsDataConverter::StsCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice)
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
        CbmRootTimeSlice->InsertData(&StsDigi);
        
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



vector<CbmStsDigi>  CbmStsDataConverter::StsConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
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



CbmMicroSlice CbmStsDataConverter::GetCbmStsMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmStsDigi> vStsDigi)
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


void CbmStsDataConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
}
