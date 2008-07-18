/* *******************************************************************
// (C)opyright 2005
// 
// Institute of Computer Science V
// Prof. M�nner
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle / Gl��
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   class:
//     - base class for holding the framework's track parameters
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2006/11/22 12:20:16 $
// $Revision: 1.1 $
//
// *******************************************************************/


#include "../include/trackFrameworkMomentum.h"
#include <math.h>


#define sqr(a)  ((a) * (a)) 


/****************************************************************
 * CLASS trackFrameworkMomentum				 				*
 ****************************************************************/

/****************************************************************
 * Default constructor											*
 ****************************************************************/

trackFrameworkMomentum::trackFrameworkMomentum() {

	setTx(0);
	setTy(0);
	setQp(0);

}

/****************************************************************
 * Constructor													*
 ****************************************************************/

trackFrameworkMomentum::trackFrameworkMomentum(const trackFrameworkMomentum& value) {

	this->tx = value.tx;
	this->ty = value.ty;
	this->qp = value.qp;

}
trackFrameworkMomentum::trackFrameworkMomentum(double tx, double ty, double qp) {

	setTx(tx);
	setTy(ty);
	setQp(qp);

}

/****************************************************************
 * Destructor													*
 ****************************************************************/

trackFrameworkMomentum::~trackFrameworkMomentum() {

}

/****************************************************************
 * operator = ()												*
 ****************************************************************/

const trackFrameworkMomentum& trackFrameworkMomentum::operator = (const trackFrameworkMomentum& value) {

	this->tx = value.tx;
	this->ty = value.ty;
	this->qp = value.qp;

	return *this;

}

/****************************************************************
 * This method converts the object into a trackMomentum object.	*
 ****************************************************************/

trackFrameworkMomentum::operator trackMomentum() {

	trackMomentum returnValue;

	returnValue.set(getQp() / sqrt(sqr(getTx()) + sqr(getTy()) + 1), PZ);
	returnValue.set(getTy() * returnValue.get(PZ), PY);
	returnValue.set(getTx() * returnValue.get(PZ), PX);

	return returnValue;

}

/****************************************************************
 * returns the value for tx										*
 ****************************************************************/

double trackFrameworkMomentum::getTx() {

	return tx;

}

/****************************************************************
 * returns the value for ty										*
 ****************************************************************/

double trackFrameworkMomentum::getTy() {

	return ty;

}

/****************************************************************
 * returns the value for qp										*
 ****************************************************************/

double trackFrameworkMomentum::getQp() {

	return qp;

}

/****************************************************************
 * sets the value for tx										*
 ****************************************************************/

void trackFrameworkMomentum::setTx(double set) {

	tx = set;

}

/****************************************************************
 * sets the value for ty										*
 ****************************************************************/

void trackFrameworkMomentum::setTy(double set) {

	ty = set;

}

/****************************************************************
 * sets the value for qp										*
 ****************************************************************/

void trackFrameworkMomentum::setQp(double set) {

	qp = set;

}
