// @(#)root/tmva $Id$
// Author: Matt Jachowski 

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TActivationRadial                                                     *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Radial basis activation function for TNeuron                              *
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
//  Radial basis activation function for ANN.
//_______________________________________________________________________

#include <iostream>

#include "TString.h"
#include "TMath.h"

#ifndef ROOT_TMVA_TActivationRadial
#include "TMVA/TActivationRadial.h"
#endif

static const Int_t  UNINITIALIZED = -1;

ClassImp(TMVA::TActivationRadial)

////////////////////////////////////////////////////////////////////////////////
/// constructor for gaussian with center 0, width 1

TMVA::TActivationRadial::TActivationRadial()
{
}

////////////////////////////////////////////////////////////////////////////////
/// destructor

TMVA::TActivationRadial::~TActivationRadial()
{
}

////////////////////////////////////////////////////////////////////////////////
/// evaluate gaussian

Double_t TMVA::TActivationRadial::Eval(Double_t arg)
{
  return TMath::Exp(-arg*arg*0.5);
}

////////////////////////////////////////////////////////////////////////////////
/// evaluate derivative

Double_t TMVA::TActivationRadial::EvalDerivative(Double_t arg)
{
  return -arg*TMath::Exp(-arg*arg*0.5);
}

////////////////////////////////////////////////////////////////////////////////
/// get expressions for the gaussian and its derivatives

TString TMVA::TActivationRadial::GetExpression()
{
   TString expr = "TMath::Exp(-x^2/2.0)\t\t-x*TMath::Exp(-x^2/2.0)";
   return expr;
}

////////////////////////////////////////////////////////////////////////////////
/// writes the sigmoid activation function source code

void TMVA::TActivationRadial::MakeFunction( std::ostream& fout, const TString& fncName ) 
{
   fout << "double " << fncName << "(double x) const {" << std::endl;
   fout << "   // radial" << std::endl;
   fout << "   return exp(-x*x*0.5);" << std::endl;
   fout << "}" << std::endl;
}
