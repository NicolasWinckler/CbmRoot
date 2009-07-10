/** CbmLitDefaultSettings.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** This header defines some of the constant settings used in the track reconstruction.
 **/

#ifndef CBMLITDEFAULTSETTINGS_H_
#define CBMLITDEFAULTSETTINGS_H_

namespace lit {

//Track propagation settings
const myf LINE_EXTRAPOLATION_START_Z = 450.; // z coordinate [cm] after which line extrapolation starts
const myf ENERGY_LOSS_CONST = 0.00354; // energy lost constant [GeV/c] used in the simple energy loss calculation
const myf MINIMUM_PROPAGATION_DISTANCE = 1e-3; // minimum propagation distance [cm]
const myf MAXIMUM_TGEO_NAVIGATION_DISTANCE = 30.; // maximum distance used in the TGeo navigation
const myf MAXIMUM_PROPAGATION_STEP_SIZE = 10.; // maximum step size in the TGeo track propagation

}

#endif /*CBMLITDEFAULTSETTINGS_H_*/
