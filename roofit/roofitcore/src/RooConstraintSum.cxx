/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id$
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
\file RooConstraintSum.cxx
\class RooConstraintSum
\ingroup Roofitcore

RooConstraintSum calculates the sum of the -(log) likelihoods of
a set of RooAbsPfs that represent constraint functions. This class
is used to calculate the composite -log(L) of constraints to be
added the regular -log(L) in RooAbsPdf::fitTo() with Constrain(..)
arguments
**/

#include <ostream>                      // for operator<<, basic_ostream, etc
#include "RooAbsArg.h"                  // for RooAbsArg
#include "RooAbsPdf.h"                  // for RooAbsPdf
#include "RooAbsReal.h"                 // for RooAbsReal
#include "RooArgSet.h"                  // for RooArgSet
#include "RooConstraintSum.h"           // for RooConstraintSum
#include "RooErrorHandler.h"            // for RooErrorHandler
#include "RooLinkedListIter.h"          // for RooFIter
#include "RooListProxy.h"               // for RooListProxy
#include "RooMsgService.h"              // for coutE
#include "RooSetProxy.h"                // for RooSetProxy
#include "Rtypes.h"                     // for ClassImp
#include "RtypesCore.h"                 // for Double_t
#include "TIterator.h"                  // for TIterator

using namespace std;

ClassImp(RooConstraintSum)
;


////////////////////////////////////////////////////////////////////////////////
/// Default constructor

RooConstraintSum::RooConstraintSum()
{
  _setIter1 = _set1.createIterator() ;
}




////////////////////////////////////////////////////////////////////////////////
/// Constructor with set of constraint p.d.f.s. All elements in constraintSet must inherit from RooAbsPdf

RooConstraintSum::RooConstraintSum(const char* name, const char* title, const RooArgSet& constraintSet, const RooArgSet& normSet) :
  RooAbsReal(name, title),
  _set1("set1","First set of components",this),
  _paramSet("paramSet","Set of parameters",this)
{

  _setIter1 = _set1.createIterator() ;

  TIterator* inputIter = constraintSet.createIterator() ;
  RooAbsArg* comp ;
  while((comp = (RooAbsArg*)inputIter->Next())) {
    if (!dynamic_cast<RooAbsPdf*>(comp)) {
      coutE(InputArguments) << "RooConstraintSum::ctor(" << GetName() << ") ERROR: component " << comp->GetName() 
			    << " is not of type RooAbsPdf" << endl ;
      RooErrorHandler::softAbort() ;
    }
    _set1.add(*comp) ;
  }

  _paramSet.add(normSet) ;

  delete inputIter ;
}





////////////////////////////////////////////////////////////////////////////////
/// Copy constructor

RooConstraintSum::RooConstraintSum(const RooConstraintSum& other, const char* name) :
  RooAbsReal(other, name), 
  _set1("set1",this,other._set1),
  _paramSet("paramSet",this,other._paramSet)
{
  _setIter1 = _set1.createIterator() ;  
}



////////////////////////////////////////////////////////////////////////////////
/// Destructor

RooConstraintSum::~RooConstraintSum() 
{
  if (_setIter1) delete _setIter1 ;
}



////////////////////////////////////////////////////////////////////////////////
/// Return sum of -log of constraint p.d.f.s

Double_t RooConstraintSum::evaluate() const 
{
  Double_t sum(0);
  RooAbsReal* comp ;
  RooFIter setIter1 = _set1.fwdIterator() ;

  while((comp=(RooAbsReal*)setIter1.next())) {
    sum -= ((RooAbsPdf*)comp)->getLogVal(&_paramSet) ;
  }
  
  return sum ;
}

