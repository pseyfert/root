/*
 * Example usage for the TMVA::BranchAdder
 * 
 * simple usages can be done on the command line with
 *
 * BranchAdderExe <root file> <tree name> <xml file> <output file> [<target branch name>]
 *
 *
 * The <tree name> can be anything that the TFile::Get can parse (i.e.
 * directory names <directory>/<tree> as used in rootls work). The entire tree
 * (even disabled branches will be copied to the output file, and a new branch
 * with name <target branch name> is added which contains the response of the
 * TMVA method. Simple classification and regression are supported.
 * If no target branch name is provided, it is extracted from the xml file.
 *
 * In cases where a user wants to do more than just adding a single TMVA
 * method, the c++ class BranchAdder might be more useful. It can be added to a
 * Loop() method from MakeClass.
 *
 * The example here can be used with the output from TMVAClassification.C with:
 *
 *    root -l ./TMVAAdvancedBranchAdder.C\(\"dataset/weights/TMVAClassification_PDERS.weights.xml,dataset/weights/TMVAClassification_MLP.weights.xml\"\)
 *
 *
 */
#include <vector>
#include "TMVA/BranchAdder.h"
#include "TString.h"
#include "TEntryList.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TSystem.h"
#include "TMVA/Tools.h"

int TMVAAdvancedBranchAdder(TString myMethodList = "") {
  std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );

  std::vector<TMVA::BranchAdder> tmva_methods;
  std::set<TBranch*> m_branches;


  TFile *input(0);
  TString fname = "./tmva_class_example.root";
  if (!gSystem->AccessPathName( fname )) {
    input = TFile::Open( fname ); // check if file in local directory exists
  }  
  else {
    TFile::SetCacheFileDir(".");
    input = TFile::Open("http://root.cern.ch/files/tmva_class_example.root", "CACHEREAD"); // if not: download from ROOT server
  }
  if (!input) {
    std::cout << "ERROR: could not open data file" << std::endl;
    exit(1);
  }
  TTree* tree = (TTree*)input->Get("TreeS");
	Float_t var1, var2;
	Float_t userVar1, userVar2;
  TBranch* inputBranch1;
  TBranch* inputBranch2;
	tree->SetBranchAddress( "var1", &userVar1, &inputBranch1 );
	tree->SetBranchAddress( "var2", &userVar2, &inputBranch2 );
  m_branches.insert(inputBranch1);
  m_branches.insert(inputBranch2);

  TEntryList *entryList = new TEntryList("mysel","mysel",tree);
  tree->Draw(">>mysel","var1+var2+var3+var4*0.01>-13.5","entrylist");
  tree->SetEntryList(entryList);


	TTree* outputTree = new TTree("outputTree","outputTree");
  outputTree->Branch("var1",&var1);
  outputTree->Branch("var2",&var2);
  
  for (auto s: mlist) {
    TMVA::BranchAdder temp;
    temp.SetTree(tree);
    temp.SetXMLFile(s.Data());
    tmva_methods.push_back(temp);
  }

  int tmva_method_errorcode = 0;
  for (auto& tmva_method: tmva_methods) {
    tmva_method_errorcode |= tmva_method.getVariables();
    tmva_method_errorcode |= tmva_method.bookReader();
    tmva_method_errorcode |= tmva_method.SetOutTree(outputTree);
    tmva_method_errorcode |= tmva_method.initFormulas(true);
    for (auto branch: tmva_method.getBranches()) {
      m_branches.insert(branch);
    }
    if (tmva_method_errorcode) {
      std::cerr << "could not initialise method" << std::endl;
      exit(tmva_method_errorcode);
    }
  }
  tree->SetBranchStatus("*",0);
  for (auto b: m_branches) b->SetStatus(1);
  for (auto& tmva_method: tmva_methods) {
    tmva_method_errorcode |= tmva_method.activateBranches();
    if (tmva_method_errorcode) {
      std::cerr << "cannot activate branches" << std::endl;
      exit(tmva_method_errorcode);
    }
  }

  TFile* outputFile = TFile::Open("test_output.root","recreate");
  if (nullptr == outputFile) {
    std::cout << "could not open outputfile" << std::endl;
    exit(1);
  }


	Long64_t entries = tree->GetEntries();

	for (Long64_t i = 0 ; ; ++i) {
		Long64_t entryNumber = tree->GetEntryNumber(i);
		if (entryNumber < 0) break;
		Long64_t localEntry = tree->LoadTree(entryNumber);
		if (localEntry < 0) break;

		/// could as well do tree->GetEntry(localEntry);
		for (auto b: m_branches) {
			b->GetEntry(localEntry);
		}
		var1 = userVar1 + userVar2;
		var2 = userVar1 - userVar2;

		for (auto& tmva_method: tmva_methods) {
			tmva_method.Evaluate();
		}

		outputTree->Fill();
	}

  outputFile->WriteTObject(outputTree);
  outputFile->Close();

  return 0;
}

int main( int argc, char** argv )
{
   // Select methods (don't look at this code - not of interest)
   TString methodList;
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(",");
      methodList += regMethod;
   }
   return TMVAAdvancedBranchAdder(methodList);
}

