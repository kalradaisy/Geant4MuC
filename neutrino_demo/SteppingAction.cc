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
#include <algorithm>

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
: fEventAction(eventAction), fRunAction(runAction) {}


void SteppingAction::UserSteppingAction(const G4Step* step) 
{
    if(!fRunAction || !fEventAction) return;
    // if there is no access to RunAction and EventAction, just stop

    // assigning info that will be helpful later
    auto track = step->GetTrack(); // gets our track
    auto material = step->GetPreStepPoint()->GetMaterial(); // gets our material
    auto element = material->GetElement(0); // and the first element

    // Accumulate total energy deposit and step count for all steps
    fEventAction->AddEdep(step->GetTotalEnergyDeposit());
    fEventAction->IncrementStep(); // move onto the next step

    // Step & Process Name prep
    const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
    std::string procName = process ? std::string(process->GetProcessName()) : "None";
    // gets process name information (if it exists)

    // Clean up process name by stripping out "biasWrapper()", just for clarity
    std::string biasPrefix = "biasWrapper(";
    if (procName.find(biasPrefix) == 0 && procName.back() == ')') {
        procName = procName.substr(biasPrefix.length(), procName.length() - biasPrefix.length() - 1);
    }
    /* this is just because modern biasing puts an ugly wrapper around the name
    of the process you bias. As long as you know what you're biasing, which you
    should, then this wrapper name can be stripped off for a more pleasing name*/

    if (procName != "Transportation" && procName != "None") { // if smth happens
        auto analysisManager = G4AnalysisManager::Instance();
        int currentEventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        // instantiate

        // information about the process for every single step, saved in ntuple 3
        // this is the analog of allInteractionProcess in the v11.4.1 code
        analysisManager->FillNtupleIColumn(3, 0, currentEventID);
        analysisManager->FillNtupleSColumn(3, 1, procName);
        analysisManager->AddNtupleRow(3);
    }

    // looking at PDG info for our particle, seeing if it's a neutrino
    int pdg = track->GetDefinition()->GetPDGEncoding();
    bool isPrimary = (track->GetParentID() == 0);
    bool isPrimaryNeutrino = isPrimary && 
                             (std::abs(pdg) == 12 || std::abs(pdg) == 14 || std::abs(pdg) == 16);

    bool isOscillationProcess = (procName.find("Oscillation") != std::string::npos || 
                                 procName.find("oscillation") != std::string::npos);

    // Oscillation (still broken right now, need to look at vacosc)
    if (isPrimaryNeutrino && isOscillationProcess) {
        fEventAction->nOscillationSteps++;

        if (!fEventAction->primaryOscillationProcessInvoked) {
            fEventAction->primaryOscillationProcessInvoked = 1;
            fEventAction->primaryOscillationPDGBefore = pdg;

            // Search secondaries for the new oscillated neutrino flavor
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
            
            // Update final PDG to the newly oscillated flavor
            fEventAction->primaryFinalPDG = postOscPDG;
        }
    }

    // Primary interaction and vertex recording
    // Physical interaction, i.e., hitting something, not transportation or oscillation
    bool isPhysicalInteraction = (process && procName != "Transportation" && procName != "None" && !isOscillationProcess);

    if (!fEventAction->decisionMade && isPrimary && isPhysicalInteraction) {
        
        // primary interaction vertex and time recording
        fEventAction->vertexX = step->GetPostStepPoint()->GetPosition().x();
        fEventAction->vertexY = step->GetPostStepPoint()->GetPosition().y();
        fEventAction->vertexZ = step->GetPostStepPoint()->GetPosition().z();
        fEventAction->vertexT = step->GetPostStepPoint()->GetGlobalTime();
        fEventAction->eventWeight = track->GetWeight();

        /* Lock decision immediately so subsequent steps/secondaries cannot
        overwrite. This is because we are saving information about the primary
        interaction vertex, and we do not want to either overwrite this info
        with later information or save the same information multiple times */
        fEventAction->decisionMade = true;

        // neutrino-specific sub-branch
        if (isPrimaryNeutrino) {
            fEventAction->nuInteractionProcess = procName; // what process?

            // Calculate expected charged lepton PDG based on incoming neutrino flavor
            int expectedLepton = 0;
            if (std::abs(pdg) == 12) expectedLepton = (pdg > 0) ? 11 : -11; // nu_e -> e-
            if (std::abs(pdg) == 14) expectedLepton = (pdg > 0) ? 13 : -13; // nu_mu -> mu-
            if (std::abs(pdg) == 16) expectedLepton = (pdg > 0) ? 15 : -15; // nu_tau -> tau-

            // instantiating some flags to default values so they may be set later
            bool foundCCLepton = false;
            bool foundOutNu = false;
            bool foundHadron = false;

            int nPions = 0;
            int nNucleons = 0;

            // Analyze particles produced in this interaction
            // Those secondaries inform CC/NC classification as well
            const auto* secondaries = step->GetSecondaryInCurrentStep();
            if (secondaries) {
                for (auto sec : *secondaries) { // check through all secondaries
                    
                    // Ensure secondary was born from this specific primary track
                    if (sec->GetParentID() != track->GetTrackID()) continue;

                    // record secondary information
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

                    // Look for outgoing neutrino for a NC interaction
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

            // classifying interaction model and type
            if (foundCCLepton) {
                fEventAction->isCC = 1;
                fEventAction->isNC = 0;
                fEventAction->interactionType = "CC"; // Charged Current
                if (nPions == 0 && nNucleons > 0) { // CC Quasi-Elastic Scattering
                    fEventAction->interactionModel = "CCQE"; 
                } else if (nPions == 1) { // CC Resonance
                    fEventAction->interactionModel = "CCRES";
                } else { // CC Deep Inelastic Scattering
                    fEventAction->interactionModel = "CCDIS";
                }

                // CC neutrino kinematics

                // Neutrino energy, momentum, and momentum magnitude
                G4double Enu = step->GetPreStepPoint()->GetTotalEnergy();
                G4ThreeVector pNu = step->GetPreStepPoint()->GetMomentum();
                G4double pNuMag = pNu.mag();
                
                // Lepton energy, momentum, and momentum magnitude
                G4double Elep = fEventAction->outgoingLeptonE;
                G4ThreeVector pLep(fEventAction->outgoingLeptonPx, 
                                   fEventAction->outgoingLeptonPy, 
                                   fEventAction->outgoingLeptonPz);
                G4double pLepMag = pLep.mag();
                
                // q_0 calculation
                G4double qEnergy = Enu - Elep;
                G4ThreeVector qVec = pNu - pLep;
                
                // Q^2 calculation
                fEventAction->q0 = qEnergy;
                G4double rawQ2 = qVec.mag2() - (qEnergy * qEnergy);
                fEventAction->Q2 = std::max(0.0, rawQ2);
                
                // W calculation
                const G4double nucleonMass = 939.565 * CLHEP::MeV;
                G4double W2 = (nucleonMass * nucleonMass) + (2.0 * nucleonMass * qEnergy) - fEventAction->Q2;
                fEventAction->W = (W2 > 0.0) ? std::sqrt(W2) : 0.0;
                
                // xBj, yBj, and inelasticity calculations
                fEventAction->xBj = (2.0 * nucleonMass * qEnergy > 0.0) 
                                    ? fEventAction->Q2 / (2.0 * nucleonMass * qEnergy) 
                                    : 0.0;
                                    
                fEventAction->yBj = (Enu > 0.0) ? qEnergy / Enu : 0.0;
                fEventAction->inelasticity = (Enu > 0.0) ? (1.0 - Elep / Enu) : 0.0;

                // Lepton scattering angle
                if (pNuMag > 0.0 && pLepMag > 0.0) {
                    G4double cosThetaLep = pNu.dot(pLep) / (pNuMag * pLepMag);
                    cosThetaLep = std::max(-1.0, std::min(1.0, cosThetaLep));
                    fEventAction->leptonCosTheta = cosThetaLep;
                    fEventAction->leptonScatteringAngle = std::acos(cosThetaLep);
                } else {
                    fEventAction->leptonCosTheta = 1.0;
                    fEventAction->leptonScatteringAngle = 0.0;
                }

                /* Recording the number of secondaries from the interaction.
                NB: This was not implemented in v11.4.1, so it may need to be
                changed if its original intention was to save other information */
                if (secondaries) {
                    fEventAction->showerNSecondaries = static_cast<G4int>(secondaries->size());
                }
            } 
            else if (foundOutNu) { // If it's not CC, it's Neutral Current
                fEventAction->isCC = 0;
                fEventAction->isNC = 1;
                fEventAction->interactionType = "NC";
                // Neutral current kinematics are generally not fully reconstructible 
                // in the same way without knowing the outgoing neutrino energy.
                // Leaving kinematics as 0.0 default.

                if (nPions == 0 && nNucleons > 0) {
                    fEventAction->interactionModel = "NCQE"; // NC Quasi Elastic
                } else if (nPions == 1) {
                    fEventAction->interactionModel = "NCRES"; // Resonance
                } else {
                    fEventAction->interactionModel = "NCDIS";
                } // Neutral Current Deep Inelastic Scattering
            } 
            else { // If it's not CC or NC, then something really weird happened
                fEventAction->isCC = 0;
                fEventAction->isNC = 0;
                fEventAction->interactionType = "Unknown";
                fEventAction->interactionModel = "Unknown";
                // This could be removed if confident Unknown interactions are
                // unlikely to take place, but it is left in for posterity's sake
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
            // Just a helpful print for neutrino biasing testing
            // Can be commented out if running a macroscopic number of events
            // Maybe it's worth gating stuff like this behind a verbosity messenger
            G4cout << "\n*** NEUTRINO INTERACTION RECORDED ***" << G4endl;
            G4cout << "Process: " << procName << " | Type: " << fEventAction->interactionType 
                   << " | Model: " << fEventAction->interactionModel << G4endl;
        } 
        else {
            // Below is if you're firing something other than neutrinos
            // You can comment this out if it's annoying
            G4cout << "\n*** PRIMARY PHYSICAL INTERACTION RECORDED (" 
                   << track->GetDefinition()->GetParticleName() << ") ***" << G4endl;
            G4cout << "Process: " << procName << " | Vertex: (" 
                   << fEventAction->vertexX / CLHEP::mm << ", " 
                   << fEventAction->vertexY / CLHEP::mm << ", " 
                   << fEventAction->vertexZ / CLHEP::mm << ") mm | Time: " 
                   << fEventAction->vertexT / CLHEP::ns << " ns" << G4endl;
        }
    }

    // validation and secondaries
    auto particle = track->GetDefinition();
    
    // Skip invalid nuclei
    if (particle->GetParticleType() == "nucleus") {
        auto ion = dynamic_cast<const G4Ions*>(particle);
        if (ion && (ion->GetAtomicNumber() <= 0 || ion->GetAtomicMass() <= 0)) {
            return;
        }
    }
    // If we are looking at something that isn't the parent track
    if(track->GetParentID() > 0 && track->GetCurrentStepNumber() == 1) {
        fEventAction->AddSecondary(); // Add a secondary to the count

        double Esec = track->GetKineticEnergy(); // Saving its kinetic energy
        if(Esec > 1*keV) { // as long as that KE is above 1 keV
            fEventAction->AddSecE(Esec);
            fRunAction->secEnergies.push_back(Esec);
            fRunAction->secWeights.push_back(track->GetWeight());
            // remember to save the weight too!
        }
        // if our secondary is at the end of its road
        if(track->GetParentID() > 0 && track->GetTrackStatus() == fStopAndKill) {
            fEventAction->AddSecEndPos(track->GetPosition());
            // record where that ending position was
        }

        int pdgSec = track->GetDefinition()->GetPDGEncoding();
        switch(pdgSec) { // just incrementing some counters based on PDG here
            case 22:    fEventAction->AddGamma();       break;
            case 11:    fEventAction->AddElectron();    break;
            case -11:   fEventAction->AddPositron();    break;
            case 2212:  fEventAction->AddProtonSec();   break;
            case 2112:  fEventAction->AddNeutron();     break;
            case 211:   fEventAction->AddPionPlus();    break;
            case -211:  fEventAction->AddPionMinus();   break;
            case 111:   fEventAction->AddPionZero();    break;
            case 13:    fEventAction->AddMuonMinus();   break;
            case -15:   fEventAction->AddTauPlus();     break;
            case 15:    fEventAction->AddTauMinus();    break;
            case 321:   fEventAction->AddKaonPlus();    break;
            case -321:  fEventAction->AddKaonMinus();   break;
            case 311:   fEventAction->AddKaonZero();    break;
            case 310:   fEventAction->AddKaonZeroL();   break;
            case 130:   fEventAction->AddKaonZeroS();   break;
            case -13:   fEventAction->AddMuonPlus();    break;
        }
        // Some functions for position and back scatter info
        fEventAction->AddSecStartPos(track->GetPosition());
        fEventAction->CountBackTracks(track->GetMomentumDirection());

        auto proc = track->GetCreatorProcess();
        if(proc) { // recording process name info here for our tree
            // if I were a better programmer, I'd probably make these enums
            // so I could use a switch statement.
            auto pname = proc->GetProcessName();
            if(pname == "compt")    fEventAction->AddCompton();
            if(pname == "conv")     fEventAction->AddPairProd();
            if(pname == "eIoni")    fEventAction->AddIonisation();
            if(pname == "eBrem")    fEventAction->AddBrem(); 
            if(pname == "phot")     fEventAction->AddPhotoEl();  
            if(pname == "annihil")  fEventAction->AddAnnihilation();  
            if(pname == "Decay")    fEventAction->AddDecay();
        }
    }

    // target info
    auto proc1 = step->GetPostStepPoint()->GetProcessDefinedStep();
    if(proc1)
    {
        auto hadProc = dynamic_cast<const G4HadronicProcess*>(proc1);
        if(hadProc) // if hadronic process
        {
            const G4Nucleus* target = hadProc->GetTargetNucleus();
            G4String pname = proc1->GetProcessName();
            // getting process name and getting the target

            if (pname.find("Nu") != std::string::npos ||
                pname.find("nu") != std::string::npos) {
                fEventAction->AddNuInteraction();
            }
            // recording if there was a neutrino interaction or not

            if(target) // there should always be a target, but still checks
            {
                int Z = target->GetZ_asInt(); // atomic number
                int A = target->GetA_asInt(); // mass number
                int targetPDGCode = 1000000000 + 10000*Z + 10*A;
                // the long PDG code

                // saving our info
                fEventAction->SetTargetZ(Z);
                fEventAction->SetTargetA(A);
                fEventAction->SetTargetPDG(targetPDGCode);
            }
        }
    }

    // Recording step length info for all tracks
    fEventAction->AddSecTrackLength(step->GetStepLength());

    // primary track info (ntuple 1)
    if(track->GetParentID() == 0) {

        // Save initial info at first step of any primary particle
        if(track->GetCurrentStepNumber() == 1) {
            fEventAction->primaryPDG = track->GetDefinition()->GetPDGEncoding();
            fEventAction->primaryFinalPDG = track->GetDefinition()->GetPDGEncoding();
            // primary final PDG is set here just as a fallback, but it is
            // purposely overwritten in the following block with fStopAndKill

            fEventAction->x = track->GetPosition().x();
            fEventAction->y = track->GetPosition().y();
            fEventAction->z = track->GetPosition().z();
            // primary coordates

            fEventAction->costh = track->GetVertexMomentumDirection().z();
            // saving primary cos(theta)
            
            if(fEventAction->ReadE() == 0) { // if no energy
                // I am not sure why this block needs to be here, 
                // but I am nervous to remove it
                fEventAction->SetInitialKinematics(
                    track->GetKineticEnergy(),
                    track->GetPosition(),
                    track->GetMomentum()
                );
            }
        }

        // Update final info at primary track end
        if(track->GetTrackStatus() == fStopAndKill) {
            int finalPDG = track->GetDefinition()->GetPDGEncoding();
            // Saving the ending particle information
            if (std::abs(finalPDG) == 12 || std::abs(finalPDG) == 14 || std::abs(finalPDG) == 16) {
                fEventAction->primaryFinalPDG = finalPDG;
            }
            // Overwriting the previous primaryFinalPDG now that we're at the end

            // Just setting final kinematic information
            fEventAction->SetFinalKinematics(
                track->GetKineticEnergy(),
                track->GetPosition()
            );
            // and momentum
            fEventAction->SetFinalMomentum(track->GetMomentum());
        }
    }

    // "tracks" tree filling (ntuple 1), all do what they say on the box
    auto analysisManager = G4AnalysisManager::Instance();
    int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    G4ThreeVector prePos  = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector postPos = step->GetPostStepPoint()->GetPosition();

    G4ThreeVector preMom  = step->GetPreStepPoint()->GetMomentum();
    G4ThreeVector postMom = step->GetPostStepPoint()->GetMomentum();

    G4double stepLength = step->GetStepLength();
    G4double stepTime   = step->GetPreStepPoint()->GetGlobalTime();
    
    const G4VProcess* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    const G4VProcess* creator = track->GetCreatorProcess();
    
    G4String procNameStr = proc ? proc->GetProcessName() : "None";
    G4String creatorName = creator ? creator->GetProcessName() : "Primary";

    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleIColumn(1, 1, track->GetTrackID());
    analysisManager->FillNtupleIColumn(1, 2, track->GetParentID());
    analysisManager->FillNtupleIColumn(1, 3, track->GetDefinition()->GetPDGEncoding());
    
    analysisManager->FillNtupleDColumn(1, 4, prePos.x() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 5, prePos.y() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 6, prePos.z() / CLHEP::mm);
    
    analysisManager->FillNtupleDColumn(1, 7, postPos.x() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 8, postPos.y() / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 9, postPos.z() / CLHEP::mm);

    analysisManager->FillNtupleDColumn(1, 10, preMom.x() / CLHEP::MeV);
    analysisManager->FillNtupleDColumn(1, 11, preMom.y() / CLHEP::MeV);
    analysisManager->FillNtupleDColumn(1, 12, preMom.z() / CLHEP::MeV);

    analysisManager->FillNtupleDColumn(1, 13, postMom.x() / CLHEP::MeV);
    analysisManager->FillNtupleDColumn(1, 14, postMom.y() / CLHEP::MeV);
    analysisManager->FillNtupleDColumn(1, 15, postMom.z() / CLHEP::MeV);
    
    analysisManager->FillNtupleDColumn(1, 16, step->GetPreStepPoint()->GetKineticEnergy() / CLHEP::MeV);
    analysisManager->FillNtupleDColumn(1, 17, step->GetTotalEnergyDeposit() / CLHEP::MeV);

    analysisManager->FillNtupleDColumn(1, 18, stepLength / CLHEP::mm);
    analysisManager->FillNtupleDColumn(1, 19, stepTime / CLHEP::ns);
    
    analysisManager->FillNtupleSColumn(1, 20, procNameStr);
    analysisManager->FillNtupleSColumn(1, 21, creatorName);
    analysisManager->FillNtupleDColumn(1, 22, track->GetWeight());
    
    analysisManager->AddNtupleRow(1);
}