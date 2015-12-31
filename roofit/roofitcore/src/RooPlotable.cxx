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
\file RooPlotable.cxx
\class RooPlotable
\ingroup Roofitcore

Class RooPotable is a base class for objects that can be inserted into RooPlots and take
advantage of its internal normalization and axis range adjustment features. The
most useful implementation of RooPlotable are RooHist and RooCurve.
**/

#include <ostream>                      // for operator<<, ostream, etc

#include "RooPlotable.h"                // for RooPlotable
#include "Rtypes.h"                     // for Bool_t, ClassImp, Int_t
#include "TObject.h"                    // for TObject
#include "TString.h"                    // for operator<<, TString

using namespace std;

ClassImp(RooPlotable)
;


////////////////////////////////////////////////////////////////////////////////
/// Print detailed information

void RooPlotable::printMultiline(ostream& os, Int_t /*content*/, Bool_t /*verbose*/, TString indent) const {
  os << indent << "--- RooPlotable ---" << endl;
  os << indent << "  y-axis min = " << getYAxisMin() << endl
     << indent << "  y-axis max = " << getYAxisMax() << endl
     << indent << "  y-axis label \"" << getYAxisLabel() << "\"" << endl;
}


////////////////////////////////////////////////////////////////////////////////
/// Return cast of RooPlotable as TObject. Note that is this a cross
/// cast that will change the pointer value in most cases

TObject *RooPlotable::crossCast() {
  return dynamic_cast<TObject*>(this);
}
