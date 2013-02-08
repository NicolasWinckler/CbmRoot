/**
 * \file CbmTrdElectronsQa.h
 *
 * \brief
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/

#ifndef CbmTrdElectronsQa_h
#define CbmTrdElectronsQa_h 1

#include "FairTask.h"

/**
 * \class CbmTrdElectronsQa
 *
 * \brief
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 **/
class CbmTrdElectronsQa : public FairTask
{
public:

   /**
    * \brief Default constructor
    */
	CbmTrdElectronsQa();

   /**
    * \brief Destructor
    */
	~CbmTrdElectronsQa();

   /**
    * \brief Inherited from FairTask
    */
	virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask
    */
	virtual void Exec(
	      Option_t* opt);

   /**
    * \brief Inherited from FairTask
    */
	virtual void Finish();

private:

	CbmTrdElectronsQa(const CbmTrdElectronsQa&);
	CbmTrdElectronsQa& operator=(const CbmTrdElectronsQa&);

	ClassDef(CbmTrdElectronsQa,1);

};

#endif
