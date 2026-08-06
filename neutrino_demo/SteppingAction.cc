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
#include "G4IonTable.hh"       // sometimes needed
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
: fEventAction(eventAction), fRunAction(runAction) {}


void SteppingAction::UserSteppingAction(const G4Step* step) 
{
    if(!fRunAction || !fEventAction) return;

    auto track = step->GetTrack();

    fEventAction->AddEdep(step->GetTotalEnergyDeposit());
    fEventAction->IncrementStep();

    // -----------------------------                                                                                                                                                                                                
    // Primary particle information (ParentID is 0 for primary)
    
    // -----------------------------                                                                                                                                                                                                 

    if(track->GetParentID() == 0) {                                   
      if(track->GetCurrentStepNumber() == 1){ // when track just started // to verify initial kinematics as we set in Primary generator
	G4cout << "Primary particle started " << G4endl;
	
        auto particle = track->GetDefinition();
        fEventAction->primaryTrackID = track->GetTrackID();
        fEventAction->PDG = particle->GetPDGEncoding();
        fEventAction->E     = track->GetKineticEnergy();
        fEventAction->x  = track->GetPosition().x();
        fEventAction->y  = track->GetPosition().y();
        fEventAction->z  = track->GetPosition().z();

	auto dir = track->GetVertexMomentumDirection();

	fEventAction->phi   = std::atan2(dir.y(),dir.x());
	fEventAction->costh = dir.z();
	
        fEventAction->px    = track->GetMomentum().x();
	fEventAction->py    = track->GetMomentum().y();
        fEventAction->pz    = track->GetMomentum().z();
      }
    }
      //First particle interaction to save interaction vertices
      auto process = step->GetPostStepPoint()->GetProcessDefinedStep();
      int pdg = track->GetDefinition()->GetPDGEncoding();
      std::string procName = process ? std::string(process->GetProcessName()) : "None";
      
      bool isPrimaryNeutrino =
	track->GetParentID() == 0 &&
	(std::abs(pdg) == 12 || std::abs(pdg) == 14 || std::abs(pdg) == 16);
      bool isOscillationProcess = process && procName.find("Oscillation") != std::string::npos;
      
      if(isPrimaryNeutrino && isOscillationProcess) {
          fEventAction->nOscillationSteps++;
          if(!fEventAction->primaryOscillationProcessInvoked) {
              fEventAction->primaryOscillationProcessInvoked = true;
              fEventAction->primaryOscillationPDGBefore = fEventAction->PDG;
              fEventAction->primaryOscillationPDGAfter = pdg;
              fEventAction->primaryOscillationFlavorChanged = (pdg != fEventAction->PDG);
          }
      }

      
      if(track->GetParentID() == 0 && process && procName != "Transportation"  && !fEventAction->interactionRecorded)
	{
	  fEventAction->vertexX = step->GetPostStepPoint()->GetPosition().x();
          fEventAction->vertexY = step->GetPostStepPoint()->GetPosition().y();
          fEventAction->vertexZ = step->GetPostStepPoint()->GetPosition().z();
          fEventAction->vertexT = step->GetPostStepPoint()->GetGlobalTime();
	  fEventAction->eventWeight = track->GetWeight();
	  
	  if(isPrimaryNeutrino) {
	    
	    G4cout << "******Primary Neutrino: " << G4endl;
            fEventAction->nuInteractionProcess = procName;
            
	  }
	  
	  
	  fEventAction->interactionRecorded = true;
	}
      
	// Update final info only at track end                                                                                                                                         
          if(track->GetTrackStatus() == fStopAndKill) {
            if(track->GetParentID() == 0) {
              fEventAction->primaryFinalPDG = track->GetDefinition()->GetPDGEncoding();
            }
            fEventAction->finalE  = track->GetKineticEnergy();
            auto p = track->GetMomentum();
            double pMag = p.mag();
	    //            fEventAction->finalPx = p.x();
            //fEventAction->finalPy = p.y();
            //fEventAction->finalPz = p.z();
            //fEventAction->finalCosth = p.z()/pMag; // correct cos(theta)                                                                                                               
            //fEventAction->finalPhi = std::atan2(p.y(), p.x());
            //fEventAction->finalPhiDeg = fEventAction->finalPhi * 180.0 / CLHEP::pi;
            fEventAction->finalX  = track->GetPosition().x();
            fEventAction->finalY  = track->GetPosition().y();
            fEventAction->finalZ  = track->GetPosition().z();

          }

 // 2. Step-level info (all tracks)                                                                                                                  
          const G4VProcess* stepProcess =
            step->GetPostStepPoint()->GetProcessDefinedStep();

          const G4VProcess* creatorProcess =
            track->GetCreatorProcess();

          fEventAction->AddStepInfo(
                                    track->GetTrackID(),
                                    track->GetParentID(),
                                    track->GetDefinition()->GetPDGEncoding(),

                                    step->GetPreStepPoint()->GetPosition(),
                                    step->GetPostStepPoint()->GetPosition(),

                                    step->GetPreStepPoint()->GetMomentum(),
                                    step->GetPostStepPoint()->GetMomentum(),

                                    step->GetPreStepPoint()->GetKineticEnergy(),
                                    step->GetTotalEnergyDeposit(),

                                    step->GetPreStepPoint()->GetGlobalTime(),
                                    step->GetStepLength(),

                                    stepProcess ? stepProcess->GetProcessName() : "None",
                                    creatorProcess ? creatorProcess->GetProcessName() : "Primary"

                                    //track->GetVertexPosition(),
                                    //track->GetVertexKineticEnergy()
                                    );

          G4int trackID = track->GetTrackID();
          G4int parentID = track->GetParentID();

	  if(track->GetCurrentStepNumber() > 0) {
            auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();
            if(proc) {
              G4String pname = proc->GetProcessName();
              fEventAction->stepProcessNames.push_back(std::string(pname));
            }}


  // Handle secondaries                                                                                                                                         \
                                                                                                                                                                         
          if(track->GetParentID() > 0 && track->GetCurrentStepNumber() == 1 && track->GetCreatorProcess())
            {
              G4String creatorName =
                track->GetCreatorProcess()->GetProcessName();
              //G4cout << "Secondary particle production " <<  fEventAction->nSecondaries << " secondaries with creator process "<< creatorName <<G4endl;

              fEventAction->nSecondaries++;

              double Esec = track->GetKineticEnergy();
              if(Esec > 0.01*keV) {
                fEventAction->totalSecondaryE += Esec;
		fRunAction->secWeights.push_back(track->GetWeight());
		
              }//fRunAction->secTotalE += Esec;

	      
              fEventAction->secEnergies.push_back(Esec);

              int pdg = track->GetDefinition()->GetPDGEncoding();
              
              // ========== ELECTRON/PHOTON SHOWER ANALYSIS ==========
              // Count electromagnetic processes
              G4String procName(creatorName);
              if(procName.find("eBrem") != std::string::npos) {
                fRunAction->nBremsstrahlung++;
                fEventAction->nBremstrahlungProcesses++;
              }
              if(procName.find("eIoni") != std::string::npos) {
                fRunAction->nIonisation++;
                fEventAction->nIonizationProcesses++;
              }
              if(procName.find("compt") != std::string::npos || 
                 procName.find("Compton") != std::string::npos) {
                fRunAction->nCompton++;
                fEventAction->nComptonScatters++;
              }
              if(procName.find("conv") != std::string::npos ||
                 procName.find("PairProd") != std::string::npos) {
                fRunAction->nPairProd++;
                fEventAction->nPairProductions++;
              }
              if(procName.find("phot") != std::string::npos ||
                 procName.find("PhotoElec") != std::string::npos) {
                fRunAction->nPhotoElectric++;
                fEventAction->nPhotoElectricEvents++;
              }
              if(procName.find("annihil") != std::string::npos) {
                fRunAction->nAnnihilation++;
              }
              if(procName.find("Decay") != std::string::npos) {
                fRunAction->nDecay++;
              }

              // ========== PARTICLE PRODUCTION COUNTING ==========
              if(pdg == 22)    fRunAction->nGamma++;

              if(pdg == 11)    fRunAction->nElectron++;
              if(pdg == -11)   fRunAction->nPositron++;

              if(pdg == 2212)  fRunAction->nProtonSec++;
              if(pdg == 2112)  fRunAction->nNeutron++;

              if(pdg == 211)   fRunAction->nPionPlus++;
              if(pdg == -211)  fRunAction->nPionMinus++;
              if(pdg == 111)   fRunAction->nPionZero++;

              if(pdg == 13)    fRunAction->nMuonMinus++;
              if(pdg == -13)   fRunAction->nMuonPlus++;

              if(pdg == 15)    fRunAction->nTauMinus++;
              if(pdg == -15)   fRunAction->nTauPlus++;

	      if(pdg == 321)   fRunAction->nKaonPlus++;
              if(pdg == -321)  fRunAction->nKaonMinus++;

              if(pdg == 311)   fRunAction->nKaonZero++;
              if(pdg == 130)   fRunAction->nKaonZeroL++;
              if(pdg == 310)   fRunAction->nKaonZeroS++;


            // Save track info                                                                                                                                           


              fEventAction->secTrackID.push_back(track->GetTrackID());
              fEventAction->secParentID.push_back(track->GetParentID());
              fEventAction->secPDG.push_back(track->GetDefinition()->GetPDGEncoding());
	      auto p = track->GetMomentum();
	      fEventAction->secPx.push_back(p.x());
	      fEventAction->secPy.push_back(p.y());
	      fEventAction->secPz.push_back(p.z());
	      
              auto pos = track->GetPosition();
              fEventAction->secStartX.push_back(pos.x());
              fEventAction->secStartY.push_back(pos.y());
              fEventAction->secStartZ.push_back(pos.z());

              // Backward tracks                                                                                                                                         
              if(track->GetMomentumDirection().z() < 0)
                {  fRunAction->nBackward++;
                }

            }

          if(track->GetParentID() > 0 && track->GetTrackStatus() == fStopAndKill) {
            auto pos = track->GetPosition();

            fEventAction->secEndX.push_back(pos.x());
            fEventAction->secEndY.push_back(pos.y());
            fEventAction->secEndZ.push_back(pos.z());
          }



          auto proc1 = step->GetPostStepPoint()->GetProcessDefinedStep();

          if(proc1)
            {
	      
              auto hadProc = dynamic_cast<const G4HadronicProcess*>(proc1);

              if(hadProc)
                {
                  const G4Nucleus* target = hadProc->GetTargetNucleus();

                  if(target)
                    {
                      int Z = target->GetZ_asInt();
                      int A = target->GetA_asInt();

                      //            G4cout << "Struck nucleus: Z=" << Z << " A=" << A << G4endl;                                                                        
                      // Construct PDG code for the nucleus
		      
                      int pdg = 1000000000 + 10000*Z + 10*A;


                      // Save in RunAction if you want                                                                                                              
                      fRunAction->targetZ = Z;
                      fRunAction->targetA = A;
                      fRunAction->targetPDG = pdg;

                    }
                }
            }

	   fEventAction->AddSecTrackLength(step->GetStepLength());

	   
	  // ***************************************** Neutrino block **********************************************
	  

	  
	  //Neutrino interaction debugging : if only one interaction or multiple interactions
	  /*if(isPrimaryNeutrino &&  process && process->GetProcessName()!= "Transportation")
	    {

	      //   if(track->GetParentID() > 0) {
	      auto creator = track->GetCreatorProcess();
	      
	      if (process->GetProcessName() == "nuVacOscillation"){ // || creator->GetProcessName() == "nuVacOscillation")          {
		G4cout << "nuvacosc." << G4endl;
		track->SetTrackStatus(fStopAndKill);

		//if(track->GetKineticEnergy()==0) {
		G4EventManager::GetEventManager()->AbortCurrentEvent();
		//	}
		G4cout
		  << "Track "
		  << track->GetTrackID()
		  << " PDG "
		  << track->GetDefinition()->GetPDGEncoding()
		  << " KE "
		  << track->GetKineticEnergy()/eV
		  << " eV"
		  << G4endl;
		if (track->GetKineticEnergy() < 1.0 * eV)
		  {
		    G4cout << "\n*** TrackingAction: Fixing broken oscillation secondary ***"
			   << "\n    PDG = " << track->GetDefinition()->GetPDGEncoding()
			   << "\n    KE  = " << track->GetKineticEnergy() / GeV << " GeV (before fix)"
			   << G4endl;
		    track->SetTrackStatus(fStopAndKill);
		  }
		
		return;
	      }
	      G4cout << "\n=== Neutrino interaction ===\n";
	      G4cout << "Event ID: "
		     << G4RunManager::GetRunManager()
		->GetCurrentEvent()->GetEventID()
		     << G4endl;
	      
	      G4cout << "Neutrino PDG: " << pdg << G4endl;
	      G4cout << "Process: " << procName << G4endl;
	      
	      	      G4cout << "Vertex (mm): "
	           << fEventAction->vertexX << " "
	           << fEventAction->vertexY << " "
	           << fEventAction->vertexZ << G4endl;
	      
		      // G4cout << "Time (ns): "
		      //<< fEventAction->vertexT/ns << G4endl;
	    }
	  */


	  
	  if ( process->GetProcessName()!= "Transportation")
	    {
	      fEventAction->allInteractionProcess.push_back(process->GetProcessName());
	    } 


}
