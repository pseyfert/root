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
\file RooLandau.cxx
\class RooLandau
\ingroup Roofit

Landau Distribution p.d.f
**/

#include "RooArgSet.h"                  // for Double_t, Int_t, Bool_t, etc
#include "RooLandau.h"                  // for RooLandau, RooAbsPdf
#include "RooRandom.h"                  // for RooRandom
#include "RooRealProxy.h"               // for RooRealProxy
#include "TError.h"                     // for R__ASSERT
#include "TMath.h"                      // for Landau
#include "TRandom.h"                    // for TRandom

using namespace std;

ClassImp(RooLandau)


////////////////////////////////////////////////////////////////////////////////

RooLandau::RooLandau(const char *name, const char *title, RooAbsReal& _x, RooAbsReal& _mean, RooAbsReal& _sigma) :
  RooAbsPdf(name,title),
  x("x","Dependent",this,_x),
  mean("mean","Mean",this,_mean),
  sigma("sigma","Width",this,_sigma)
{
}
 

////////////////////////////////////////////////////////////////////////////////

RooLandau::RooLandau(const RooLandau& other, const char* name) : 
  RooAbsPdf(other,name),
  x("x",this,other.x),
  mean("mean",this,other.mean),
  sigma("sigma",this,other.sigma)
{
} 


////////////////////////////////////////////////////////////////////////////////

Double_t RooLandau::evaluate() const
{
  return TMath::Landau(x, mean, sigma);
}


////////////////////////////////////////////////////////////////////////////////

Int_t RooLandau::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
  if (matchArgs(directVars,generateVars,x)) return 1 ;  
  return 0 ;
}


////////////////////////////////////////////////////////////////////////////////

void RooLandau::generateEvent(Int_t code)
{
  R__ASSERT(code==1) ;
  Double_t xgen ;
  while(1) {    
    xgen = RooRandom::randomGenerator()->Landau(mean,sigma);
    if (xgen<x.max() && xgen>x.min()) {
      x = xgen ;
      break;
    }
  }
  return;
}


