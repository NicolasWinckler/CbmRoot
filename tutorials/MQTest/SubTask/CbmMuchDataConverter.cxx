/* 
 * File:   CbmMuchDataConverter.cxx
 * Author: winckler
 * 
 * Created on July 30, 2014, 11:44 AM
 */

#include "CbmMuchDataConverter.h"

CbmMuchDataConverter::CbmMuchDataConverter() :fPrint(false),
        fDigiToPrint(0)
{
    fMuchDigiPayloadSize=sizeof(Int_t)+sizeof(Int_t)+sizeof(Int_t)+sizeof(Int_t);
}


CbmMuchDataConverter::~CbmMuchDataConverter() 
{
    
}


void CbmMuchDataConverter::MuchCbmTSFiller(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent, CbmTimeSlice* CbmRootTimeSlice)
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
        CbmRootTimeSlice->InsertData(&MuchDigi);
        
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



vector<CbmMuchDigi> CbmMuchDataConverter::MuchConverter(const fles::MicrosliceDescriptor* MSdesc,  const uint8_t* FlesTimeSliceContent)
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



CbmMicroSlice CbmMuchDataConverter::GetCbmMuchMicroSlice(fles::MicrosliceDescriptor* MSdesc, const vector<CbmMuchDigi> vMuchDigi)
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



void CbmMuchDataConverter::SetPrintOption(uint32_t DigiToPrint, bool print)
{
    fPrint=print;
    fDigiToPrint=DigiToPrint;
}


