/** CbmLitDefaultSettings.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **
 ** Header defines constants which are used in the track reconstruction.
 **/

#ifndef CBMLITDEFAULTSETTINGS_H_
#define CBMLITDEFAULTSETTINGS_H_

namespace lit
{

// Track propagation settings
const litfloat LINE_EXTRAPOLATION_START_Z = 450.; // z coordinate [cm] after which linear track extrapolation starts
const litfloat ENERGY_LOSS_CONST = 0.00354; // energy lost constant [GeV/c] used in the simple energy loss calculation
const litfloat MINIMUM_PROPAGATION_DISTANCE = 1e-6; // minimum propagation distance [cm]
const litfloat MAXIMUM_TGEO_NAVIGATION_DISTANCE = 25.; // maximum distance used in the TGeo navigation
const litfloat MAXIMUM_PROPAGATION_STEP_SIZE = 10.; // maximum step size in the TGeo track propagation

}

#endif /*CBMLITDEFAULTSETTINGS_H_*/
