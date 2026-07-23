#include "EventAction.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
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


    // Get primary particle info and store it locally
    auto vertex = event->GetPrimaryVertex();
    if(vertex) {
        auto primary = vertex->GetPrimary();
        if(primary) {
            E = primary->GetKineticEnergy();

            x = vertex->GetX0();
            y = vertex->GetY0();
            z = vertex->GetZ0();

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

    analysisManager->FillNtupleDColumn(0, E);      // Column 0
    analysisManager->FillNtupleDColumn(1, x);      // Column 1
    analysisManager->FillNtupleDColumn(2, y);      // Column 2
    analysisManager->FillNtupleDColumn(3, z);      // Column 3
    analysisManager->FillNtupleDColumn(4, costh);  // ...
    analysisManager->FillNtupleDColumn(5, finalE);
    analysisManager->FillNtupleDColumn(6, finalX);
    analysisManager->FillNtupleDColumn(7, finalY);
    analysisManager->FillNtupleDColumn(8, finalZ);
    analysisManager->FillNtupleDColumn(9, px);
    analysisManager->FillNtupleDColumn(10, py);
    analysisManager->FillNtupleDColumn(11, pz);
    analysisManager->FillNtupleDColumn(12, finalPx);
    analysisManager->FillNtupleDColumn(13, finalPy);
    analysisManager->FillNtupleDColumn(14, finalPz);
    analysisManager->FillNtupleDColumn(15, finalCosth);
    analysisManager->FillNtupleDColumn(16, theta);
    analysisManager->FillNtupleDColumn(17, phi);
    analysisManager->FillNtupleDColumn(18, finalPhi);
    analysisManager->FillNtupleDColumn(19, finalPhiDeg);
    analysisManager->FillNtupleDColumn(20, totalEdep);

    analysisManager->FillNtupleIColumn(21, nSteps);
    analysisManager->FillNtupleIColumn(22, nSecondaries);
    analysisManager->FillNtupleIColumn(23, nGamma);
    analysisManager->FillNtupleIColumn(24, nElectron);
    analysisManager->FillNtupleIColumn(25, nPositron);
    analysisManager->FillNtupleIColumn(26, nProtonSec);
    analysisManager->FillNtupleIColumn(27, nNeutron);
    analysisManager->FillNtupleIColumn(28, nPionPlus);
    analysisManager->FillNtupleIColumn(29, nPionMinus);
    analysisManager->FillNtupleIColumn(30, nPionZero);
    analysisManager->FillNtupleIColumn(31, nMuonPlus);
    analysisManager->FillNtupleIColumn(32, nMuonMinus);
    analysisManager->FillNtupleIColumn(33, nTauPlus);
    analysisManager->FillNtupleIColumn(34, nTauMinus);
    analysisManager->FillNtupleIColumn(35, nKaonPlus);
    analysisManager->FillNtupleIColumn(36, nKaonMinus);
    analysisManager->FillNtupleIColumn(37, nKaonZero);
    analysisManager->FillNtupleIColumn(38, nKaonZeroL);
    analysisManager->FillNtupleIColumn(39, nKaonZeroS);


    analysisManager->FillNtupleDColumn(40, secTotalE);
    analysisManager->FillNtupleDColumn(41, secMeanE);
    analysisManager->FillNtupleDColumn(42, secTrackLength);
    analysisManager->FillNtupleDColumn(43, secFirstZ);
    analysisManager->FillNtupleDColumn(44, secLastZ);
    analysisManager->FillNtupleDColumn(45, secFirstX);
    analysisManager->FillNtupleDColumn(46, secLastX);
    analysisManager->FillNtupleDColumn(47, secFirstY);
    analysisManager->FillNtupleDColumn(48, secLastY);

    analysisManager->FillNtupleIColumn(49, nBackward);
    analysisManager->FillNtupleIColumn(50, nDecay);
    analysisManager->FillNtupleIColumn(51, nCompton);
    analysisManager->FillNtupleIColumn(52, nPairProd);
    analysisManager->FillNtupleIColumn(53, nIonisation);
    analysisManager->FillNtupleIColumn(54, nBremsstrahlung);
    analysisManager->FillNtupleIColumn(55, nPhotoElectric);
    analysisManager->FillNtupleIColumn(56, nAnnihilation);
    analysisManager->FillNtupleIColumn(57, targetZ);
    analysisManager->FillNtupleIColumn(58, targetA);
    analysisManager->FillNtupleIColumn(59, targetPDG);
    analysisManager->FillNtupleIColumn(60, fNuInteractions);

    // Add the row! This replaces fTree->Fill()
    analysisManager->AddNtupleRow();
}

