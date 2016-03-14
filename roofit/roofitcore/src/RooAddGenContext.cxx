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
\file RooAddGenContext.cxx
\class RooAddGenContext
\ingroup Roofitcore

RooAddGenContext is an efficient implementation of the
generator context specific for RooAddPdf PDFs. The strategy
of RooAddGenContext is to defer generation of each component
to a dedicated generator context for that component and to
randomly choose one of those context to generate an event,
with a probability proportional to its associated coefficient
**/


#include <ostream>                      // for operator<<, basic_ostream, etc
#include <vector>                       // for vector, vector<>::iterator, etc

#include "RooAbsArg.h"                  // for operator<<, RooAbsArg, etc
#include "RooAddGenContext.h"           // for RooAddGenContext, etc
#include "RooAddModel.h"                // for RooAddModel
#include "RooAddPdf.h"                  // for RooAbsPdf, RooAddPdf
#include "RooDataSet.h"                 // for RooArgSet, Double_t, Int_t, etc
#include "RooListProxy.h"               // for RooListProxy
#include "RooMsgService.h"              // for ccxcoutI, cxcoutI
#include "RooRandom.h"                  // for RooRandom
#include "TIterator.h"                  // for TIterator
#include "TString.h"                    // for operator<<, TString

using namespace std;

ClassImp(RooAddGenContext)
;
  

////////////////////////////////////////////////////////////////////////////////
/// Constructor

RooAddGenContext::RooAddGenContext(const RooAddPdf &model, const RooArgSet &vars, 
				   const RooDataSet *prototype, const RooArgSet* auxProto,
				   Bool_t verbose) :
  RooAbsGenContext(model,vars,prototype,auxProto,verbose), _isModel(kFALSE)
{
  cxcoutI(Generation) << "RooAddGenContext::ctor() setting up event special generator context for sum p.d.f. " << model.GetName() 
			<< " for generation of observable(s) " << vars ;
  if (prototype) ccxcoutI(Generation) << " with prototype data for " << *prototype->get() ;
  if (auxProto && auxProto->getSize()>0)  ccxcoutI(Generation) << " with auxiliary prototypes " << *auxProto ;
  ccxcoutI(Generation) << endl ;

  // Constructor. Build an array of generator contexts for each product component PDF
  _pdfSet = (RooArgSet*) RooArgSet(model).snapshot(kTRUE) ;
  _pdf = (RooAddPdf*) _pdfSet->find(model.GetName()) ;
  _pdf->setOperMode(RooAbsArg::ADirty,kTRUE) ;

  // Fix normalization set of this RooAddPdf
  if (prototype) 
    {
      RooArgSet coefNSet(vars) ;
      coefNSet.add(*prototype->get()) ;
      _pdf->fixAddCoefNormalization(coefNSet,kFALSE) ;
    }

  model._pdfIter->Reset() ;
  RooAbsPdf* pdf ;
  _nComp = model._pdfList.getSize() ;
  _coefThresh = new Double_t[_nComp+1] ;
  _vars = (RooArgSet*) vars.snapshot(kFALSE) ;

  while((pdf=(RooAbsPdf*)model._pdfIter->Next())) {
    RooAbsGenContext* cx = pdf->genContext(vars,prototype,auxProto,verbose) ;
    _gcList.push_back(cx) ;
  }  

  ((RooAddPdf*)_pdf)->getProjCache(_vars) ;
  _pdf->recursiveRedirectServers(*_theEvent) ;

  _mcache = 0 ;
  _pcache = 0 ;
}



////////////////////////////////////////////////////////////////////////////////
/// Constructor

RooAddGenContext::RooAddGenContext(const RooAddModel &model, const RooArgSet &vars, 
				   const RooDataSet *prototype, const RooArgSet* auxProto,
				   Bool_t verbose) :
  RooAbsGenContext(model,vars,prototype,auxProto,verbose), _isModel(kTRUE)
{
  cxcoutI(Generation) << "RooAddGenContext::ctor() setting up event special generator context for sum resolution model " << model.GetName() 
			<< " for generation of observable(s) " << vars ;
  if (prototype) ccxcoutI(Generation) << " with prototype data for " << *prototype->get() ;
  if (auxProto && auxProto->getSize()>0)  ccxcoutI(Generation) << " with auxiliary prototypes " << *auxProto ;
  ccxcoutI(Generation) << endl ;

  // Constructor. Build an array of generator contexts for each product component PDF
  _pdfSet = (RooArgSet*) RooArgSet(model).snapshot(kTRUE) ;
  _pdf = (RooAbsPdf*) _pdfSet->find(model.GetName()) ;


  model._pdfIter->Reset() ;
  RooAbsPdf* pdf ;
  _nComp = model._pdfList.getSize() ;
  _coefThresh = new Double_t[_nComp+1] ;
  _vars = (RooArgSet*) vars.snapshot(kFALSE) ;

  while((pdf=(RooAbsPdf*)model._pdfIter->Next())) {
    RooAbsGenContext* cx = pdf->genContext(vars,prototype,auxProto,verbose) ;
    _gcList.push_back(cx) ;
  }  

  ((RooAddModel*)_pdf)->getProjCache(_vars) ;
  _pdf->recursiveRedirectServers(*_theEvent) ;

  _mcache = 0 ;
  _pcache = 0 ;
}



