/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id$
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
\file RooUniform.cxx
\class RooUniform
\ingroup Roofit

Flat p.d.f. in N dimensions
**/

#include <ostream>                      // for operator<<, basic_ostream, etc

#include "RooAbsArg.h"                  // for RooAbsArg
#include "RooArgSet.h"                  // for RooArgSet, Int_t, Double_t, etc
#include "RooListProxy.h"               // for RooListProxy
#include "RooMsgService.h"              // for coutW
#include "RooRealVar.h"                 // for RooAbsRealLValue
#include "RooUniform.h"                 // for RooUniform, RooAbsPdf

using namespace std;

ClassImp(RooUniform)


////////////////////////////////////////////////////////////////////////////////

RooUniform::RooUniform(const char *name, const char *title, const RooArgSet& _x) :
  RooAbsPdf(name,title),
  x("x","Observables",this,kTRUE,kFALSE)
{
  x.add(_x) ;
}



////////////////////////////////////////////////////////////////////////////////

RooUniform::RooUniform(const RooUniform& other, const char* name) : 
  RooAbsPdf(other,name), x("x",this,other.x)
{
}



////////////////////////////////////////////////////////////////////////////////

Double_t RooUniform::evaluate() const
{
  return 1 ;
}



////////////////////////////////////////////////////////////////////////////////
/// Advertise analytical integral

Int_t RooUniform::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const 
{
  Int_t nx = x.getSize() ;
  if (nx>31) {
    // Warn that analytical integration is only provided for the first 31 observables
    coutW(Integration) << "RooUniform::getAnalyticalIntegral(" << GetName() << ") WARNING: p.d.f. has " << x.getSize() 
		       << " observables, analytical integration is only implemented for the first 31 observables" << endl ;
    nx=31 ;
  }

  Int_t code(0) ;
  for (int i=0 ; i<x.getSize() ; i++) {
    if (allVars.find(x.at(i)->GetName())) {
      code |= (1<<i) ;
      analVars.add(*allVars.find(x.at(i)->GetName())) ;
    }
  }    
  return code ;
}



////////////////////////////////////////////////////////////////////////////////
/// Implement analytical integral

Double_t RooUniform::analyticalIntegral(Int_t code, const char* rangeName) const 
{
  Double_t ret(1) ;
  for (int i=0 ; i<32 ; i++) {
    if (code&(1<<i)) {
      RooAbsRealLValue* var = (RooAbsRealLValue*)x.at(i) ;
      ret *= (var->getMax(rangeName) - var->getMin(rangeName)) ;
    }    
  }
  return ret ;
}




////////////////////////////////////////////////////////////////////////////////
/// Advertise internal generator 

Int_t RooUniform::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
  Int_t nx = x.getSize() ;
  if (nx>31) {
    // Warn that analytical integration is only provided for the first 31 observables
    coutW(Integration) << "RooUniform::getGenerator(" << GetName() << ") WARNING: p.d.f. has " << x.getSize() 
		       << " observables, internal integrator is only implemented for the first 31 observables" << endl ;
    nx=31 ;
  }
  
  Int_t code(0) ;
  for (int i=0 ; i<x.getSize() ; i++) {
    if (directVars.find(x.at(i)->GetName())) {
      code |= (1<<i) ;
      generateVars.add(*directVars.find(x.at(i)->GetName())) ;
    }
  }    
  return code ;
  return 0 ;
}



////////////////////////////////////////////////////////////////////////////////
/// Implement internal generator

void RooUniform::generateEvent(Int_t code)
{
  // Fast-track handling of one-observable case
  if (code==1) {
    ((RooAbsRealLValue*)x.at(0))->randomize() ;
    return ;
  }

  for (int i=0 ; i<32 ; i++) {
    if (code&(1<<i)) {
      RooAbsRealLValue* var = (RooAbsRealLValue*)x.at(i) ;
      var->randomize() ;
    }    
  }
}


