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
\file RooSuperCategory.cxx
\class RooSuperCategory
\ingroup Roofitcore

RooSuperCategory consolidates several RooAbsCategoryLValue objects into
a single category. The states of the super category consist of all the permutations
of the input categories. The super category is an lvalue and requires that
all input categories are lvalues as well as modification
of its state will back propagate into a modification of its input categories.
To define a consolidated category of multiple non-lvalye categories
use class RooMultiCategory
RooSuperCategory state are automatically defined and updated whenever an input
category modifies its list of states
**/

#include <string.h>                     // for strlen, strtok
#include <ostream>                      // for operator<<, basic_ostream, etc
#include "RooAbsArg.h"                  // for RooAbsArg
#include "RooAbsCategory.h"             // for RooAbsCategory
#include "RooAbsCategoryLValue.h"       // for RooAbsCategoryLValue
#include "RooArgSet.h"                  // for RooArgSet
#include "RooCatType.h"                 // for RooCatType
#include "RooMsgService.h"              // for coutE, ccoutE
#include "RooMultiCatIter.h"            // for RooMultiCatIter
#include "RooPrintable.h"               // for operator<<, etc
#include "RooSetProxy.h"                // for RooSetProxy
#include "RooSuperCategory.h"           // for RooSuperCategory
#include "Rtypes.h"                     // for kTRUE, kFALSE, etc
#include "RtypesCore.h"                 // for Bool_t, Int_t
#include "TClass.h"                     // for TClass
#include "TIterator.h"                  // for TIterator
#include "TObjString.h"                 // for TObjString
#include "TString.h"                    // for TString, operator<<
#include "strlcpy.h"                    // for strlcpy

using namespace std;

ClassImp(RooSuperCategory)
;


////////////////////////////////////////////////////////////////////////////////
/// Construct a lvalue product of the given set of input RooAbsCategoryLValues in 'inInputCatList'
/// The state names of this product category are {S1;S2,S3,...Sn} where Si are the state names
/// of the input categories. A RooSuperCategory is an lvalue.

RooSuperCategory::RooSuperCategory(const char *name, const char *title, const RooArgSet& inInputCatList) :
  RooAbsCategoryLValue(name, title), _catSet("input","Input category set",this,kTRUE,kTRUE)
{  
  // Copy category list
  TIterator* iter = inInputCatList.createIterator() ;
  RooAbsArg* arg ;
  while ((arg=(RooAbsArg*)iter->Next())) {
    if (!arg->IsA()->InheritsFrom(RooAbsCategoryLValue::Class())) {
      coutE(InputArguments) << "RooSuperCategory::RooSuperCategory(" << GetName() << "): input category " << arg->GetName() 
			    << " is not an lvalue" << endl ;
    }
    _catSet.add(*arg) ;
  }
  delete iter ;
  _catIter = _catSet.createIterator() ;
  
  updateIndexList() ;
}



////////////////////////////////////////////////////////////////////////////////
/// Copy constructor

RooSuperCategory::RooSuperCategory(const RooSuperCategory& other, const char *name) :
  RooAbsCategoryLValue(other,name), _catSet("input",this,other._catSet)
{
  _catIter = _catSet.createIterator() ;
  updateIndexList() ;
  setIndex(other.getIndex()) ;
}



////////////////////////////////////////////////////////////////////////////////
/// Destructor

RooSuperCategory::~RooSuperCategory() 
{
  delete _catIter ;
}



////////////////////////////////////////////////////////////////////////////////
/// Make an iterator over all state permutations of 
/// the input categories of this supercategory

TIterator* RooSuperCategory::MakeIterator() const 
{
  return new RooMultiCatIter(_catSet) ;
}



////////////////////////////////////////////////////////////////////////////////
/// Update the list of possible states of this super category

void RooSuperCategory::updateIndexList()
{
  clearTypes() ;

  RooMultiCatIter mcIter(_catSet) ;
  TObjString* obj ;
  Int_t i(0) ;
  while((obj = (TObjString*) mcIter.Next())) {
    // Register composite label
    defineTypeUnchecked(obj->String(),i++) ;
  }

  // Renumbering will invalidate cache
  setValueDirty() ;
}



////////////////////////////////////////////////////////////////////////////////
/// Return the name of the current state, 
/// constructed from the state names of the input categories

TString RooSuperCategory::currentLabel() const
{
  _catIter->Reset() ;

  // Construct composite label name
  TString label ;
  RooAbsCategory* cat ;
  Bool_t first(kTRUE) ;
  while((cat=(RooAbsCategory*) _catIter->Next())) {
    label.Append(first?"{":";") ;
    label.Append(cat->getLabel()) ;      
    first=kFALSE ;
  }
  label.Append("}") ;  

  return label ;
}