////////////////////////////////////////////////////////////////////////////////
/// Destructor. Delete all owned subgenerator contexts

RooAddGenContext::~RooAddGenContext()
{
  delete[] _coefThresh ;
  for (vector<RooAbsGenContext*>::iterator iter=_gcList.begin() ; iter!=_gcList.end() ; ++iter) {
    delete *iter ;
  }
  delete _vars ;
  delete _pdfSet ;
}



////////////////////////////////////////////////////////////////////////////////
/// Attach given set of variables to internal p.d.f. clone

void RooAddGenContext::attach(const RooArgSet& args) 
{
  _pdf->recursiveRedirectServers(args) ;

  // Forward initGenerator call to all components
  for (vector<RooAbsGenContext*>::iterator iter=_gcList.begin() ; iter!=_gcList.end() ; ++iter) {
    (*iter)->attach(args) ;
  }
}



////////////////////////////////////////////////////////////////////////////////
/// One-time initialization of generator contex. Attach theEvent
/// to internal p.d.f clone and forward initialization call to 
/// the component generators

void RooAddGenContext::initGenerator(const RooArgSet &theEvent)
{
  _pdf->recursiveRedirectServers(theEvent) ;

  if (_isModel) {    
    RooAddModel* amod = (RooAddModel*) _pdf ;
    _mcache = amod->getProjCache(_vars) ;
  } else {
    RooAddPdf* apdf = (RooAddPdf*) _pdf ;
    _pcache = apdf->getProjCache(_vars,0,"FULL_RANGE_ADDGENCONTEXT") ;
  }
  
  // Forward initGenerator call to all components
  for (vector<RooAbsGenContext*>::iterator iter=_gcList.begin() ; iter!=_gcList.end() ; ++iter) {
    (*iter)->initGenerator(theEvent) ;
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Randomly choose one of the component contexts to generate this event,
/// with a probability proportional to its coefficient

void RooAddGenContext::generateEvent(RooArgSet &theEvent, Int_t remaining)
{
  // Throw a random number to determin which component to generate
  updateThresholds() ;
  Double_t rand = RooRandom::uniform() ;
  Int_t i=0 ;
  for (i=0 ; i<_nComp ; i++) {
    if (rand>_coefThresh[i] && rand<_coefThresh[i+1]) {
      _gcList[i]->generateEvent(theEvent,remaining) ;
      return ;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////
/// Update the cumulative threshold table from the current coefficient
/// values

void RooAddGenContext::updateThresholds()
{
  if (_isModel) {
    
    RooAddModel* amod = (RooAddModel*) _pdf ;
    amod->updateCoefficients(*_mcache,_vars) ;

    _coefThresh[0] = 0. ;
    Int_t i ;
    for (i=0 ; i<_nComp ; i++) {
      _coefThresh[i+1] = amod->_coefCache[i] ;
      _coefThresh[i+1] += _coefThresh[i] ;
    }

  } else {

    RooAddPdf* apdf = (RooAddPdf*) _pdf ;
    
    apdf->updateCoefficients(*_pcache,_vars) ;
    
    _coefThresh[0] = 0. ;
    Int_t i ;
    for (i=0 ; i<_nComp ; i++) {
      _coefThresh[i+1] = apdf->_coefCache[i] ;
      _coefThresh[i+1] += _coefThresh[i] ;
//       cout << "RooAddGenContext::updateThresholds(" << GetName() << ") _coefThresh[" << i+1 << "] = " << _coefThresh[i+1] << endl ;
    }
    
  }

}


////////////////////////////////////////////////////////////////////////////////
/// Forward the setProtoDataOrder call to the component generator contexts

void RooAddGenContext::setProtoDataOrder(Int_t* lut)
{
  RooAbsGenContext::setProtoDataOrder(lut) ;
  for (vector<RooAbsGenContext*>::iterator iter=_gcList.begin() ; iter!=_gcList.end() ; ++iter) {
    (*iter)->setProtoDataOrder(lut) ;
  }
}



////////////////////////////////////////////////////////////////////////////////
/// Print the details of the context

void RooAddGenContext::printMultiline(ostream &os, Int_t content, Bool_t verbose, TString indent) const 
{
  RooAbsGenContext::printMultiline(os,content,verbose,indent) ;
  os << indent << "--- RooAddGenContext ---" << endl ;
  os << indent << "Using PDF ";
  _pdf->printStream(os,kName|kArgs|kClassName,kSingleLine,indent);
  
  os << indent << "List of component generators" << endl ;
  TString indent2(indent) ;
  indent2.Append("    ") ;
  for (vector<RooAbsGenContext*>::const_iterator iter=_gcList.begin() ; iter!=_gcList.end() ; ++iter) {
    (*iter)->printMultiline(os,content,verbose,indent2) ;
  }
}
