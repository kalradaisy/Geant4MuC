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

    auto material = step->GetPreStepPoint()->GetMaterial();
    auto element = material->GetElement(0); // first element

    // -----------------------------
    // Primary Neutrino Interaction Classification
    // -----------------------------
    int pdg = track->GetDefinition()->GetPDGEncoding();
    bool isPrimaryNeutrino = track->GetParentID() == 0 && 
                             (std::abs(pdg) == 12 || std::abs(pdg) == 14 || std::abs(pdg) == 16);
                             
    const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();

    std::string nuProcName = process ? std::string(process->GetProcessName()) : "None";

    // Clean up the process name by stripping out "biasWrapper()"
    std::string biasPrefix = "biasWrapper(";
    if (nuProcName.find(biasPrefix) == 0 && nuProcName.back() == ')') {
        // Extract just the core process name
        nuProcName = nuProcName.substr(biasPrefix.length(), nuProcName.length() - biasPrefix.length() - 1);
    }

    // ========== NEUTRINO OSCILLATION TRACKING ==========
    bool isOscillationProcess = (nuProcName.find("Oscillation") != std::string::npos || 
                                 nuProcName.find("oscillation") != std::string::npos);

    if (isPrimaryNeutrino && isOscillationProcess) {
        fEventAction->nOscillationSteps++;

        if (!fEventAction->primaryOscillationProcessInvoked) {
            fEventAction->primaryOscillationProcessInvoked = 1;
            fEventAction->primaryOscillationPDGBefore = pdg;

            // Search the secondaries for the new oscillated neutrino flavor
            int postOscPDG = pdg; 
            const auto* oscSecondaries = step->GetSecondaryInCurrentStep();
            if (oscSecondaries) {
                for (auto sec : *oscSecondaries) {
                    int secPDG = sec->GetDefinition()->GetPDGEncoding();
                    int absSecPDG = std::abs(secPDG);
                    if (absSecPDG == 12 || absSecPDG == 14 || absSecPDG == 16) {
                        postOscPDG = secPDG;
                        break;
                    }
                }
            }

            fEventAction->primaryOscillationPDGAfter = postOscPDG;
            fEventAction->primaryOscillationFlavorChanged = (postOscPDG != pdg) ? 1 : 0;
            
            // Update the final PDG to the newly oscillated flavor
            fEventAction->primaryFinalPDG = postOscPDG;
        }
    }

    // Only process this once per event at the exact moment of the first real interaction
    if (!fEventAction->decisionMade && isPrimaryNeutrino && process && nuProcName != "Transportation") {
        
        // Record the clean process name
        fEventAction->nuInteractionProcess = nuProcName;
        fEventAction->eventWeight = track->GetWeight();
        fEventAction->allInteractionProcess = nuProcName;

        // Calculate the expected charged lepton PDG based on the incoming neutrino flavor
        int expectedLepton = 0;
        if (std::abs(pdg) == 12) expectedLepton = (pdg > 0) ? 11 : -11; // nu_e -> e-
        if (std::abs(pdg) == 14) expectedLepton = (pdg > 0) ? 13 : -13; // nu_mu -> mu-
        if (std::abs(pdg) == 16) expectedLepton = (pdg > 0) ? 15 : -15; // nu_tau -> tau-

        bool foundCCLepton = false;
        bool foundOutNu = false;
        bool foundHadron = false;

        // Particle counters for interaction model topology
        int nPions = 0;
        int nNucleons = 0;

        // Analyze the particles produced in this interaction
        const auto* secondaries = step->GetSecondaryInCurrentStep();
        if (secondaries) {
            for (auto sec : *secondaries) {
                
                // Ensure the secondary was born from this specific primary track
                if (sec->GetParentID() != track->GetTrackID()) continue;

                int secPDG = sec->GetDefinition()->GetPDGEncoding();
                int absSecPDG = std::abs(secPDG);
                G4double secE = sec->GetKineticEnergy() + sec->GetDefinition()->GetPDGMass();

                // Look for correct lepton for a CC interaction
                if (secPDG == expectedLepton) {
                    foundCCLepton = true;
                    fEventAction->outgoingLeptonPDG = secPDG;
                    fEventAction->outgoingLeptonE = secE;
                    fEventAction->outgoingLeptonPx = sec->GetMomentum().x();
                    fEventAction->outgoingLeptonPy = sec->GetMomentum().y();
                    fEventAction->outgoingLeptonPz = sec->GetMomentum().z();
                }

                // Looks for an outgoing neutrino for a NC interaction
                if (secPDG == pdg) {
                    foundOutNu = true;
                }

                // Topology Counters for Interaction Model Classification
                if (absSecPDG == 211 || absSecPDG == 111) { // pi+, pi-, pi0
                    nPions++;
                }
                if (absSecPDG == 2212 || absSecPDG == 2112) { // proton, neutron
                    nNucleons++;
                }

                // Hadronic energy accumulation
                if (absSecPDG == 2212 || absSecPDG == 2112 || absSecPDG == 211 ||
                    absSecPDG == 321  || absSecPDG == 111  || absSecPDG == 311 ||
                    absSecPDG == 310  || absSecPDG == 130) {
                    foundHadron = true;
                    fEventAction->outgoingHadronE += secE;
                }
            }
        }

        // Classify the interaction type & model topology
        if (foundCCLepton) {
            fEventAction->isCC = 1;
            fEventAction->isNC = 0;
            fEventAction->interactionType = "CC";
            // Assign CC Interaction Model Topology
            if (nPions == 0 && nNucleons > 0) {
                fEventAction->interactionModel = "CCQE";
            } else if (nPions == 1) {
                fEventAction->interactionModel = "CCRES";
            } else {
                fEventAction->interactionModel = "CCDIS";
            }
            // ========== NEUTRINO KINEMATICS (CC events) ==========
            // Incoming Neutrino kinematics MUST come from the PreStepPoint.
            // The track itself is killed at the PostStepPoint, meaning its current energy is 0.
            G4double Enu = step->GetPreStepPoint()->GetTotalEnergy();
            G4ThreeVector pNu = step->GetPreStepPoint()->GetMomentum();
            G4double pNuMag = pNu.mag();
            
            // Outgoing Lepton kinematics from the saved variables
            G4double Elep = fEventAction->outgoingLeptonE;
            G4ThreeVector pLep(fEventAction->outgoingLeptonPx, 
                               fEventAction->outgoingLeptonPy, 
                               fEventAction->outgoingLeptonPz);
            G4double pLepMag = pLep.mag();
            
            // Energy and momentum transfer
            G4double qEnergy = Enu - Elep;
            G4ThreeVector qVec = pNu - pLep;
            
            fEventAction->q0 = qEnergy;
            // Q2 = |qVec|^2 - q0^2
            fEventAction->Q2 = qVec.mag2() - (qEnergy * qEnergy);
            
            // Invariant Mass (W)
            const G4double nucleonMass = 939.565 * CLHEP::MeV;
            G4double W2 = (nucleonMass * nucleonMass) + (2.0 * nucleonMass * qEnergy) - fEventAction->Q2;
            fEventAction->W = (W2 > 0.0) ? std::sqrt(W2) : 0.0;
            
            // Bjorken-x & Inelasticity
            fEventAction->xBj = (2.0 * nucleonMass * qEnergy > 0.0) 
                                ? fEventAction->Q2 / (2.0 * nucleonMass * qEnergy) 
                                : 0.0;
                                
            // Inelasticity (y)
            fEventAction->yBj = (Enu > 0.0) ? qEnergy / Enu : 0.0;
            fEventAction->inelasticity = (Enu > 0.0) ? (1.0 - Elep / Enu) : 0.0;

            // ========== LEPTON SCATTERING ANGLE ==========
            if (pNuMag > 0.0 && pLepMag > 0.0) {
                G4double cosThetaLep = pNu.dot(pLep) / (pNuMag * pLepMag);
                // Clamp within [-1, 1] to protect acos against precision floating point overflow
                cosThetaLep = std::max(-1.0, std::min(1.0, cosThetaLep));
                fEventAction->leptonCosTheta = cosThetaLep;
                fEventAction->leptonScatteringAngle = std::acos(cosThetaLep);
            } else {
                fEventAction->leptonCosTheta = 1.0;
                fEventAction->leptonScatteringAngle = 0.0;
            }

            // ========== SHOWER SECONDARIES MULTIPLICITY ==========
            if (secondaries) {
                fEventAction->showerNSecondaries = static_cast<G4int>(secondaries->size());
            }
        } 
        else if (foundOutNu) {
            fEventAction->isCC = 0;
            fEventAction->isNC = 1;
            fEventAction->interactionType = "NC";

            // Neutral current kinematics are generally not fully reconstructible 
            // in the same way without knowing the outgoing neutrino energy.
            // Leaving kinematics as 0.0 default.

            // Assign NC Interaction Model Topology
            if (nPions == 0 && nNucleons > 0) {
                fEventAction->interactionModel = "NCQE";
            } else if (nPions == 1) {
                fEventAction->interactionModel = "NCRES";
            } else {
                fEventAction->interactionModel = "NCDIS";
            }
        } 
        else {
            fEventAction->isCC = 0;
            fEventAction->isNC = 0;
            fEventAction->interactionType = "Unknown";
            fEventAction->interactionModel = "Unknown";
            
            // Debug block retained only for true anomaly cases (neither lepton nor neutrino found)
            G4cout << "\n[DEBUG] --- UNKNOWN INTERACTION DETECTED ---" << G4endl;
            G4cout << "[DEBUG] Expected Lepton PDG: " << expectedLepton << G4endl;
            if (secondaries && secondaries->size() > 0) {
                G4cout << "[DEBUG] Secondaries produced directly from primary:" << G4endl;
                for (auto sec : *secondaries) {
                    if (sec->GetParentID() == track->GetTrackID()) {
                        G4cout << "        -> PDG: " << sec->GetDefinition()->GetPDGEncoding() 
                               << " (" << sec->GetDefinition()->GetParticleName() << ") "
                               << "| KE: " << sec->GetKineticEnergy() / CLHEP::MeV << " MeV" << G4endl;
                    }
                }
            } else {
                G4cout << "[DEBUG] No secondaries produced in this step." << G4endl;
            }
            G4cout << "[DEBUG] ----------------------------------------" << G4endl;
        }

        // Lock this so we don't overwrite it on the next step
        fEventAction->decisionMade = true;
        
        G4cout << "\n*** NEUTRINO INTERACTION RECORDED ***" << G4endl;
        G4cout << "Process: " << nuProcName << " | Type: " << fEventAction->interactionType 
        << " | Model: " << fEventAction->interactionModel << G4endl;
    }
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
        if(Esec > 1*keV) {
            fEventAction->AddSecE(Esec);
            fRunAction->secEnergies.push_back(Esec);
            fRunAction->secWeights.push_back(track->GetWeight());
        }

        // Particle type
        auto name = track->GetDefinition()->GetParticleName();

        int pdg = track->GetDefinition()->GetPDGEncoding();
        switch(pdg) {
            case 22:    fEventAction->AddGamma();       break;  // gamma
            case 11:    fEventAction->AddElectron();    break;  // e-
            case -11:   fEventAction->AddPositron();    break;  // e+
            case 2212:  fEventAction->AddProtonSec();   break;  // proton
            case 2112:  fEventAction->AddNeutron();     break;  // neutron
            case 211:   fEventAction->AddPionPlus();    break;  // pi+
            case -211:  fEventAction->AddPionMinus();   break;  // pi-
            case 111:   fEventAction->AddPionZero();    break;  // pi0
            case 13:    fEventAction->AddMuonMinus();   break;  // mu-
            case -15:   fEventAction->AddTauPlus();     break;  // tau+
            case 15:    fEventAction->AddTauMinus();    break;  // tau-
            case 321:   fEventAction->AddKaonPlus();    break;  // kaon+
            case -321:  fEventAction->AddKaonMinus();   break;  // kaon-
            case 311:   fEventAction->AddKaonZero();    break;  // kaon0
            case 310:   fEventAction->AddKaonZeroL();   break;  // kaon0 Long
            case 130:   fEventAction->AddKaonZeroS();   break;  // kaon0 short
            case -13:   fEventAction->AddMuonPlus();    break;  // mu+
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
                fEventAction->vertexX = step->GetPostStepPoint()->GetPosition().x();
                fEventAction->vertexY = step->GetPostStepPoint()->GetPosition().y();
                fEventAction->vertexZ = step->GetPostStepPoint()->GetPosition().z();
                fEventAction->vertexT = step->GetPostStepPoint()->GetGlobalTime();

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
        if(track->GetCurrentStepNumber() == 1) {
            // Initialize primary PDG tracking variables
            fEventAction->primaryPDG = track->GetDefinition()->GetPDGEncoding();
            fEventAction->primaryFinalPDG = track->GetDefinition()->GetPDGEncoding();

            fEventAction->x = track->GetPosition().x();
            fEventAction->y = track->GetPosition().y();
            fEventAction->z = track->GetPosition().z();
            
            if(fEventAction->ReadE() == 0) {
                fEventAction->SetInitialKinematics(
                    track->GetKineticEnergy(),
                    track->GetPosition(),
                    track->GetMomentum()
            );
            }
        }

    // Update final info only at track end
        if(track->GetTrackStatus() == fStopAndKill) {
            // If the particle terminates without oscillating, record its final definition
            int finalPDG = track->GetDefinition()->GetPDGEncoding();
            if (std::abs(finalPDG) == 12 || std::abs(finalPDG) == 14 || std::abs(finalPDG) == 16) {
                fEventAction->primaryFinalPDG = finalPDG;
            }

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
    int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    // Get Pre and Post step positions
    G4ThreeVector prePos = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPos = step->GetPostStepPoint()->GetPosition();
    
    // Get Process names safely
    const G4VProcess* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    const G4VProcess* creator = track->GetCreatorProcess();
    
    G4String procName = proc ? proc->GetProcessName() : "None";
    G4String creatorName = creator ? creator->GetProcessName() : "Primary";

    // Fill IDs
    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, track->GetTrackID());
    analysisManager->FillNtupleIColumn(1, 2, track->GetParentID());
    analysisManager->FillNtupleIColumn(1, 3, track->GetDefinition()->GetPDGEncoding());
    
    // Fill Pre-Step Coordinates
    analysisManager->FillNtupleDColumn(1, 4, prePos.x() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 5, prePos.y() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 6, prePos.z() / CLHEP::mm);
    
    // Fill Post-Step Coordinates
    analysisManager->FillNtupleDColumn(1, 7, postPos.x() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 8, postPos.y() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 9, postPos.z() / CLHEP::mm);
    
    // Fill Energy values
    analysisManager->FillNtupleDColumn(1, 10, step->GetPreStepPoint()->GetKineticEnergy() / CLHEP::MeV);
    analysisManager->FillNtupleDColumn(1, 11, step->GetTotalEnergyDeposit() / CLHEP::MeV);
    
    // Fill String Processes
    analysisManager->FillNtupleSColumn(1, 12, procName);
    analysisManager->FillNtupleSColumn(1, 13, creatorName);

    // Read and write statistical weights
    analysisManager->FillNtupleDColumn(1, 14, track->GetWeight());
    
    analysisManager->AddNtupleRow(1); // Adds row specifically to Ntuple 1 ("tracks")
}