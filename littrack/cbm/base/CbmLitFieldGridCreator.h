/** CbmLitFieldGridCreator.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **
 ** Class creates grid with magnetic field
 ** values at a certain Z position.
 **/
#ifndef CBMLITFIELDGRIDCREATOR_H_
#define CBMLITFIELDGRIDCREATOR_H_

#include <vector>
#include "../../parallel/LitField.h"
class FairField;

class CbmLitFieldGridCreator {
public:
   /* Constructor */
   CbmLitFieldGridCreator();

   /* Destructor */
   virtual ~CbmLitFieldGridCreator();

   /* Sets acceptance angle for X. */
   void SetXangle(float xangle) {fXangle = xangle;}

   /* Sets acceptance angle for Y. */
   void SetYangle(float yangle) {fYangle = yangle;}

   /* Sets cell size for X. */
   void SetCellSizeX(int cellSizeX) {fCellSizeX = cellSizeX;}

   /* Sets cell size for Y. */
   void SetCellSizeY(int cellSizeY) {fCellSizeY = cellSizeY;}

   /* Creates grid with magnetic field values in (X, Y) slice
    * Z position.
    *@param Z Z position of the slice.
    *@param grid Output grid */
   void CreateGrid(
      fscal Z,
      lit::parallel::LitFieldGrid& grid);

private:
   FairField* fField; // Magnetic field

   float fXangle; // Acceptance angle for X [grad]
   float fYangle; // Acceptance angle for Y [grad]

   float fCellSizeX; // Cell size along X [cm]
   float fCellSizeY; // Cell size along Y [cm]
};

#endif /* CBMLITFIELDGRIDCREATOR_H_ */
