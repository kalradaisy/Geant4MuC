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

        fEventAction->px    = track->GetMomentum().x();
        fEventAction->py    = track->GetMomentum().y();
        fEventAction->pz    = track->GetMomentum().z();
        fEventAction->theta  = track->GetMomentumDirection().theta();
        fEventAction->phi    = track->GetMomentumDirection().phi();
        fEventAction->costh  = track->GetMomentumDirection().z();
      }
    }
     // Update final info only at track end                                                                                                                             \
     // when primary particle track ends as a result of some kind of interaction or if it exists World vlume 
          if(track->GetParentID() == 0 && track->GetTrackStatus() == fStopAndKill) {
	    G4cout << "Primary particle interacted or died " << G4endl;
	
            fEventAction->finalE  = track->GetKineticEnergy();
            auto p = track->GetMomentum();
            double pMag = p.mag();
            fEventAction->finalPx = p.x();
            fEventAction->finalPy = p.y();
            fEventAction->finalPz = p.z();
            fEventAction->finalCosth = p.z()/pMag;
	    
            fEventAction->finalPhi = std::atan2(p.y(), p.x());
            fEventAction->finalPhiDeg = fEventAction->finalPhi * 180.0 / CLHEP::pi;


            fEventAction->finalX  = track->GetPosition().x();
            fEventAction->finalY  = track->GetPosition().y();
            fEventAction->finalZ  = track->GetPosition().z();

          }



      //First particle interaction to save interaction vertices
      auto process = step->GetPostStepPoint()->GetProcessDefinedStep();
      int pdg = track->GetDefinition()->GetPDGEncoding();
      G4String procName = process->GetProcessName();
      bool isPrimaryNeutrino =
	track->GetParentID() == 0 &&
	(std::abs(pdg) == 12 || std::abs(pdg) == 14 || std::abs(pdg) == 16);
      
      if(track->GetParentID() == 0 && process && process->GetProcessName()!= "Transportation"  && !fEventAction->interactionRecorded)
	{
	  fEventAction->vertexX = step->GetPostStepPoint()->GetPosition().x();
          fEventAction->vertexY = step->GetPostStepPoint()->GetPosition().y();
          fEventAction->vertexZ = step->GetPostStepPoint()->GetPosition().z();
          fEventAction->vertexT = step->GetPostStepPoint()->GetGlobalTime();

	  if(isPrimaryNeutrino)	{
            fEventAction->nuInteractionProcess = procName;
	  }
	  fEventAction->interactionRecorded = true;

	}


      	// Update final info only at track end                                                                                                                                         
          if(track->GetTrackStatus() == fStopAndKill) {

            fEventAction->finalE  = track->GetKineticEnergy();
            auto p = track->GetMomentum();
            double pMag = p.mag();
            fEventAction->finalPx = p.x();
            fEventAction->finalPy = p.y();
            fEventAction->finalPz = p.z();
            fEventAction->finalCosth = p.z()/pMag; // correct cos(theta)                                                                                                               
            fEventAction->finalPhi = std::atan2(p.y(), p.x());
            fEventAction->finalPhiDeg = fEventAction->finalPhi * 180.0 / CLHEP::pi;


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
                                    creatorProcess ? creatorProcess->GetProcessName() : "Primary",

                                    track->GetVertexPosition(),
                                    track->GetVertexKineticEnergy()
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
              G4cout << "Secondary particle production " <<  fEventAction->nSecondaries << " secondaries with creator process "<< creatorName <<G4endl;

              fEventAction->nSecondaries++;

              double Esec = track->GetKineticEnergy();
              if(Esec > 1*keV) {
                fEventAction->totalSecondaryE += Esec;
              }//fRunAction->secTotalE += Esec;                                                                                                                       

              fEventAction->secEnergies.push_back(Esec);

              int pdg = track->GetDefinition()->GetPDGEncoding();

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


	    // Track length accumulation (all tracks); can be used for dE/dx calculation                                                                                         
          if(track->GetParentID() > 0)
            {
              fRunAction->secTrackLength += step->GetStepLength();
            }

	  // ***************************************** Neutrino block **********************************************
	  
	  
	  //Neutrino interaction debugging : if only one interaction or multiple interactions
	  if(isPrimaryNeutrino &&  process && process->GetProcessName()!= "Transportation")
	    {
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
	      
	       G4cout << "Time (ns): "
	           << fEventAction->vertexT/ns << G4endl;
	    }


	  
	  G4double Ein = step->GetPreStepPoint()->GetKineticEnergy()
	    + track->GetDefinition()->GetPDGMass();
	  
	  G4ThreeVector pin = step->GetPreStepPoint()->GetMomentum();
	  
	  auto secondaries = step->GetSecondaryInCurrentStep();

	  if ( process->GetProcessName()!= "Transportation")
	    {
	      fEventAction->allInteractionProcess.push_back(process->GetProcessName());

	      //	      if only one interaction or multiple interactions , check Process
	      /*      G4cout << "\n=== Primary particle interaction information ===" << G4endl;
	      G4cout << "Incoming PDG = " << pdg << G4endl;
	      G4cout << "Process = " << procName << G4endl;
	      G4cout << "Track status = " << track->GetTrackStatus() << G4endl;
	      G4cout << "Pre KE  = " << step->GetPreStepPoint()->GetKineticEnergy()/MeV << " MeV" << G4endl;
	      G4cout << "Post KE = " << step->GetPostStepPoint()->GetKineticEnergy()/MeV << " MeV" << G4endl;
	      G4cout << "Number of secondaries = " << secondaries->size() << G4endl;
	      */
	    } 
	  //Classify Neutrino CC and NC primary interaction
	  bool hasCCLepton = false;
	  bool hasOutNeutrino = false;
	  bool hasHadron = false;
	  bool decisionMade = false;
	  int expectedLepton = 0;
	  
	  if (std::abs(pdg) == 12) expectedLepton = (pdg > 0) ? -11 : 11;
	  if (std::abs(pdg) == 14) expectedLepton = (pdg > 0) ? -13 : 13;
	  if (std::abs(pdg) == 16) expectedLepton = (pdg > 0) ? -15 : 15;
	  G4double Eout = -1.0;    // leptonic system
	  G4double Ehadr = 0.0;     // hadronic system
	  G4ThreeVector pout;
	  int outgoingPDG = 0;

	  std::vector<int> secPDGs;
	  int primaryTrackID = track->GetTrackID();
  
	  for (auto sec : *secondaries)
	    {
	      
	      if (sec->GetParentID() != primaryTrackID)
		continue;
	      
	      int secPDG = sec->GetDefinition()->GetPDGEncoding();
	      secPDGs.push_back(secPDG);
	      auto def = sec->GetDefinition();
	      if (!decisionMade)
		{
		  
		  if(secPDG == expectedLepton)
		    {
		      hasCCLepton = true;
		      G4cout << "First relevant secondary: "
			     << secPDG << " "
			     << sec->GetDefinition()->GetParticleName()
			     << G4endl;
		      decisionMade == true;
		    }
		  else if (secPDG == pdg)
		    {
		  hasOutNeutrino = true;
		  G4cout << "First relevant secondary: "
                             << secPDG << " "
                             << sec->GetDefinition()->GetParticleName()
                             << G4endl;
                      decisionMade == true;
		      
		    }
		  	}

	      
	      // if (firstSec){
	      /*  G4cout << "  secondary: "
		     << " PDG=" << def->GetPDGEncoding()
		     << " name=" << def->GetParticleName()
		     << " KE=" << sec->GetKineticEnergy()/MeV << " MeV"
		     << " E=" << (sec->GetKineticEnergy() + def->GetPDGMass())/MeV << " MeV"
		     << " parentID=" << sec->GetParentID()
		     << G4endl;
	      */
	      //}
	      
	      // CC lepton
	      if ( secPDG == expectedLepton)
		{
		  //  hasCCLepton = true;
		  // decisionMade = true;

		  outgoingPDG = secPDG;
		  Eout = sec->GetKineticEnergy()
		    + sec->GetDefinition()->GetPDGMass();
		  pout = sec->GetMomentum();
		  //  break;
		}
	      
	      // outgoing neutrino
	      // if ( secPDG == pdg) {
		//hasOutNeutrino = true;
		//decisionMade = true;
		//break;
	      //	}
	      
	      int apdg = std::abs(secPDG);
	      if (apdg == 2212 || apdg == 2112 ||
		  apdg == 211  || apdg == 321 ||
		  apdg == 111)
		{
	      hasHadron = true;
	      Ehadr += sec->GetKineticEnergy()
		+ sec->GetDefinition()->GetPDGMass();
		}
	    }
	  
	  
	  bool foundCC = isPrimaryNeutrino && hasCCLepton;
	  bool foundNC = isPrimaryNeutrino && (!hasCCLepton && hasOutNeutrino);
	  
	  // ********* Neutrino NC/CC Classification Done
	  
	  // some neutrino specific variables
	  
	  if(foundCC || foundNC) {
	    fEventAction->isCC = foundCC;
	    fEventAction->isNC = foundNC;
	    
	    fEventAction->outgoingLeptonPDG = outgoingPDG;
	    fEventAction->outgoingLeptonE = Eout;
            fEventAction->outgoingHadronE = Ehadr;
	    fEventAction->outgoingLeptonPx = pout.x();
	    fEventAction->outgoingLeptonPy = pout.y();
	    fEventAction->outgoingLeptonPz = pout.z();
	    fEventAction->finalStatePDG = secPDGs;

	    fEventAction->q0 = Ein - Eout;
	    
	    G4ThreeVector qvec = pin - pout;
	    fEventAction->Q2 = qvec.mag2() - fEventAction->q0 * fEventAction->q0;
	    
	    if(Ein > 0.0) {
	      fEventAction->yBj = fEventAction->q0 / Ein;
	    }
	    
	    G4double M = 939.565; // MeV
	    if(fEventAction->q0 > 0.0) {
	      fEventAction->xBj = fEventAction->Q2 / (2.0 * M * fEventAction->q0);
	      
	      G4double W2 = M*M + 2.0*M*fEventAction->q0 - fEventAction->Q2;
	      fEventAction->W = (W2 > 0.0) ? std::sqrt(W2) : -999.0;
	    }
	  }	  

                                                                              


	  

}
