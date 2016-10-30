// @(#)root/tmva $Id$
// Author: Matt Jachowski

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TActivationSigmoid                                                    *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Sigmoid activation function for TNeuron                                   *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Matt Jachowski  <jachowski@stanford.edu> - Stanford University, USA       *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland                                                         *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/
  
//_______________________________________________________________________
//                                                                      
//  Sigmoid activation function for TNeuron. This really simple implementation
//  uses TFormulas and should probably be replaced with something more
//  efficient later.
//                                                                      
//_______________________________________________________________________

#include "TMVA/TActivationSigmoid.h"

#include "TMVA/TActivation.h"

#include "vdt/atan.h"
#include "vdt/exp.h"

#include "TMath.h"
#include "TString.h"

#include <iostream>

ClassImp(TMVA::TActivationSigmoid)

////////////////////////////////////////////////////////////////////////////////
/// constructor for sigmoid normalized in [0,1]

TMVA::TActivationSigmoid::TActivationSigmoid()
{
}

////////////////////////////////////////////////////////////////////////////////
/// destructor

TMVA::TActivationSigmoid::~TActivationSigmoid()
{
}

////////////////////////////////////////////////////////////////////////////////
/// evaluate the sigmoid

Double_t TMVA::TActivationSigmoid::Eval(Double_t arg)
{
   return 0.5*(1.+vdt::fast_tanh(0.5*arg));
   //return 1.0/(1.0+TMath::Exp(-arg));
}

////////////////////////////////////////////////////////////////////////////////
/// evaluate the derivative of the sigmoid

Double_t TMVA::TActivationSigmoid::EvalDerivative(Double_t arg)
{
   Double_t expon = vdt::fast_exp(-arg);
   Double_t frac = Eval(arg);
   frac *= frac;
   return expon*frac;
   //Double_t denom = (1.0+expon);
   //denom *= denom;
   //return expon/denom;
}

////////////////////////////////////////////////////////////////////////////////
/// get expressions for the sigmoid and its derivatives

TString TMVA::TActivationSigmoid::GetExpression()
{
   TString expr = "";
   
   expr += "1.0/(1.0+TMath::Exp(-x))";
   
   expr += "\t\t";
   
   expr += "TMath::Exp(-x)/(1.0+TMath::Exp(-x))^2";
   
   return expr;
}

////////////////////////////////////////////////////////////////////////////////
/// writes the sigmoid activation function source code

void TMVA::TActivationSigmoid::MakeFunction( std::ostream& fout, const TString& fncName ) 
{
   fout << "double " << fncName << "(double x) const {" << std::endl;
   fout << "   // sigmoid" << std::endl;
   fout << "   return 1.0/(1.0+exp(-x));" << std::endl;
   fout << "}" << std::endl;
}
