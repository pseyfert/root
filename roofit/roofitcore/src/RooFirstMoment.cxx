/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id$
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

/**
\file RooFirstMoment.cxx
\class RooFirstMoment
\ingroup Roofitcore

RooFirstMoment represents the first, second, or third order derivative
of any RooAbsReal as calculated (numerically) by the MathCore Richardson
derivator class.
**/

#include <string>                       // for string

#include "RooArgList.h"                 // for RooArgList
#include "RooCategory.h"                // for kTRUE, kFALSE, RooCategory, etc
#include "RooFirstMoment.h"             // for RooFirstMoment, RooAbsReal, etc
#include "RooNumIntConfig.h"            // for RooNumIntConfig
#include "RooProduct.h"                 // for RooProduct
#include "RooRealIntegral.h"            // for RooRealIntegral
#include "RooRealProxy.h"               // for RooRealProxy
#include "RooRealVar.h"                 // for RooRealVar
#include "RooSetProxy.h"                // for RooArgSet, RooSetProxy
#include "TString.h"                    // for Form

using namespace std ;


ClassImp(RooFirstMoment)
;


////////////////////////////////////////////////////////////////////////////////
/// Default constructor

RooFirstMoment::RooFirstMoment() 
{
}



////////////////////////////////////////////////////////////////////////////////

RooFirstMoment::RooFirstMoment(const char* name, const char* title, RooAbsReal& func, RooRealVar& x) :
  RooAbsMoment(name, title,func,x,1,kFALSE),
  _xf("!xf","xf",this,kFALSE,kFALSE),
  _ixf("!ixf","ixf",this),
  _if("!if","if",this)
{
  setExpensiveObjectCache(func.expensiveObjectCache()) ;
  
  string pname=Form("%s_product",name) ;

  RooProduct* XF = new RooProduct(pname.c_str(),pname.c_str(),RooArgSet(x,func)) ;
  XF->setExpensiveObjectCache(func.expensiveObjectCache()) ;

  if (func.isBinnedDistribution(x)) {
    XF->specialIntegratorConfig(kTRUE)->method1D().setLabel("RooBinIntegrator");
  }

  RooRealIntegral* intXF = (RooRealIntegral*) XF->createIntegral(x) ;
  RooRealIntegral* intF =  (RooRealIntegral*) func.createIntegral(x) ;
  intXF->setCacheNumeric(kTRUE) ;
  intF->setCacheNumeric(kTRUE) ;

  _xf.setArg(*XF) ;
  _ixf.setArg(*intXF) ;
  _if.setArg(*intF) ;
  addOwnedComponents(RooArgSet(*XF,*intXF,*intF)) ;
}

////////////////////////////////////////////////////////////////////////////////

RooFirstMoment::RooFirstMoment(const char* name, const char* title, RooAbsReal& func, RooRealVar& x, const RooArgSet& nset, Bool_t intNSet) :
  RooAbsMoment(name, title,func,x,1,kFALSE),
  _xf("!xf","xf",this,kFALSE,kFALSE),
  _ixf("!ixf","ixf",this),
  _if("!if","if",this)
{
  setExpensiveObjectCache(func.expensiveObjectCache()) ;

  _nset.add(nset) ;

  string pname=Form("%s_product",name) ;

  RooProduct* XF = new RooProduct(pname.c_str(),pname.c_str(),RooArgSet(x,func)) ;
  XF->setExpensiveObjectCache(func.expensiveObjectCache()) ;

  if (func.isBinnedDistribution(x)) {
    XF->specialIntegratorConfig(kTRUE)->method1D().setLabel("RooBinIntegrator");
  }

  if (intNSet && _nset.getSize()>0 && func.isBinnedDistribution(_nset)) {
    XF->specialIntegratorConfig(kTRUE)->method2D().setLabel("RooBinIntegrator");
    XF->specialIntegratorConfig(kTRUE)->methodND().setLabel("RooBinIntegrator");
  }

  RooArgSet intSet(x) ;
  if (intNSet) intSet.add(_nset,kTRUE) ;
  RooRealIntegral* intXF = (RooRealIntegral*) XF->createIntegral(intSet,&_nset) ;
  RooRealIntegral* intF =  (RooRealIntegral*) func.createIntegral(intSet,&_nset) ;
  intXF->setCacheNumeric(kTRUE) ;
  intF->setCacheNumeric(kTRUE) ;

  _xf.setArg(*XF) ;
  _ixf.setArg(*intXF) ;
  _if.setArg(*intF) ;
  addOwnedComponents(RooArgSet(*XF,*intXF,*intF)) ;
}



////////////////////////////////////////////////////////////////////////////////

RooFirstMoment::RooFirstMoment(const RooFirstMoment& other, const char* name) :
  RooAbsMoment(other, name), 
  _xf("xf",this,other._xf),
  _ixf("ixf",this,other._ixf),
  _if("if",this,other._if)
{
}



////////////////////////////////////////////////////////////////////////////////
/// Destructor

RooFirstMoment::~RooFirstMoment() 
{
}



////////////////////////////////////////////////////////////////////////////////
/// Calculate value  

Double_t RooFirstMoment::evaluate() const 
{
  Double_t ratio = _ixf / _if ;
  //cout << "\nRooFirstMoment::eval(" << GetName() << ") val = " << ratio << endl ;
  return ratio ;
}


