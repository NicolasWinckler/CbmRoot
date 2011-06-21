/** CbmLitFieldGridCreator.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **
 ** Class creates grid with magnetic field
 ** values at a certain Z position.
 **/
#include "CbmLitFieldGridCreator.h"
#include "CbmLitEnvironment.h"
#include "../../parallel/LitField.h"

#include "FairField.h"

CbmLitFieldGridCreator::CbmLitFieldGridCreator():
   fField(CbmLitEnvironment::Instance()->GetField()),
   fXangle(35.),
   fYangle(35.),
   fCellSizeX(2.),
   fCellSizeY(2.)
{

}

CbmLitFieldGridCreator::~CbmLitFieldGridCreator()
{

}

void CbmLitFieldGridCreator::CreateGrid(
   fscal Z,
   lit::parallel::LitFieldGrid& grid)
{
   float tanXangle = std::tan(fXangle * 3.14159265 / 180.); //
   float tanYangle = std::tan(fYangle * 3.14159265 / 180.); //
   float Xmax = Z * tanXangle;
   float Ymax = Z * tanYangle;
   int nofCellsX = int(Xmax / fCellSizeX);
   int nofCellsY = int(Ymax / fCellSizeY);
   Xmax = nofCellsX * fCellSizeX;
   Ymax = nofCellsY * fCellSizeY;
   nofCellsX += nofCellsX;
   nofCellsY += nofCellsY;

   std::vector<std::vector<lit::parallel::LitFieldValue<fscal> > > field;
   field.resize(nofCellsX + 1);
   for (unsigned int i = 0; i < nofCellsX + 1; i++) {
      field[i].resize(nofCellsY + 1);
   }

   for (int j = 0; j < nofCellsX + 1; j++) { // loop over x position
      double X = -Xmax + j * fCellSizeX;
      for (int k = 0; k < nofCellsY + 1; k++) { // loop over y position
         double Y = -Ymax + k * fCellSizeY;

         // get field value
         double pos[3] = {X, Y, Z};
         double B[3];
         fField->GetFieldValue(pos, B);

         lit::parallel::LitFieldValue<fscal> v;
         v.Bx = B[0];
         v.By = B[1];
         v.Bz = B[2];
         field[j][k] = v;
      }
   }
   grid.SetZ(Z);
   grid.SetField(field, -Xmax, Xmax, -Ymax, Ymax, nofCellsX, nofCellsY);
   std::cout << "Grid created: " << grid << std::endl;
}
