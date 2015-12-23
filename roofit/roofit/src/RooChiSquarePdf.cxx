/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitModels                                                     *
 * @(#)root/roofit:$Id$
 * Authors:                                                                  *
 *   Kyle Cranmer
 *                                                                           *
 *****************************************************************************/

/**
\file RooChiSquarePdf.cxx
\class RooChiSquarePdf
\ingroup Roofit

The PDF of the Chi Square distribution for n degrees of freedom.  
Oddly, this is hard to find in ROOT (except via relation to GammaDist).
Here we also implement the analytic integral.
**/

#include <math.h>                       // for pow, exp
#include <string.h>                     // for strlen

#include "RooChiSquarePdf.h"            // for RooChiSquarePdf, RooAbsPdf
#include "RooRealProxy.h"               // for RooRealProxy
#include "RooRealVar.h"                 // for Double_t, Int_t, ClassImp
#include "TError.h"                     // for R__ASSERT
#include "TMath.h"                      // for Gamma

class RooArgSet;

using namespace std;

ClassImp(RooChiSquarePdf)
;


////////////////////////////////////////////////////////////////////////////////

RooChiSquarePdf::RooChiSquarePdf()
{
}


////////////////////////////////////////////////////////////////////////////////

RooChiSquarePdf::RooChiSquarePdf(const char* name, const char* title, 
                           RooAbsReal& x, RooAbsReal& ndof): 
  RooAbsPdf(name, title),
  _x("x", "Dependent", this, x),
  _ndof("ndof","ndof", this, ndof)
{
}



////////////////////////////////////////////////////////////////////////////////

RooChiSquarePdf::RooChiSquarePdf(const RooChiSquarePdf& other, const char* name) :
  RooAbsPdf(other, name), 
  _x("x", this, other._x), 
  _ndof("ndof",this,other._ndof)
{
}


////////////////////////////////////////////////////////////////////////////////

Double_t RooChiSquarePdf::evaluate() const 
{
  if(_x <= 0) return 0;

  return  pow(_x,(_ndof/2.)-1.) * exp(-_x/2.) / TMath::Gamma(_ndof/2.) / pow(2.,_ndof/2.);


}


////////////////////////////////////////////////////////////////////////////////
/// No analytical calculation available (yet) of integrals over subranges

Int_t RooChiSquarePdf::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName) const 
{
  if (rangeName && strlen(rangeName)) {
    return 0 ;
  }


  if (matchArgs(allVars, analVars, _x)) return 1;
  return 0;
}


////////////////////////////////////////////////////////////////////////////////

Double_t RooChiSquarePdf::analyticalIntegral(Int_t code, const char* rangeName) const 
{
  R__ASSERT(code==1) ;
  Double_t xmin = _x.min(rangeName); Double_t xmax = _x.max(rangeName);

  // TMath::Prob needs ndof to be an integer, or it returns 0.
  //  return TMath::Prob(xmin, _ndof) - TMath::Prob(xmax,_ndof);

  // cumulative is known based on lower incomplete gamma function, or regularized gamma function
  // Wikipedia defines lower incomplete gamma function without the normalization 1/Gamma(ndof), 
  // but it is included in the ROOT implementation.  
  Double_t pmin = TMath::Gamma(_ndof/2,xmin/2);
  Double_t pmax = TMath::Gamma(_ndof/2,xmax/2);

  // only use this if range is appropriate
  return pmax-pmin;
}

