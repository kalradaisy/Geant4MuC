#include "RunAction.hh"
#include "G4Run.hh"


RunAction::RunAction() : G4UserRunAction(), fBaseName("")
{
    // Get analysis manager to manage our analysis
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Name our file and define its type
    analysisManager->SetDefaultFileType("root");
    //analysisManager->SetFileName("neutrino");

    // Tells Geant4 to automatically merge thread files
    analysisManager->SetNtupleMerging(true);

    // Rather than using TTree, we're going to use what Geant4 prefers, Ntuples
    analysisManager->CreateNtuple("tree", "Neutrino data");
    
    // Create the branches, similar to standard TTree versions
    // NOTE: AnalysisManager assigns them an ID starting from 0, 1, 2, 3...
    analysisManager->CreateNtupleDColumn("E");      // Column 0
    analysisManager->CreateNtupleDColumn("x");      // Column 1
    analysisManager->CreateNtupleDColumn("y");      // Column 2
    analysisManager->CreateNtupleDColumn("z");      // Column 3
    analysisManager->CreateNtupleDColumn("costh");  // ...
    analysisManager->CreateNtupleDColumn("finalE");
    analysisManager->CreateNtupleDColumn("finalX");
    analysisManager->CreateNtupleDColumn("finalY");
    analysisManager->CreateNtupleDColumn("finalZ");
    analysisManager->CreateNtupleDColumn("px");
    analysisManager->CreateNtupleDColumn("py");
    analysisManager->CreateNtupleDColumn("pz");
    analysisManager->CreateNtupleDColumn("finalPx");
    analysisManager->CreateNtupleDColumn("finalPy");
    analysisManager->CreateNtupleDColumn("finalPz");
    analysisManager->CreateNtupleDColumn("finalCosth");
    analysisManager->CreateNtupleDColumn("theta");
    analysisManager->CreateNtupleDColumn("phi");
    analysisManager->CreateNtupleDColumn("finalPhi");
    analysisManager->CreateNtupleDColumn("finalPhiDeg");
    analysisManager->CreateNtupleDColumn("totalEdep");

    analysisManager->CreateNtupleIColumn("nSteps");
    analysisManager->CreateNtupleIColumn("nSecondaries");
    analysisManager->CreateNtupleIColumn("nGamma");
    analysisManager->CreateNtupleIColumn("nElectron");
    analysisManager->CreateNtupleIColumn("nPositron");
    analysisManager->CreateNtupleIColumn("nProtonSec");
    analysisManager->CreateNtupleIColumn("nNeutron");
    analysisManager->CreateNtupleIColumn("nPionPlus");
    analysisManager->CreateNtupleIColumn("nPionMinus");
    analysisManager->CreateNtupleIColumn("nPionZero");
    analysisManager->CreateNtupleIColumn("nMuonPlus");
    analysisManager->CreateNtupleIColumn("nMuonMinus");
    analysisManager->CreateNtupleIColumn("nTauPlus");
    analysisManager->CreateNtupleIColumn("nTauMinus");
    analysisManager->CreateNtupleIColumn("nKaonPlus");
    analysisManager->CreateNtupleIColumn("nKaonMinus");
    analysisManager->CreateNtupleIColumn("nKaonZero");
    analysisManager->CreateNtupleIColumn("nKaonZeroL");
    analysisManager->CreateNtupleIColumn("nKaonZeroS");


    analysisManager->CreateNtupleDColumn("secTotalE");
    analysisManager->CreateNtupleDColumn("secMeanE");
    analysisManager->CreateNtupleDColumn("secTrackLength");
    analysisManager->CreateNtupleDColumn("secFirstZ");
    analysisManager->CreateNtupleDColumn("secLastZ");
    analysisManager->CreateNtupleDColumn("secFirstX");
    analysisManager->CreateNtupleDColumn("secLastX");
    analysisManager->CreateNtupleDColumn("secFirstY");
    analysisManager->CreateNtupleDColumn("secLastY");

    analysisManager->CreateNtupleIColumn("nBackward");
    analysisManager->CreateNtupleIColumn("nDecay");
    analysisManager->CreateNtupleIColumn("nCompton");
    analysisManager->CreateNtupleIColumn("nPairProd");
    analysisManager->CreateNtupleIColumn("nIonisation");
    analysisManager->CreateNtupleIColumn("nBremsstrahlung");
    analysisManager->CreateNtupleIColumn("nPhotoElectric");
    analysisManager->CreateNtupleIColumn("nAnnihilation");
    analysisManager->CreateNtupleIColumn("targetZ");
    analysisManager->CreateNtupleIColumn("targetA");
    analysisManager->CreateNtupleIColumn("targetPDG");
    analysisManager->CreateNtupleIColumn("fNuInteractions");
    
    analysisManager->FinishNtuple(0);

    analysisManager->CreateNtuple("tracks", "3D Step Coordinates");
    analysisManager->CreateNtupleIColumn(1, "eventID");
    analysisManager->CreateNtupleIColumn(1, "trackID");
    analysisManager->CreateNtupleIColumn(1, "parentID");
    analysisManager->CreateNtupleIColumn(1, "pdg");
    analysisManager->CreateNtupleDColumn(1, "x");
    analysisManager->CreateNtupleDColumn(1, "y");
    analysisManager->CreateNtupleDColumn(1, "z");
    analysisManager->CreateNtupleDColumn(1, "edep");
    analysisManager->FinishNtuple(1);
}

RunAction::~RunAction() 
{}

void RunAction::BeginOfRunAction(const G4Run* run) 
{
    auto analysisManager = G4AnalysisManager::Instance();
    // capture the base name on the very first run (Run 0)
    // We only do this on Run 0 so we don't accidentally grab a name 
    // that already has a number appended to it!
    if (run->GetRunID() == 0) {
        fBaseName = analysisManager->GetFileName();
        
        // A fallback just in case you forget to set it in the macro
        if (fBaseName.empty()) fBaseName = "neutrino_data"; 
    }

    // Combine the base name with the Run ID if doing many runs
    G4String finalName = fBaseName + "_" + std::to_string(run->GetRunID());
    // Open the file at the start of the run
    analysisManager->OpenFile(finalName);
}

void RunAction::EndOfRunAction(const G4Run*) 
{
    auto analysisManager = G4AnalysisManager::Instance();
    // Safely merge and write the file at the end of the run
    analysisManager->Write();
    analysisManager->CloseFile();
}
