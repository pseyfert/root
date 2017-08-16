#pragma once
#include <stdio.h>
#include <string.h>

//#define branchadder_use_blacklist 1
//#include "blacklist.h"
#include <unordered_set>

// at least g++ 4.9 or clang++
// FIXME: use TRegex
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40900 || __clang__
#define branchadder_use_regex 1
#include <regex>
#else // older g++
#warning "Building test if branch name is a valid c++ variable name"
#endif // compiler version

#include "TString.h"
#include "TTree.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TMVA/Reader.h"
#include "TTreeFormula.h"

namespace TMVA {

  class BranchAdder {
    public:
      class VariableWrapper {
        public:
          VariableWrapper(TString form) : formula(form), value(0.f), ttreeformula(nullptr){};
          VariableWrapper() : formula(""), value(-999.f), ttreeformula(nullptr){};
          TString formula;
          Float_t value;
          TTreeFormula *ttreeformula;
      };
      //// new interface
      void CopyDirectoryStack() { m_dirstack = true; }
      int SetTargetFile(TDirectoryFile *file);
      int SetTargetBranch(TString name);
      int SetXMLFile(TString filename);
      int SetTree(TObject *tree);
      int SetOutTree(TTree *tree);
      int SetTree(TTree *tree);
      TTree *GetOutTree() { return m_outtree;}
      void Close();
      int Process();
      std::unordered_set<TBranch *> getBranches();

    protected:
      TString m_xmlfilename;
      TString m_targetbranchname;
      int check_all_initialised();
      //// old interface
      TString m_methodName;
      std::vector<VariableWrapper> m_variables;
      std::vector<VariableWrapper> m_spectators;
      std::vector<TTreeFormula *> m_formulas;
      TTree *m_intree;
      TTree *m_outtree;
      TMVA::Reader *m_reader;
      std::unordered_set<TBranch *> m_branches;
      Float_t m_response;
      TBranch *m_responseBranch;
      TFile *m_infile;
      TDirectoryFile *m_outfile;
      bool m_dirstack;
      bool m_regression;

    public:
      int getVariables(TString);
      int getVariables();
      int bookReader(TString);
      int bookReader();
      int activateBranches();
      int createTree();
      int initFormulas(TString, bool);
      int initFormulas(bool eval_on_in);
      int getTree(TString, TString, TString);
      int GetEntry(Long64_t);
      int Evaluate();
      BranchAdder();
      virtual ~BranchAdder();
  };
}
