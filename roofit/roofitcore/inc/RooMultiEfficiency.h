/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooEfficiency.h,v 1.6 2007/05/11 10:14:56 verkerke Exp $
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
#ifndef ROO_MULTIEFFICIENCY
#define ROO_MULTIEFFICIENCY

#include "RooAbsPdf.h"
#include "RooCategoryProxy.h"
#include "RooListProxy.h"
#include "TString.h" 
#include <vector>

class RooArgList ;


class RooMultiEfficiency : public RooAbsPdf {
public:
  // Constructors, assignment etc
  inline RooMultiEfficiency() { 
    // Default constructor
  }
  RooMultiEfficiency(const char *name, const char *title, const RooArgList& effFuncList, const RooAbsCategory& cat, std::vector<TString> sigCatNames);
  RooMultiEfficiency(const RooMultiEfficiency& other, const char* name=0);
  virtual TObject* clone(const char* newname) const { return new RooMultiEfficiency(*this,newname); }
  virtual ~RooMultiEfficiency();

  Int_t getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* rangeName=0) const ;
  Double_t analyticalIntegral(Int_t code, const char* rangeName=0) const ;

protected:

  // Function evaluation
  virtual Double_t evaluate() const ;
  RooCategoryProxy _cat ; // category
  RooListProxy _effFuncList ; // Efficiency modeling functions
  std::vector<TString> _sigCatNames ;   // Name of accept state of accept/reject category

  ClassDef(RooMultiEfficiency,1) // Generic PDF defined by string expression and list of variables
};

#endif
