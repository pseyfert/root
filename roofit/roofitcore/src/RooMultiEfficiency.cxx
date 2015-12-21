/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id$
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *   PS, Paul Seyfert,    IT INFN-MIB,      pseyfert@cern.ch                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

/**
\file RooMultiEfficiency.cxx
\class RooMultiEfficiency
\ingroup Roofitcore

RooMultiEfficiency is a PDF helper class to fit efficiencies parameterized
by a supplied function F.

Given a dataset with a category C that determines if a given
event is accepted or rejected for the efficiency to be measured,
this class evaluates as F if C is 'accept' and as (1-F) if
C is 'reject'. Values of F below 0 and above 1 are clipped.
F may have an arbitrary number of dependents and parameters
**/

#include "RooFit.h"

#include "RooMultiEfficiency.h"
#include "RooStreamParser.h"
#include "RooArgList.h"

#include "TError.h"

using namespace std;

ClassImp(RooMultiEfficiency)
  ;


////////////////////////////////////////////////////////////////////////////////
/// Construct an N+1 dimensional efficiency p.d.f from an N-dimensional efficiency
/// function and a category cat with two states (0,1) that indicate if a given
/// event should be counted as rejected or accepted respectively

RooMultiEfficiency::RooMultiEfficiency(const char *name, const char *title, const RooArgList& effFuncList, const RooAbsCategory& cat, vector<TString> sigCatNames) :
  RooAbsPdf(name,title),
  _cat("cat","category",this,(RooAbsCategory&)cat),
  _effFuncList("effFuncList","List of efficiency functions",this),
  _sigCatNames(sigCatNames)
{  
  _effFuncList.add(effFuncList);

  if (_sigCatNames.size() != effFuncList.getSize()) {
    coutE(InputArguments) << "RooMultiEfficiency::ctor(" << GetName() << ") ERROR: Wrong input, should have equal number of category names and efficiencies." << endl;
    throw string("RooMultiEfficiency::ctor() ERROR: Wrong input, should have equal number of category names and efficiencies") ;
  }
}



////////////////////////////////////////////////////////////////////////////////
/// Copy constructor

RooMultiEfficiency::RooMultiEfficiency(const RooMultiEfficiency& other, const char* name) : 
  RooAbsPdf(other, name),
  _cat("cat",this,other._cat),
  _effFuncList("effFunc",this,other._effFuncList),
  _sigCatNames(other._sigCatNames)
{
}



////////////////////////////////////////////////////////////////////////////////
/// Destructor

RooMultiEfficiency::~RooMultiEfficiency() 
{
}



////////////////////////////////////////////////////////////////////////////////
/// Calculate the raw value of this p.d.f which is the effFunc
/// value if cat==1 and it is (1-effFunc) if cat==0

Double_t RooMultiEfficiency::evaluate() const
{
  Int_t effFuncListSize = _effFuncList.getSize();

  // Get efficiency function for category i

  vector<Double_t> effFuncVal(effFuncListSize);
  for (int i=0; i<effFuncListSize; ++i) {
    effFuncVal[i] = ((RooAbsReal&)_effFuncList[i]).getVal() ;
  }

  // Truncate efficiency functions in range 0.0-1.0

  for (int i=0; i<effFuncListSize; ++i) {
    if (effFuncVal[i]>1.) {
      coutW(Eval) << "WARNING: Efficency >1 (equal to " << effFuncVal[i] 
		  << " ), for i = " << i << "...TRUNCATED" << endl;
      effFuncVal[i] = 1.0 ;
    } else if (effFuncVal[i]<0) {
      effFuncVal[i] = 0.0 ;
      coutW(Eval) << "WARNING: Efficency <0 (equal to " << effFuncVal[i] 
		  << " ), for i = " << i << "...TRUNCATED" << endl;
    }
  }

  Double_t sum = std::accumulate(effFuncVal.begin(),effFuncVal.end(),0.);
  if (sum>1. || sum<0.) {
    coutW(Eval) << "WARNING: Efficency sum out of range (equal to " << sum
		<< " )  ...TRUNCATED" << endl;
  }

  for (size_t l = 0 ; l < _sigCatNames.size() ; ++l) {
    if (_cat.label() == _sigCatNames[l]) {
      return effFuncVal[l];
    }
  }

  coutE(Eval) << "Category label not found!" << endl;
  return 0.;
}



////////////////////////////////////////////////////////////////////////////////

Int_t RooMultiEfficiency::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const 
{
  if (matchArgs(allVars,analVars,_cat)) return 1 ;
  return 0 ;
}



////////////////////////////////////////////////////////////////////////////////

Double_t RooMultiEfficiency::analyticalIntegral(Int_t code, const char* /*rangeName*/) const 
{
  R__ASSERT(code==1) ;
  return 1.0 ;
}






