 /***************************************************************************** 
  * Project: RooFit                                                           * 
  * @(#)root/roofit:$Id$ *
  *                                                                           * 
  * RooFit Lognormal PDF                                                      *
  *                                                                           * 
  * Author: Gregory Schott and Stefan Schmitz                                 *
  *                                                                           * 
  *****************************************************************************/ 

/**
\file RooLognormal.cxx
\class RooLognormal
\ingroup Roofit

RooFit Lognormal PDF. The two parameters are:
  - m0: the median of the distribution
  - k=exp(sigma): sigma is called the shape parameter in the TMath parametrization

\f[ Lognormal(x,m_0,k) = \frac{e^{(-ln^2(x/m_0))/(2ln^2(k))}}{\sqrt{2\pi \cdot ln(k)\cdot x}} \f]

The parametrization here is physics driven and differs from the ROOT::Math::lognormal_pdf(x,m,s,x0) with:
  - m = log(m0)
  - s = log(k)
  - x0 = 0
**/

#include <Math/PdfFuncMathCore.h>       // for lognormal_pdf
#include <math.h>                       // for sqrt

#include "RooArgSet.h"                  // for Double_t, Int_t, Bool_t, etc
#include "RooLognormal.h"               // for RooLognormal, RooAbsPdf
#include "RooMath.h"                    // for RooMath
#include "RooRandom.h"                  // for RooRandom
#include "RooRealProxy.h"               // for RooRealProxy
#include "TError.h"                     // for R__ASSERT
#include "TMath.h"                      // for Log, Exp
#include "TMathBase.h"                  // for Abs
#include "TRandom.h"                    // for TRandom

using namespace std;

ClassImp(RooLognormal)


////////////////////////////////////////////////////////////////////////////////

RooLognormal::RooLognormal(const char *name, const char *title,
			 RooAbsReal& _x, RooAbsReal& _m0,
			 RooAbsReal& _k) :
  RooAbsPdf(name,title),
  x("x","Observable",this,_x),
  m0("m0","m0",this,_m0),
  k("k","k",this,_k)
{
}



////////////////////////////////////////////////////////////////////////////////

RooLognormal::RooLognormal(const RooLognormal& other, const char* name) : 
  RooAbsPdf(other,name), x("x",this,other.x), m0("m0",this,other.m0),
  k("k",this,other.k)
{
}



////////////////////////////////////////////////////////////////////////////////
/// ln(k)<1 would correspond to sigma < 0 in the parametrization
/// resulting by transforming a normal random variable in its
/// standard parametrization to a lognormal random variable
/// => treat ln(k) as -ln(k) for k<1

Double_t RooLognormal::evaluate() const
{
  Double_t xv = x;
  Double_t ln_k = TMath::Abs(TMath::Log(k));
  Double_t ln_m0 = TMath::Log(m0);
  Double_t x0 = 0;

  Double_t ret = ROOT::Math::lognormal_pdf(xv,ln_m0,ln_k,x0);
  return ret ;
}



////////////////////////////////////////////////////////////////////////////////

Int_t RooLognormal::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const 
{
  if (matchArgs(allVars,analVars,x)) return 1 ;
  return 0 ;
}



////////////////////////////////////////////////////////////////////////////////

Double_t RooLognormal::analyticalIntegral(Int_t code, const char* rangeName) const 
{
  R__ASSERT(code==1) ;

  static const Double_t root2 = sqrt(2.) ;

  Double_t ln_k = TMath::Abs(TMath::Log(k));
  Double_t ret = 0.5*( RooMath::erf( TMath::Log(x.max(rangeName)/m0)/(root2*ln_k) ) - RooMath::erf( TMath::Log(x.min(rangeName)/m0)/(root2*ln_k) ) ) ;

  return ret ;
}




////////////////////////////////////////////////////////////////////////////////

Int_t RooLognormal::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t /*staticInitOK*/) const
{
  if (matchArgs(directVars,generateVars,x)) return 1 ;  
  return 0 ;
}



////////////////////////////////////////////////////////////////////////////////

void RooLognormal::generateEvent(Int_t code)
{
  R__ASSERT(code==1) ;

  Double_t xgen ;
  while(1) {    
    xgen = TMath::Exp(RooRandom::randomGenerator()->Gaus(TMath::Log(m0),TMath::Log(k)));
    if (xgen<=x.max() && xgen>=x.min()) {
      x = xgen ;
      break;
    }
  }

  return;
}


