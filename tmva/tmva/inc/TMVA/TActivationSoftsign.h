// @(#)root/tmva $Id$
// Author: Paul Seyfert

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : TActivationSoftsign                                              *
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

#ifndef ROOT_TMVA_TActivationSoftsign
#define ROOT_TMVA_TActivationSoftsign

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TActivationSoftsign                                                  //
//                                                                      //
// softsign activation function for TNeuron                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TString
#include "TString.h"
#endif

#ifndef ROOT_TMVA_TActivation
#include "TMVA/TActivation.h"
#endif

namespace TMVA {
  
   class TActivationSoftsign : public TActivation {
    
   public:

      TActivationSoftsign();
      ~TActivationSoftsign();

      // evaluate the activation function
      Double_t Eval(Double_t arg);

      // evaluate the derivative of the activation function
      Double_t EvalDerivative(Double_t arg);

      // minimum of the range of the activation function
      Double_t GetMin() { return -1; }

      // maximum of the range of the activation function
      Double_t GetMax() { return 1; }

      // expression for the activation function
      TString GetExpression();

      // writer of function code
      virtual void MakeFunction(std::ostream& fout, const TString& fncName);

   private:

      ClassDef(TActivationSoftsign,0) // softsign activation function for TNeuron
   };

} // namespace TMVA

#endif
