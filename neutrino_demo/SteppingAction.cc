#include "SteppingAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4Ions.hh"
#include "G4SystemOfUnits.hh"
#include "G4HadronicProcess.hh"
#include "G4Nucleus.hh"
#include "G4IonTable.hh"       
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
: fEventAction(eventAction), fRunAction(runAction) {}


void SteppingAction::UserSteppingAction(const G4Step* step) 
{
    if(!fRunAction || !fEventAction) return;

    auto track = step->GetTrack();

    if (track->GetParticleDefinition() == G4NeutrinoMu::NeutrinoMu()) {
    
    // 1. Did the process name contain muNuNucleus?
    const G4VProcess* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    if (proc && proc->GetProcessName().find("muNuNucleus") != std::string::npos) {
        fEventAction->AddNuInteraction();
        
        G4cout << "\n--- NEUTRINO INTERACTION CONFIRMED ---" << G4endl;
        G4cout << "Process: " << proc->GetProcessName() << G4endl;
        
        // 2. Did the neutrino die?
        if (track->GetTrackStatus() == fStopAndKill) {
            G4cout << "Status: Incident Neutrino was killed." << G4endl;
        }

        // 3. What particles were created?
        const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
        G4cout << "Energy Deposited in Step: " << step->GetTotalEnergyDeposit() << " MeV" << G4endl;
        G4cout << "Secondaries produced: " << secondaries->size() << G4endl;
    
        for (size_t i = 0; i < secondaries->size(); ++i) {
            G4cout << "  -> " << (*secondaries)[i]->GetParticleDefinition()->GetParticleName() 
                   << " (Energy: " << (*secondaries)[i]->GetKineticEnergy() << " MeV)" << G4endl;
        }
        G4cout << "--------------------------------------\n" << G4endl;
        }
    }

    auto material = step->GetPreStepPoint()->GetMaterial();
    auto element = material->GetElement(0); // first element

 
    // -----------------------------

    // Accumulate total energy deposition and steps for all tracks
    // -----------------------------
    fEventAction->AddEdep(step->GetTotalEnergyDeposit());
    fEventAction->IncrementStep();

    auto particle = track->GetDefinition();
    
    // Skip invalid nuclei
    if (particle->GetParticleType() == "nucleus") {
      auto ion = dynamic_cast<const G4Ions*>(particle);
      if (ion && (ion->GetAtomicNumber() <= 0 || ion->GetAtomicMass() <= 0)) {
        return;
      }
    }
    
    
    // -----------------------------
    // Handle secondaries
    // -----------------------------
    if(track->GetParentID() > 0 && track->GetCurrentStepNumber() == 1) {
        fEventAction->AddSecondary();

        // initial kinetic energy of secondary
        double Esec = track->GetKineticEnergy();
        if(Esec > 1*keV) fEventAction->AddSecE(Esec);

        // Particle type
        auto name = track->GetDefinition()->GetParticleName();

        int pdg = track->GetDefinition()->GetPDGEncoding();
        switch(pdg) {
            case 22:    fEventAction->AddGamma();      // gamma
            case 11:    fEventAction->AddElectron();   // e-
            case -11:   fEventAction->AddPositron();   // e+
            case 2212:  fEventAction->AddProtonSec();  // proton
            case 2112:  fEventAction->AddNeutron();    // neutron
            case 211:   fEventAction->AddPionPlus();   // pi+
            case -211:  fEventAction->AddPionMinus();  // pi-
            case 111:   fEventAction->AddPionZero();   // pi0
            case 13:    fEventAction->AddMuonMinus();  // mu-
            case -15:   fEventAction->AddTauPlus(); // tau+
            case 15:    fEventAction->AddTauMinus(); // tau-
            case 321:   fEventAction->AddKaonPlus(); // kaon+
            case -321:  fEventAction->AddKaonMinus(); // kaon-
            case 311:   fEventAction->AddKaonZero(); // kaon0
            case 310:   fEventAction->AddKaonZeroL(); // kaon0 Long
            case 130:   fEventAction->AddKaonZeroS(); // kaon0 short
            case -13:   fEventAction->AddMuonPlus(); // mu+
            }
        
        // Z position
        fEventAction->SetFirstLastXYZ(track->GetPosition());

        // Backward tracks
        fEventAction->CountBackTracks(track->GetMomentumDirection());

        // Creator process
        auto proc = track->GetCreatorProcess();
        if(proc) {
            auto pname = proc->GetProcessName();
        //	     G4cout << proc->GetProcessName() << G4endl;

        if(pname == "compt") fEventAction->AddCompton();
        if(pname == "conv")  fEventAction->AddPairProd();
        if(pname == "eIoni") fEventAction->AddIonisation();
        if(pname == "eBrem") fEventAction->AddBrem(); //RunAction->nIonisation++;
        if (pname == "phot")     fEventAction->AddPhotoEl();  
        if (pname == "annihil")  fEventAction->AddAnnihilation();  
        if(pname == "Decay") fEventAction->AddDecay();
        }
    }

    if (step->GetPostStepPoint()->GetStepStatus() == fPostStepDoItProc) {

        auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();

        if (proc) {
            auto pname = proc->GetProcessName();
            //G4cout << "PROCESS: " << pname << G4endl; // debugging print

            if (pname == "nu_eNuclear" ||
                pname == "nu_muNuclear" ||
                pname == "nu_tauNuclear") {

                G4cout << "\n*** NEUTRINO INTERACTION ***\n"
                    << "Particle: "
                    << step->GetTrack()->GetParticleDefinition()->GetParticleName()
                    << "\nProcess: " << pname << G4endl;
            }
        }
    }

    auto proc1 = step->GetPostStepPoint()->GetProcessDefinedStep();

    if(proc1)
    {
        // Try to cast to hadronic process
        auto hadProc = dynamic_cast<const G4HadronicProcess*>(proc1);

        if(hadProc)
        {
            const G4Nucleus* target = hadProc->GetTargetNucleus();
            G4String pname = proc1->GetProcessName();
            //G4cout << pname << G4endl;
            // debug print out
            auto particle = step->GetTrack()->GetParticleDefinition()->GetParticleName();

            // crude but effective check
            if (pname.find("Nu") != std::string::npos ||
                pname.find("nu") != std::string::npos) {

                fEventAction->AddNuInteraction();

                /*G4cout << "*** NEUTRINO INTERACTION EVENT ***\n"
                    << "Process: " << pname << "\n"
                    << "Particle: " << particle << G4endl;*/
            }

            if(target)
            {
                int Z = target->GetZ_asInt();
                int A = target->GetA_asInt();

            //            G4cout << "Struck nucleus: Z=" << Z << " A=" << A << G4endl;

            // Construct PDG code for the nucleus
                int pdg = 1000000000 + 10000*Z + 10*A;

                
                // Save in RunAction if you want

                fEventAction->SetTargetZ(Z);
                fEventAction->SetTargetA(A);
                fEventAction->SetTargetPDG(pdg);

        }
        }
    }

auto secondaries = step->GetSecondaryInCurrentStep();

/*
if(secondaries && secondaries->size() > 0) {
    for (auto sec : *secondaries) {
        G4cout << "Secondary: "
               << sec->GetDefinition()->GetParticleName()
               << G4endl;
    }
}
*/
// above is a debugging print out for seconadry analysis.    
    // Track length accumulation (all tracks)
    fEventAction->AddSecTrackLength(step->GetStepLength());

    // -----------------------------
    // Primary particle information (ID = 0)
    // -----------------------------

    if(track->GetParentID() == 0) {

    // Save initial info at first step
    if(fEventAction->ReadE() == 0) {
        fEventAction->SetInitialKinematics(
            track->GetKineticEnergy(),
            track->GetPosition(),
            track->GetMomentum()
        );
    }

    // Update final info only at track end
    if(track->GetTrackStatus() == fStopAndKill) {
        fEventAction->SetFinalKinematics(
            track->GetKineticEnergy(),
            track->GetPosition()
        );

        // Recompute final direction
        fEventAction->SetFinalMomentum(track->GetMomentum());
        }
    }
    // Save step position for offline 3D visualization
    auto analysisManager = G4AnalysisManager::Instance();
    G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
    int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, track->GetTrackID());
    analysisManager->FillNtupleIColumn(1, 2, track->GetParentID());
    analysisManager->FillNtupleIColumn(1, 3, track->GetDefinition()->GetPDGEncoding());
    analysisManager->FillNtupleDColumn(1, 4, pos.x() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 5, pos.y() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 6, pos.z() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 7, step->GetTotalEnergyDeposit() / CLHEP::MeV);
    
    analysisManager->AddNtupleRow(1); // Adds row specifically to Ntuple 1 ("tracks")
}