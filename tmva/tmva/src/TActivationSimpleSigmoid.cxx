// @(#)root/tmva $Id$
// Author: Paul Seyfert

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TActivationSimpleSigmoid                                              *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      A x/sqrt(1+x^2) activation function for TNeuron                           *
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
//  Simple sigmoid activation function for ANN.
//                                                                      
//_______________________________________________________________________

#include <iostream>

#include "TString.h"
#include "TMath.h"

#ifndef ROOT_TMVA_TActivationSimpleSigmoid
#include "TMVA/TActivationSimpleSigmoid.h"
#endif

ClassImp(TMVA::TActivationSimpleSigmoid)

//______________________________________________________________________________
TMVA::TActivationSimpleSigmoid::TActivationSimpleSigmoid()
{
   // constructor for simple sigmoid

}

//______________________________________________________________________________
TMVA::TActivationSimpleSigmoid::~TActivationSimpleSigmoid()
{
   // destructor

}

//______________________________________________________________________________
Double_t TMVA::TActivationSimpleSigmoid::Eval(Double_t arg)
{
   // evaluate the simple sigmoid

  return arg/TMath::Sqrt(1.+arg*arg);
}

//______________________________________________________________________________
Double_t TMVA::TActivationSimpleSigmoid::EvalDerivative(Double_t arg)
{
   // evaluate the derivative

  Double_t buffer = (1.+arg*arg);
  return 1./(buffer * TMath::Sqrt(buffer));
}

//______________________________________________________________________________
TString TMVA::TActivationSimpleSigmoid::GetExpression()
{
   // get expressions for the function and its derivative

   TString expr = "x/(1.+TMath::Sqrt(x*x))";

   expr += "\t\t";

   expr += "1./(1.+TMath::Sqrt(x*x))^3";

   return expr;
}

//______________________________________________________________________________
void TMVA::TActivationSimpleSigmoid::MakeFunction( std::ostream& fout, const TString& fncName ) 
{
   // writes the sigmoid activation function source code
   fout << "double " << fncName << "(double x) const {" << std::endl;
   fout << "   // activation function" << std::endl;
   fout << "   return x/sqrt(1.+x*x);" << std::endl;
   fout << "}" << std::endl;
}
