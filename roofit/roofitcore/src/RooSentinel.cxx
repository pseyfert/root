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
\file RooSentinel.cxx
\class RooSentinel
\ingroup Roofitcore

RooSentinel is a special purposes singleton class that terminates
all other RooFit singleton services when the process exists. 

All function RooFit singleton services are created on the heap with
a static wrapper function to avoid the 'static initialization order fiasco'
but are not automatically destroyed at the end of the session. This class
installs an atexit() function that takes care of this
**/

#include "RooArgSet.h"                  // for RooArgSet
#include "RooExpensiveObjectCache.h"    // for RooExpensiveObjectCache
#include "RooMinuit.h"                  // for RooMinuit
#include "RooMsgService.h"              // for RooMsgService
#include "RooNameReg.h"                 // for RooNameReg
#include "RooNumGenConfig.h"            // for RooNumGenConfig
#include "RooNumGenFactory.h"           // for RooNumGenFactory
#include "RooNumIntConfig.h"            // for RooNumIntConfig
#include "RooNumIntFactory.h"           // for RooNumIntFactory
#include "RooRealConstant.h"            // for RooRealConstant
#include "RooResolutionModel.h"         // for RooResolutionModel
#include "RooSentinel.h"                // for RooSentinel
#include "Rtypes.h"                     // for kFALSE, kTRUE
#include "RtypesCore.h"                 // for Bool_t


Bool_t RooSentinel::_active = kFALSE ;

static void CleanUpRooFitAtExit()
{
  // Clean up function called at program termination before global objects go out of scope.
  RooMinuit::cleanup() ;
  RooMsgService::cleanup() ;
  RooNumIntConfig::cleanup() ;
  RooNumGenConfig::cleanup() ;
  RooNumIntFactory::cleanup() ;
  RooNumGenFactory::cleanup() ;
  RooNameReg::cleanup() ;
  RooArgSet::cleanup() ;
  RooRealConstant::cleanup() ;
  RooResolutionModel::cleanup() ;
  RooExpensiveObjectCache::cleanup() ;
}



////////////////////////////////////////////////////////////////////////////////
/// Install atexit handler that calls CleanupRooFitAtExit()
/// on program termination

void RooSentinel::activate()
{
  if (!_active) {
    _active = kTRUE ;
    atexit(CleanUpRooFitAtExit) ;
  }
}


 
