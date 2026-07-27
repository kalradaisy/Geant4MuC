#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "EventAction.hh"


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
    analysisManager->CreateNtupleDColumn(0, "E");      // Column 0
    analysisManager->CreateNtupleDColumn(0, "x");      // Column 1
    analysisManager->CreateNtupleDColumn(0, "y");      // Column 2
    analysisManager->CreateNtupleDColumn(0, "z");      // Column 3
    analysisManager->CreateNtupleDColumn(0, "costh");  // ...
    analysisManager->CreateNtupleDColumn(0, "vertexX");
    analysisManager->CreateNtupleDColumn(0, "vertexY");
    analysisManager->CreateNtupleDColumn(0, "vertexZ");
    analysisManager->CreateNtupleDColumn(0, "vertexT");
    analysisManager->CreateNtupleDColumn(0, "finalE");
    analysisManager->CreateNtupleDColumn(0, "finalX");
    analysisManager->CreateNtupleDColumn(0, "finalY");
    analysisManager->CreateNtupleDColumn(0, "finalZ");
    analysisManager->CreateNtupleDColumn(0, "px");
    analysisManager->CreateNtupleDColumn(0, "py");
    analysisManager->CreateNtupleDColumn(0, "pz");
    analysisManager->CreateNtupleDColumn(0, "finalPx");
    analysisManager->CreateNtupleDColumn(0, "finalPy");
    analysisManager->CreateNtupleDColumn(0, "finalPz");
    analysisManager->CreateNtupleDColumn(0, "finalCosth");
    analysisManager->CreateNtupleDColumn(0, "theta");
    analysisManager->CreateNtupleDColumn(0, "phi");
    analysisManager->CreateNtupleDColumn(0, "finalPhi");
    analysisManager->CreateNtupleDColumn(0, "finalPhiDeg");
    analysisManager->CreateNtupleDColumn(0, "totalEdep");

    analysisManager->CreateNtupleIColumn(0, "nSteps");
    analysisManager->CreateNtupleIColumn(0, "nSecondaries");
    analysisManager->CreateNtupleDColumn(0, "secEnergies", secEnergies);
    // We have to treat this specially because G4 wants vectors defined upfront
    analysisManager->CreateNtupleIColumn(0, "nGamma");
    analysisManager->CreateNtupleIColumn(0, "nElectron");
    analysisManager->CreateNtupleIColumn(0, "nPositron");
    analysisManager->CreateNtupleIColumn(0, "nProtonSec");
    analysisManager->CreateNtupleIColumn(0, "nNeutron");
    analysisManager->CreateNtupleIColumn(0, "nPionPlus");
    analysisManager->CreateNtupleIColumn(0, "nPionMinus");
    analysisManager->CreateNtupleIColumn(0, "nPionZero");
    analysisManager->CreateNtupleIColumn(0, "nMuonPlus");
    analysisManager->CreateNtupleIColumn(0, "nMuonMinus");
    analysisManager->CreateNtupleIColumn(0, "nTauPlus");
    analysisManager->CreateNtupleIColumn(0, "nTauMinus");
    analysisManager->CreateNtupleIColumn(0, "nKaonPlus");
    analysisManager->CreateNtupleIColumn(0, "nKaonMinus");
    analysisManager->CreateNtupleIColumn(0, "nKaonZero");
    analysisManager->CreateNtupleIColumn(0, "nKaonZeroL");
    analysisManager->CreateNtupleIColumn(0, "nKaonZeroS");


    analysisManager->CreateNtupleDColumn(0, "secTotalE");
    analysisManager->CreateNtupleDColumn(0, "secMeanE");
    analysisManager->CreateNtupleDColumn(0, "secTrackLength");
    analysisManager->CreateNtupleDColumn(0, "secFirstZ");
    analysisManager->CreateNtupleDColumn(0, "secLastZ");
    analysisManager->CreateNtupleDColumn(0, "secFirstX");
    analysisManager->CreateNtupleDColumn(0, "secLastX");
    analysisManager->CreateNtupleDColumn(0, "secFirstY");
    analysisManager->CreateNtupleDColumn(0, "secLastY");

    analysisManager->CreateNtupleIColumn(0, "nBackward");
    analysisManager->CreateNtupleIColumn(0, "nDecay");
    analysisManager->CreateNtupleIColumn(0, "nCompton");
    analysisManager->CreateNtupleIColumn(0, "nPairProd");
    analysisManager->CreateNtupleIColumn(0, "nIonisation");
    analysisManager->CreateNtupleIColumn(0, "nBremsstrahlung");
    analysisManager->CreateNtupleIColumn(0, "nPhotoElectric");
    analysisManager->CreateNtupleIColumn(0, "nAnnihilation");
    analysisManager->CreateNtupleIColumn(0, "targetZ");
    analysisManager->CreateNtupleIColumn(0, "targetA");
    analysisManager->CreateNtupleIColumn(0, "targetPDG");
    analysisManager->CreateNtupleIColumn(0, "fNuInteractions");

    analysisManager->CreateNtupleSColumn(0, "interactionType");
    analysisManager->CreateNtupleSColumn(0, "interactionModel");

    analysisManager->CreateNtupleIColumn(0, "eventID");
    analysisManager->CreateNtupleIColumn(0, "primaryPDG");
    analysisManager->CreateNtupleIColumn(0, "particleType");
    analysisManager->CreateNtupleIColumn(0, "primaryFinalPDG");
    analysisManager->CreateNtupleIColumn(0, "primaryOscillationProcessInvoked");
    analysisManager->CreateNtupleIColumn(0, "primaryOscillationFlavorChanged");
    analysisManager->CreateNtupleIColumn(0, "primaryOscillationPDGBefore");
    analysisManager->CreateNtupleIColumn(0, "primaryOscillationPDGAfter");
    analysisManager->CreateNtupleIColumn(0, "nOscillationSteps");

    analysisManager->CreateNtupleSColumn(0, "nuInteractionProcess");
    analysisManager->CreateNtupleSColumn(0, "allInteractionProcess");
    analysisManager->CreateNtupleIColumn(0, "isCC");
    analysisManager->CreateNtupleIColumn(0, "isNC");
    analysisManager->CreateNtupleIColumn(0, "outgoingLeptonPDG");
    analysisManager->CreateNtupleDColumn(0, "outgoingLeptonE");
    analysisManager->CreateNtupleDColumn(0, "outgoingHadronE");
    analysisManager->CreateNtupleDColumn(0, "outgoingLeptonPx");
    analysisManager->CreateNtupleDColumn(0, "outgoingLeptonPy");
    analysisManager->CreateNtupleDColumn(0, "outgoingLeptonPz");

    analysisManager->CreateNtupleDColumn(0, "q0");
    analysisManager->CreateNtupleDColumn(0, "Q2");
    analysisManager->CreateNtupleDColumn(0, "W");
    analysisManager->CreateNtupleDColumn(0, "xBj");
    analysisManager->CreateNtupleDColumn(0, "yBj");

    analysisManager->CreateNtupleDColumn(0, "leptonScatteringAngle");
    analysisManager->CreateNtupleDColumn(0, "leptonCosTheta");      
    analysisManager->CreateNtupleDColumn(0, "inelasticity");         
    analysisManager->CreateNtupleIColumn(0, "showerNSecondaries");   
    
    analysisManager->FinishNtuple(0);

    analysisManager->CreateNtuple("tracks", "3D Step Coordinates");
    analysisManager->CreateNtupleIColumn(1, "eventID");
    analysisManager->CreateNtupleIColumn(1, "step_trackID");
    analysisManager->CreateNtupleIColumn(1, "step_parentID");
    analysisManager->CreateNtupleIColumn(1, "step_PDG");
    analysisManager->CreateNtupleDColumn(1, "step_preX");
    analysisManager->CreateNtupleDColumn(1, "step_preY");
    analysisManager->CreateNtupleDColumn(1, "step_preZ");
    analysisManager->CreateNtupleDColumn(1, "step_postX");
    analysisManager->CreateNtupleDColumn(1, "step_postY");
    analysisManager->CreateNtupleDColumn(1, "step_postZ");
    analysisManager->CreateNtupleDColumn(1, "step_kinE");
    analysisManager->CreateNtupleDColumn(1, "step_edep");
    analysisManager->CreateNtupleSColumn(1, "step_proc");
    analysisManager->CreateNtupleSColumn(1, "step_creatorproc");
    analysisManager->FinishNtuple(1);

    analysisManager->CreateNtuple("trackBirths", "Track Initial Properties");
    analysisManager->CreateNtupleIColumn(2, "eventID");
    analysisManager->CreateNtupleIColumn(2, "trackID");
    analysisManager->CreateNtupleDColumn(2, "trk_birthPosX");
    analysisManager->CreateNtupleDColumn(2, "trk_birthPosY");
    analysisManager->CreateNtupleDColumn(2, "trk_birthPosZ");
    analysisManager->CreateNtupleDColumn(2, "trk_birthKE");
    analysisManager->FinishNtuple(2);
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
