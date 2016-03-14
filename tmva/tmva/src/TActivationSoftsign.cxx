// @(#)root/tmva $Id$
// Author: Paul Seyfert

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TActivationSoftsign                                                   *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      A x/(1+|x|) activation function for TNeuron                               *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Paul Seyfert  <pseyfert@cern.ch>                                          *
 *                                                                                *
 * Copyright (c) 2015:                                                            *
 *      CERN, Switzerland                                                         *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_______________________________________________________________________
//                                                                      
//  softsign activation function for ANN.
//                                                                      
//_______________________________________________________________________

#include <iostream>

#include "TString.h"
#include "TMath.h"

#ifndef ROOT_TMVA_TActivationSoftsign
#include "TMVA/TActivationSoftsign.h"
#endif

ClassImp(TMVA::TActivationSoftsign)

//______________________________________________________________________________
TMVA::TActivationSoftsign::TActivationSoftsign()
{
   // constructor for softsign

}

//______________________________________________________________________________
TMVA::TActivationSoftsign::~TActivationSoftsign()
{
   // destructor

}

//______________________________________________________________________________
Double_t TMVA::TActivationSoftsign::Eval(Double_t arg)
{
   // evaluate the softsign

  return arg/(1.+TMath::Abs(arg));
}

//______________________________________________________________________________
Double_t TMVA::TActivationSoftsign::EvalDerivative(Double_t arg)
{
   // evaluate the derivative
  Double_t buffer = 1.+TMath::Abs(arg);
  return 1./(buffer*buffer);
}

//______________________________________________________________________________
TString TMVA::TActivationSoftsign::GetExpression()
{
   // get expressions for the function and its derivative

   TString expr = "x/(1.+TMath::Abs(x))";

   expr += "\t\t";

   expr += "1./(1.+TMath::Abs(x))^2";

   return expr;
}

//______________________________________________________________________________
void TMVA::TActivationSoftsign::MakeFunction( std::ostream& fout, const TString& fncName ) 
{
   // writes the sigmoid activation function source code
   fout << "double " << fncName << "(double x) const {" << std::endl;
   fout << "   // activation function" << std::endl;
   fout << "   return x/(1.+fabs(x));" << std::endl;
   fout << "}" << std::endl;
}
