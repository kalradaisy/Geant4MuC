#include "EventAction.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4AnalysisManager.hh" // NEW: Required to write data!

EventAction::EventAction() : totalEdep_(0), nSteps_(0) {}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    // Reset local variables at the start of every event
    E = 0;     
    x = 0;     
    y = 0;     
    z = 0;     
    costh = 0; 
    finalE = 0;
    finalX = 0;
    finalY = 0;
    finalZ = 0;
    px = 0;
    py = 0;
    pz = 0;
    finalPx = 0;
    finalPy = 0;
    finalPz = 0;
    finalCosth = 0;
    theta = 0;
    phi = 0;
    finalPhi = 0;
    finalPhiDeg = 0;
    totalEdep = 0;

    nSteps = 0;
    nSecondaries = 0;
    auto runAction = const_cast<RunAction*>(
        static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction())
    );
    runAction->secEnergies.clear(); // handles secondary energies
    nGamma = 0;
    nElectron = 0;
    nPositron = 0;
    nProtonSec = 0;
    nNeutron = 0;
    nPionPlus = 0;
    nPionMinus = 0;
    nPionZero = 0;
    nMuonPlus = 0;
    nMuonMinus = 0;
    nTauPlus = 0;
    nTauMinus = 0;
    nKaonPlus = 0;
    nKaonMinus = 0;
    nKaonZero = 0;
    nKaonZeroL = 0;
    nKaonZeroS = 0;


    secTotalE = 0;
    secMeanE = 0;
    secTrackLength = 0;
    secFirstZ = 0;
    secLastZ = 0;
    secFirstX = 0;
    secLastX = 0;
    secFirstY = 0;
    secLastY = 0;

    nBackward = 0;
    nDecay = 0;
    nCompton = 0;
    nPairProd = 0;
    nIonisation = 0;
    nBremsstrahlung = 0;
    nPhotoElectric = 0;
    nAnnihilation = 0;
    targetZ = 0;
    targetA = 0;
    targetPDG = 0;
    fNuInteractions = 0;

    // Reset interaction strings
    interactionType = "None";
    interactionModel = "None";

    eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    primaryPDG = 0;
    particleType = 0;
    primaryFinalPDG = 0;
    primaryOscillationProcessInvoked = 0;
    primaryOscillationFlavorChanged = 0;
    primaryOscillationPDGBefore = 0;
    primaryOscillationPDGAfter = 0;
    nOscillationSteps = 0;

    decisionMade = false;

    nuInteractionProcess = "None";
    allInteractionProcess = "None";
    
    isCC = 0;
    isNC = 0;
    
    outgoingLeptonPDG = 0;
    outgoingLeptonE = 0.0;
    outgoingHadronE = 0.0;
    
    outgoingLeptonPx = 0.0;
    outgoingLeptonPy = 0.0;
    outgoingLeptonPz = 0.0;

    q0 = 0.0;
    Q2 = 0.0;
    W = 0.0;
    xBj = 0.0;
    yBj = 0.0;

    leptonScatteringAngle = 0.0;
    leptonCosTheta = 1.0;
    inelasticity = 0.0;
    showerNSecondaries = 0;


    // Get primary particle info and store it locally
    auto vertex = event->GetPrimaryVertex();
    if(vertex) {
        auto primary = vertex->GetPrimary();
        if(primary) {
            E = primary->GetKineticEnergy();

            /*x = vertex->GetX0();
            y = vertex->GetY0();
            z = vertex->GetZ0();*/

            px = primary->GetPx();
            py = primary->GetPy();
            pz = primary->GetPz();

            G4ThreeVector p(px, py, pz);
            theta = p.theta();
            phi   = p.phi();
        }
    }
}

