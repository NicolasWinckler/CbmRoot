/**
* \file CbmRichRingFitterCircle.h
*
* \brief Implementation of a ring fitting algorithm with equation of a circle.
*  Algorithm from F77 subroutine of S.Sadovsky.
*
* \author Supriya Das
* \date 2006
**/

#ifndef CBMRICHRINGFITTERCIRCLE
#define CBMRICHRINGFITTERCIRCLE

#include "CbmRichRingFitterImpl.h"

/**
* \class CbmRichRingFitterCircle
*
* \brief Implementation of a ring fitting algorithm with equation of a circle.
*  Algorithm from F77 subroutine of S.Sadovsky.
*
* \author Supriya Das
* \date 2006
**/
class CbmRichRingFitterCircle : public CbmRichRingFitterImpl
{
public:

   /**
    * \brief Default constructor.
    */
   CbmRichRingFitterCircle();

   /**
    * \brief Destructor.
    */
   virtual ~CbmRichRingFitterCircle();

   /**
    * \brief Start ring fitting algorithm.
    * \param[in,out] ring Pointer to ring to be fitted.
    */
   void DoFit(
         CbmRichRing* ring);

   ClassDef(CbmRichRingFitterCircle,1);
};

#endif