////////////////////////////////////////////////////////////////////////////////
/// Calculate and return the current value 

RooCatType RooSuperCategory::evaluate() const
{
  if (isShapeDirty()) {
    const_cast<RooSuperCategory*>(this)->updateIndexList() ;
  }
  const RooCatType* ret = lookupType(currentLabel(),kTRUE) ;
  if (!ret) {
    coutE(Eval) << "RooSuperCat::evaluate(" << this << ") error: current state not defined: '" << currentLabel() << "'" << endl ;
    printStream(ccoutE(Eval),0,kVerbose) ;
    return RooCatType() ;
  }
  return *ret ;
}



////////////////////////////////////////////////////////////////////////////////
/// Set the value of the super category by specifying the state index code
/// by setting the states of the corresponding input category lvalues

Bool_t RooSuperCategory::setIndex(Int_t index, Bool_t /*printError*/) 
{
  const RooCatType* type = lookupType(index,kTRUE) ;
  if (!type) return kTRUE ;
  return setType(type) ;
}



////////////////////////////////////////////////////////////////////////////////
/// Set the value of the super category by specifying the state name
/// by setting the state names of the corresponding input category lvalues

Bool_t RooSuperCategory::setLabel(const char* label, Bool_t /*printError*/) 
{
  const RooCatType* type = lookupType(label,kTRUE) ;
  if (!type) return kTRUE ;
  return setType(type) ;
}



////////////////////////////////////////////////////////////////////////////////
/// Set the value of the super category by specifying the state object
/// by setting the state names of the corresponding input category lvalues

Bool_t RooSuperCategory::setType(const RooCatType* type, Bool_t /*printError*/)
{
  char buf[1024] ;
  strlcpy(buf,type->GetName(),1024) ;

  RooAbsCategoryLValue* arg ;
  Bool_t error(kFALSE) ;

  // Parse composite label and set label of components to their values  
  char* ptr=buf+1 ;
  char* token = ptr ;
  _catIter->Reset() ;
  while ((arg=(RooAbsCategoryLValue*)_catIter->Next())) {

    // Delimit name token for this category
    if (*ptr=='{') {
      // Token is composite itself, terminate at matching '}'
      Int_t nBrak(1) ;
      while(*(++ptr)) {
	if (nBrak==0) {
	  *ptr = 0 ;
	  break ;
	}
	if (*ptr=='{') {
	  nBrak++ ;
	} else if (*ptr=='}') {
	  nBrak-- ;
	}
      }	
    } else {
      // Simple token, terminate at next semi-colon
      ptr = strtok(ptr,";}") ;
      ptr += strlen(ptr) ;
    }

    error |= arg->setLabel(token) ;
    token = ++ptr ;
  }
  
  return error ;
}



////////////////////////////////////////////////////////////////////////////////
/// Print the state of this object to the specified output stream.

void RooSuperCategory::printMultiline(ostream& os, Int_t content, Bool_t verbose, TString indent) const
{
  RooAbsCategory::printMultiline(os,content,verbose,indent) ;
  
  if (verbose) {     
    os << indent << "--- RooSuperCategory ---" << endl;
    os << indent << "  Input category list:" << endl ;
    TString moreIndent(indent) ;
    os << moreIndent << _catSet << endl ;
  }
}



////////////////////////////////////////////////////////////////////////////////
/// Read object contents from given stream

Bool_t RooSuperCategory::readFromStream(istream& /*is*/, Bool_t /*compact*/, Bool_t /*verbose*/) 
{
  return kTRUE ;
}



////////////////////////////////////////////////////////////////////////////////
/// Write object contents to given stream

void RooSuperCategory::writeToStream(ostream& os, Bool_t compact) const
{
  RooAbsCategory::writeToStream(os,compact) ;
}



////////////////////////////////////////////////////////////////////////////////
/// Return true of all of the input category states are in the given range

Bool_t RooSuperCategory::inRange(const char* rangeName) const 
{
  _catIter->Reset() ;
  RooAbsCategoryLValue* cat ;
  while((cat = (RooAbsCategoryLValue*)_catIter->Next())) {
    if (!cat->inRange(rangeName)) {
      return kFALSE ;
    }
  }
  return kTRUE ;
}



////////////////////////////////////////////////////////////////////////////////
/// Return true if any of the input categories has a range
/// named 'rangeName'

Bool_t RooSuperCategory::hasRange(const char* rangeName) const 
{
  _catIter->Reset() ;
  RooAbsCategoryLValue* cat ;
  while((cat = (RooAbsCategoryLValue*)_catIter->Next())) {
    if (cat->hasRange(rangeName)) return kTRUE ;
  }

  return kFALSE ;
}