void EventAction::EndOfEventAction(const G4Event*)
{
    // Get the Analysis Manager
    auto analysisManager = G4AnalysisManager::Instance();

    analysisManager->FillNtupleDColumn(0, 0, E);      // Column 0
    analysisManager->FillNtupleDColumn(0, 1, x);      // Column 1
    analysisManager->FillNtupleDColumn(0, 2, y);      // Column 2
    analysisManager->FillNtupleDColumn(0, 3, z);      // Column 3
    analysisManager->FillNtupleDColumn(0, 4, costh);  // ...
    analysisManager->FillNtupleDColumn(0, 5, vertexX); 
    analysisManager->FillNtupleDColumn(0, 6, vertexY);      
    analysisManager->FillNtupleDColumn(0, 7, vertexZ);      
    analysisManager->FillNtupleDColumn(0, 8, vertexT);

    analysisManager->FillNtupleDColumn(0, 9, finalE);
    analysisManager->FillNtupleDColumn(0, 10, finalX);
    analysisManager->FillNtupleDColumn(0, 11, finalY);
    analysisManager->FillNtupleDColumn(0, 12, finalZ);
    analysisManager->FillNtupleDColumn(0, 13, px);
    analysisManager->FillNtupleDColumn(0, 14, py);
    analysisManager->FillNtupleDColumn(0, 15, pz);
    analysisManager->FillNtupleDColumn(0, 16, finalPx);
    analysisManager->FillNtupleDColumn(0, 17, finalPy);
    analysisManager->FillNtupleDColumn(0, 18, finalPz);
    analysisManager->FillNtupleDColumn(0, 19, finalCosth);
    analysisManager->FillNtupleDColumn(0, 20, theta);
    analysisManager->FillNtupleDColumn(0, 21, phi);
    analysisManager->FillNtupleDColumn(0, 22, finalPhi);
    analysisManager->FillNtupleDColumn(0, 23, finalPhiDeg);
    analysisManager->FillNtupleDColumn(0, 24, totalEdep);

    analysisManager->FillNtupleIColumn(0, 25, nSteps);
    analysisManager->FillNtupleIColumn(0, 26, nSecondaries);
    // Sec Energies is 27, but it is handled entirely in RunAction due to memory
    analysisManager->FillNtupleIColumn(0, 28, nGamma);
    analysisManager->FillNtupleIColumn(0, 29, nElectron);
    analysisManager->FillNtupleIColumn(0, 30, nPositron);
    analysisManager->FillNtupleIColumn(0, 31, nProtonSec);
    analysisManager->FillNtupleIColumn(0, 32, nNeutron);
    analysisManager->FillNtupleIColumn(0, 33, nPionPlus);
    analysisManager->FillNtupleIColumn(0, 34, nPionMinus);
    analysisManager->FillNtupleIColumn(0, 35, nPionZero);
    analysisManager->FillNtupleIColumn(0, 36, nMuonPlus);
    analysisManager->FillNtupleIColumn(0, 37, nMuonMinus);
    analysisManager->FillNtupleIColumn(0, 38, nTauPlus);
    analysisManager->FillNtupleIColumn(0, 39, nTauMinus);
    analysisManager->FillNtupleIColumn(0, 40, nKaonPlus);
    analysisManager->FillNtupleIColumn(0, 41, nKaonMinus);
    analysisManager->FillNtupleIColumn(0, 42, nKaonZero);
    analysisManager->FillNtupleIColumn(0, 43, nKaonZeroL);
    analysisManager->FillNtupleIColumn(0, 44, nKaonZeroS);


    analysisManager->FillNtupleDColumn(0, 45, secTotalE);
    analysisManager->FillNtupleDColumn(0, 46, secMeanE);
    analysisManager->FillNtupleDColumn(0, 47, secTrackLength);
    analysisManager->FillNtupleDColumn(0, 48, secFirstZ);
    analysisManager->FillNtupleDColumn(0, 49, secLastZ);
    analysisManager->FillNtupleDColumn(0, 50, secFirstX);
    analysisManager->FillNtupleDColumn(0, 51, secLastX);
    analysisManager->FillNtupleDColumn(0, 52, secFirstY);
    analysisManager->FillNtupleDColumn(0, 53, secLastY);

    analysisManager->FillNtupleIColumn(0, 54, nBackward);
    analysisManager->FillNtupleIColumn(0, 55, nDecay);
    analysisManager->FillNtupleIColumn(0, 56, nCompton);
    analysisManager->FillNtupleIColumn(0, 57, nPairProd);
    analysisManager->FillNtupleIColumn(0, 58, nIonisation);
    analysisManager->FillNtupleIColumn(0, 59, nBremsstrahlung);
    analysisManager->FillNtupleIColumn(0, 60, nPhotoElectric);
    analysisManager->FillNtupleIColumn(0, 61, nAnnihilation);
    analysisManager->FillNtupleIColumn(0, 62, targetZ);
    analysisManager->FillNtupleIColumn(0, 63, targetA);
    analysisManager->FillNtupleIColumn(0, 64, targetPDG);
    analysisManager->FillNtupleIColumn(0, 65, fNuInteractions);

    analysisManager->FillNtupleSColumn(0, 66, interactionType);
    analysisManager->FillNtupleSColumn(0, 67, interactionModel);
    analysisManager->FillNtupleIColumn(0, 68, eventID);
    analysisManager->FillNtupleIColumn(0, 69, primaryPDG);
    analysisManager->FillNtupleIColumn(0, 70, particleType);
    analysisManager->FillNtupleIColumn(0, 71, primaryFinalPDG);
    analysisManager->FillNtupleIColumn(0, 72, primaryOscillationProcessInvoked);
    analysisManager->FillNtupleIColumn(0, 73, primaryOscillationFlavorChanged);
    analysisManager->FillNtupleIColumn(0, 74, primaryOscillationPDGBefore);
    analysisManager->FillNtupleIColumn(0, 75, primaryOscillationPDGAfter);
    analysisManager->FillNtupleIColumn(0, 76, nOscillationSteps);

    analysisManager->FillNtupleSColumn(0, 77, nuInteractionProcess);
    analysisManager->FillNtupleSColumn(0, 78, allInteractionProcess);
    
    analysisManager->FillNtupleIColumn(0, 79, isCC);
    analysisManager->FillNtupleIColumn(0, 80, isNC);
    analysisManager->FillNtupleIColumn(0, 81, outgoingLeptonPDG);
    
    analysisManager->FillNtupleDColumn(0, 82, outgoingLeptonE);
    analysisManager->FillNtupleDColumn(0, 83, outgoingHadronE);
    analysisManager->FillNtupleDColumn(0, 84, outgoingLeptonPx);
    analysisManager->FillNtupleDColumn(0, 85, outgoingLeptonPy);
    analysisManager->FillNtupleDColumn(0, 86, outgoingLeptonPz);

    analysisManager->FillNtupleDColumn(0, 87, q0);
    analysisManager->FillNtupleDColumn(0, 88, Q2);
    analysisManager->FillNtupleDColumn(0, 89, W);
    analysisManager->FillNtupleDColumn(0, 90, xBj);
    analysisManager->FillNtupleDColumn(0, 91, yBj);

    analysisManager->FillNtupleDColumn(0, 92, leptonScatteringAngle);
    analysisManager->FillNtupleDColumn(0, 93, leptonCosTheta);
    analysisManager->FillNtupleDColumn(0, 94, inelasticity);
    analysisManager->FillNtupleIColumn(0, 95, showerNSecondaries);

    // This replaces fTree->Fill()
    analysisManager->AddNtupleRow(0);
}

