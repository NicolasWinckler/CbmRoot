/** @file CbmStsModule.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 14.05.2013
 **/


#include "FairLogger.h"
#include "CbmStsAddress.h"
#include "CbmStsDigi.h"
#include "digitize/CbmStsDigitizeIdeal.h"
#include "setup/CbmStsModule.h"
#include "setup/CbmStsSensorType.h"
#include "setup/CbmStsSetup.h"


// -----   Default constructor   -------------------------------------------
CbmStsModule::CbmStsModule() : CbmStsElement(),
	                             fDynRange(0.),
	                             fThreshold(0.),
	                             fNofAdcChannels(0),
	                             fIsSet(kFALSE),
	                             fBuffer()
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsModule::CbmStsModule(const char* name, const char* title,
                           TGeoPhysicalNode* node) :
                           CbmStsElement(name, title, kStsModule, node),
                           fDynRange(0.),
                           fThreshold(0.),
                           fNofAdcChannels(0),
                           fIsSet(0),
                           fBuffer()
{
}
// -------------------------------------------------------------------------



// --- Destructor   --------------------------------------------------------
CbmStsModule::~CbmStsModule() {
}
// -------------------------------------------------------------------------



// -----   Add a signal to the buffer   ------------------------------------
void CbmStsModule::AddSignal(Int_t channel, Double_t time,
                             Double_t charge) {

	// --- Debug
	LOG(DEBUG3) << GetName() << ": Receiving signal " << charge
			        << " in channel " << channel << " at time "
			        << time << " s" << FairLogger::endl;

	// --- If there is no signal in the buffer for this channel:
	// --- write the signal into the buffer
	if ( fBuffer.find(channel) == fBuffer.end() ) {
		fBuffer[channel] = pair<Double_t, Double_t>(charge, time);
		LOG(DEBUG4) << GetName() << ": New signal " << charge << " at time "
				        << time << " in channel " << channel << FairLogger::endl;
	}  //? No signal in buffer

	// --- If there is already a signal, compare the time of the two
	else {
		Double_t chargeOld = (fBuffer.find(channel)->second).first;
		Double_t timeOld   = (fBuffer.find(channel)->second).second;
		LOG(DEBUG4) << GetName() << ": old signal in channel " << channel
				        << " at time " << timeOld << FairLogger::endl;

		// --- Time separation large; no interference. Digitise the old signal,
		// --- send it to the DAQ and write the new signal into the buffer.
		if ( time - timeOld > 100. ) {  // 5 Mark in die Kasse für hardcoded numbers
			Digitize(channel, chargeOld, timeOld);
			fBuffer[channel] = pair<Double_t, Double_t>(charge, time);
			LOG(DEBUG4) << GetName() << ": New signal " << charge << " at time "
					        << time << " in channel " << channel << FairLogger::endl;
		}  //? No interference of signals

		// --- Time separation small; interference.
		// --- Current implementation: Add up charge in the buffer.
		// --- Time is the one of the first signal.
		else {
			fBuffer[channel] =
					pair<Double_t, Double_t>(chargeOld + charge, timeOld);
			LOG(DEBUG4) << GetName() << ": New signal " << chargeOld + charge
					        << " at time "<< timeOld << " in channel " << channel
					        << FairLogger::endl;
		}  //? Interference of signals

	}  //? Already signal in buffer

}
// -------------------------------------------------------------------------



// -----   Clean the analogue buffer   -------------------------------------
void CbmStsModule::CleanBuffer(Double_t readoutTime) {

	map<Int_t, pair<Double_t, Double_t> >::iterator it = fBuffer.begin();

	while ( it != fBuffer.end() ) {

		// --- Compare signal time to readout time
		Double_t signalTime = (it->second).second;
		if ( readoutTime < 0 || signalTime < readoutTime ) {

			// --- Digitise the signal and remove it from buffer
			Double_t charge = (it->second).first;
			Int_t channel = it->first;
			LOG(DEBUG4) << GetName() << ": Clean Buffer " << channel << " "
				        << signalTime << " " << charge << FairLogger::endl;
			Digitize(channel, charge, signalTime);

			// Next entry in buffer. Delete digitised buffer entry.
			it++;
			fBuffer.erase(channel);

		} //? Signal before read-out time

	}  // Iterator over buffer

}
// -------------------------------------------------------------------------



// -----   Digitise an analog charge signal   ------------------------------
void CbmStsModule::Digitize(Int_t channel, Double_t charge, Double_t time) {

	// --- No action if charge is below threshold
	if ( charge <= fThreshold ) return;

	// --- Construct channel address from module address and channel number
	UInt_t address = CbmStsAddress::SetElementId(GetAddress(),
			                                         kStsChannel, channel);

	// --- Digitise charge
	// --- Prescription according to the information on the STS-XYTER
	// --- by C. Schmidt.
	UShort_t adc = 0;
	if ( charge > fDynRange ) adc = fNofAdcChannels - 1;
	else adc = UShort_t( (charge - fThreshold) / fDynRange
				     * Double_t(fNofAdcChannels) );

	// --- Digitise time
	// TODO: Add Gaussian time resolution
	ULong64_t dTime = ULong64_t(time);

	// --- Send the message to the digitiser task
	LOG(DEBUG4) << GetName() << ": charge " << charge << ", dyn. range "
			<< fDynRange << ", threshold " << fThreshold << ", # ADC channels "
			<< fNofAdcChannels << FairLogger::endl;
	LOG(DEBUG3) << GetName() << ": Sending message. Address " << address
			        << ", time " << dTime << ", adc " << adc << FairLogger::endl;
	CbmStsDigitizeIdeal* digitiser = CbmStsSetup::Instance()->GetDigitizer();
	if ( digitiser ) digitiser->CreateDigi(address, dTime, adc);

}
// -------------------------------------------------------------------------


ClassImp(CbmStsModule)
