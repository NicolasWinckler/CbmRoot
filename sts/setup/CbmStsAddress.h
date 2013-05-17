/** @file CbmStsAddress.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.05.2013
 **/

#ifndef CBMSTSADDRESS_H
#define CBMSTSADDRESS_H 1


#include "Rtypes.h"
#include "FairLogger.h"


/** Enumerator for STS element levels
 ** If this is changed, the initialisation of the static members must be
 ** changed accordingly!
 **/
enum StsElementLevel { kStsSystem,     //!< kStsSystem
                       kStsStation,    //!< kStsStation
                       kStsLadder,     //!< kStsLadder
                       kStsHalfLadder, //!< kStsHalfLadder
                       kStsModule,     //!< kStsModule
                       kStsSensor,     //!< kStsSensor
                       kStsSide,       //!< kStsSide
                       kStsChannel,    //!< kStsChannel
                       kStsNofLevels };//!< kStsNofLevels



/** @class CbmStsAddress
 ** @brief Interface class to unique address for the STS
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The CbmStsAddress interprets and modifies the unique address
 ** for the STS by the proper bit operation on the address bit field.
 **
 ** The current definition of the bit field for the STS is:
 **
 **                                               3         2         1
 ** Level           Bits  Max. Elements  Shift   10987654321098765432109876543210
 ** System (kSTS)     4         16          0    0000000000000000000000000000xxxx
 ** Station           4         16          4    000000000000000000000000xxxx0000
 ** Ladder            4         16          8    00000000000000000000xxxx00000000
 ** HalfLadder        1          2         12    0000000000000000000x000000000000
 ** Module            3          8         13    0000000000000000xxx0000000000000
 ** Sensor            2          4         16    00000000000000xx0000000000000000
 ** Side              1          2         18    0000000000000x000000000000000000
 ** Channel          13       8192         19    xxxxxxxxxxxxx0000000000000000000
 **/
class CbmStsAddress
{

  public:

    /** Construct address
     ** @param station      Station index
     ** @param ladder       Ladder index in station
     ** @param halfladder   Halfladder index in ladder
     ** @param module       Module index within halfladder
     ** @param sensor       Sensor index within module
     ** @param side         Side (0=front, 1=back) of sensor
     ** @param channel      Channel number
     ** @return Unique element address
     **/
    static Int_t GetAddress(Int_t station = 0,
                            Int_t ladder = 0,
                            Int_t halfladder = 0,
                            Int_t module = 0,
                            Int_t sensor = 0,
                            Int_t side = 0,
                            Int_t channel = 0);


    /** Get the index of an element
     ** @param address Unique element address
     ** @param level Hierarchy level
     ** @return Element index
     **/
    static Int_t GetElementId(Int_t address, StsElementLevel level) {
      return ( address & ( fgkMask[level] << fgkShift[level] ) )
             >> fgkShift[level];
    }


    /** Set the index of an element
     ** leaving the other element levels untouched
     ** @param address Unique element address
     ** @param level   Hierarchy level
     ** @param newId   New element index
     ** @return New address
     **/
     static Int_t SetElementId(Int_t address, StsElementLevel level, Int_t newId) {
      return ( address & (! (fgkMask[level] << fgkShift[level]) ) )
             & ( newId << fgkShift[level]);
    }


  private:

    /** Number of bits for the different levels **/
    static const Int_t fgkBits[kStsNofLevels];

    /** Bit shifts for the different levels **/
    static const Int_t fgkShift[kStsNofLevels];

    /** Bit masks for the different levels **/
    static const Int_t fgkMask[kStsNofLevels];

};


#endif /* CBMSTSADDRESS_H  */
