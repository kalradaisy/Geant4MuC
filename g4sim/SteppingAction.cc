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

SteppingAction::SteppingAction(EventAction* eventAction, RunAction* runAction)
: fEventAction(eventAction), fRunAction(runAction) {}


void SteppingAction::UserSteppingAction(const G4Step* step) 
{
    if(!fRunAction || !fEventAction) return;

    auto track = step->GetTrack();

    auto material = step->GetPreStepPoint()->GetMaterial();
    auto element = material->GetElement(0); // first element

    

    //G4cout << "Material: " << material->GetName() << G4endl;
    //G4cout << "Element Z: " << element->GetZ() << G4endl;

 
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
        fRunAction->nSecondaries++;

        // initial kinetic energy of secondary
        double Esec = track->GetKineticEnergy();
        if(Esec > 1*keV) fRunAction->secTotalE += Esec;

        // Particle type
        auto name = track->GetDefinition()->GetParticleName();
	

	if(name == "gamma") fRunAction->nGamma++;
        if(name == "e-")    fRunAction->nElectron++;
        if(name == "e+")    fRunAction->nPositron++;
        if(name == "proton") fRunAction->nProtonSec++;
        if(name == "neutron") fRunAction->nNeutron++;
        if(name == "pi+")   fRunAction->nPionPlus++;
        if(name == "pi-")   fRunAction->nPionMinus++;
        if(name == "pi0")   fRunAction->nPionZero++;
	if(name == "mu-")   fRunAction->nMuonMinus++;
	if(name == "tau+")   fRunAction->nTauPlus++;
        if(name == "tau-")   fRunAction->nTauMinus++;	
	if(name == "kaon+")  fRunAction->nKaonPlus++;
	if(name == "kaon-")  fRunAction->nKaonMinus++;
	if(name == "kaon0")  fRunAction->nKaonZero++;
	if(name == "kaon0L") fRunAction->nKaonZeroL++;
	if(name == "kaon0S") fRunAction->nKaonZeroS++;
	if(name == "mu+"){
	  fRunAction->nMuonPlus++;
	 // Identify parent
        auto parentTrack = track->GetParentID(); // this is just the ID
        auto creator = track->GetCreatorProcess(); // process that created this secondary

        G4cout << "mu+ created!" << G4endl;
        G4cout << "Parent ID: " << parentTrack << G4endl;
        if(creator){
            G4cout << "Created by process: " << creator->GetProcessName() << G4endl;
        }
	else
            G4cout << "No creator process (primary muon?)" << G4endl;
    }
    
	// Z position
        double z = track->GetPosition().z();
        if(fRunAction->secFirstZ > z) fRunAction->secFirstZ = z;
        if(fRunAction->secLastZ  < z) fRunAction->secLastZ = z;

	double x = track->GetPosition().x();
	if(fRunAction->secFirstX > x) fRunAction->secFirstX = x;
        if(fRunAction->secLastX  < x) fRunAction->secLastX = x;

	
	double y = track->GetPosition().y();
        if(fRunAction->secFirstY > y) fRunAction->secFirstY = y;
        if(fRunAction->secLastY  < y) fRunAction->secLastY = y;


        // Backward tracks
        if(track->GetMomentumDirection().z() < 0)
            fRunAction->nBackward++;

        // Creator process
        auto proc = track->GetCreatorProcess();
        if(proc) {
            auto pname = proc->GetProcessName();
	    //	     G4cout << proc->GetProcessName() << G4endl;

            if(pname == "compt") fRunAction->nCompton++;
            if(pname == "conv")  fRunAction->nPairProd++;
            if(pname == "eIoni") fRunAction->nIonisation++;
            if(pname == "eBrem") fRunAction->nBremsstrahlung++; //RunAction->nIonisation++;
	    if (pname == "phot")     fRunAction->nPhotoElectric++;  
	    if (pname == "annihil")  fRunAction->nAnnihilation++;  
	    if(pname == "Decay") fRunAction->nDecay++;
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


/*
if(proc1)
{
    auto pname1 = proc1->GetProcessName();

    if(!fEventAction->neutrinoInteractionPrinted &&
       (pname1.contains("nu") || pname1.contains("Nu")))
    {
        auto pos1 = step->GetPostStepPoint()->GetPosition();

	//        G4cout << "\n==============================" << G4endl;
        //G4cout << "Neutrino interaction detected" << G4endl;
        //G4cout << "Process: " << pname1 << G4endl;
        //G4cout << "Vertex: "
	//      << pos1.x()/cm << " "
	//      << pos1.y()/cm << " "
	//      << pos1.z()/cm << " cm" << G4endl;
        //G4cout << "==============================" << G4endl;

        fEventAction->neutrinoInteractionPrinted = true;
    }
}
*/
    
    // Track length accumulation (all tracks)
    fRunAction->secTrackLength += step->GetStepLength();

    // -----------------------------
    // Primary particle information (ID = 0)
    // -----------------------------

    if(track->GetParentID() == 0) {

    // Save initial info at first step
    if(fRunAction->E == 0) {
        fRunAction->E     = track->GetKineticEnergy();
        fRunAction->px    = track->GetMomentum().x();
        fRunAction->py    = track->GetMomentum().y();
        fRunAction->pz    = track->GetMomentum().z();
        fRunAction->theta  = track->GetMomentumDirection().theta();
        fRunAction->phi    = track->GetMomentumDirection().phi();
        fRunAction->costh  = track->GetMomentumDirection().z();
    }

    // Update final info only at track end
    if(track->GetTrackStatus() == fStopAndKill) {
        fRunAction->finalE  = track->GetKineticEnergy();
        fRunAction->finalX  = track->GetPosition().x();
        fRunAction->finalY  = track->GetPosition().y();
        fRunAction->finalZ  = track->GetPosition().z();

        // Recompute final direction
        auto p = track->GetMomentum();
        double pMag = p.mag();
        fRunAction->finalPx = p.x();
        fRunAction->finalPy = p.y();
        fRunAction->finalPz = p.z();
        fRunAction->finalCosth = p.z()/pMag; // correct cos(theta)
	fRunAction->finalPhi = std::atan2(p.y(), p.x());
	fRunAction->finalPhiDeg = fRunAction->finalPhi * 180.0 / CLHEP::pi;
    }
    }
}

    /*   if(track->GetParentID() == 0) {
        // Save initial info (first step)
        if(fRunAction->E == 0c) {
            fRunAction->E     = track->GetKineticEnergy();
            fRunAction->x     = track->GetPosition().x();
            fRunAction->y     = track->GetPosition().y();
            fRunAction->z     = track->GetPosition().z();

            fRunAction->px    = track->GetMomentum().x();
            fRunAction->py    = track->GetMomentum().y();
            fRunAction->pz    = track->GetMomentum().z();

            fRunAction->theta  = track->GetMomentumDirection().theta();
            fRunAction->phi    = track->GetMomentumDirection().phi();
            fRunAction->costh  = track->GetMomentumDirection().z();
        }

        // Update final info every step
        fRunAction->finalE  = track->GetKineticEnergy();
        fRunAction->finalX  = track->GetPosition().x();
        fRunAction->finalY  = track->GetPosition().y();
        fRunAction->finalZ  = track->GetPosition().z();
    }
    }*/

/*


void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if(!fRunAction || !fEventAction) return;

    auto track = step->GetTrack();

    // ============================
    // Count ALL steps + edep (once)
    // ============================
    fEventAction->IncrementStep();
    fEventAction->AddEdep(step->GetTotalEnergyDeposit());

    // ============================
    // SECONDARIES
    // ============================
    if(track->GetParentID() > 0)
    {
        // Only once per secondary
        if(track->GetCurrentStepNumber() == 1)
        {
            fRunAction->nSecondaries++;

            // Energy at creation
            fRunAction->secTotalE += track->GetKineticEnergy();

            // Particle type
            auto name =
              track->GetDefinition()->GetParticleName();

            if(name == "gamma") fRunAction->nGamma++;
            if(name == "e-")    fRunAction->nElectron++;
            if(name == "e+")    fRunAction->nPositron++;
	    if(name=="proton")   fRunAction->nProtonSec++;
	    if(name=="neutron")  fRunAction->nNeutron++;
	    if(name=="pi+")      fRunAction->nPionPlus++;
	    if(name=="pi-")      fRunAction->nPionMinus++;
	    if(name=="pi0")      fRunAction->nPionZero++;
	    
            // Z limits
            double z = track->GetPosition().z();

            if(z < fRunAction->secFirstZ)
                fRunAction->secFirstZ = z;

            if(z > fRunAction->secLastZ)
                fRunAction->secLastZ = z;

            // Backward
            if(track->GetMomentumDirection().z() < 0)
                fRunAction->nBackward++;

            // Creator process
            auto proc = track->GetCreatorProcess();

            if(proc)
            {
                auto pname = proc->GetProcessName();

                if(pname == "compt")
                    fRunAction->nCompton++;

                if(pname == "conv")
                    fRunAction->nPairProd++;

                if(pname == "eIoni")
                    fRunAction->nIonisation++;
            }
        }

        // Track length (only secondaries)
        fRunAction->secTrackLength += step->GetStepLength();

        return; // Done with secondaries
    }

    // ============================
    // PRIMARY
    // ============================

    // Initial truth (only once)
    if(fRunAction->E == 0)
    {
        fRunAction->E = track->GetKineticEnergy();

        fRunAction->x = track->GetPosition().x();
        fRunAction->y = track->GetPosition().y();
        fRunAction->z = track->GetPosition().z();

        fRunAction->px = track->GetMomentum().x();
        fRunAction->py = track->GetMomentum().y();
        fRunAction->pz = track->GetMomentum().z();

        fRunAction->theta =
            track->GetMomentumDirection().theta();

        fRunAction->phi =
            track->GetMomentumDirection().phi();

        fRunAction->costh =
            track->GetMomentumDirection().z();
    }

    // Final state: only when primary stops
    if(track->GetTrackStatus() != fAlive)

      //    if(track->GetTrackStatus() == fStopAndKill)
    {

G4cout
      << "Primary ended with status "
      << track->GetTrackStatus()
      << " KE = "
      << track->GetKineticEnergy()/CLHEP::MeV
      << " MeV"
      << G4endl;
      
        fRunAction->finalE =
            track->GetKineticEnergy();

        fRunAction->finalX =
            track->GetPosition().x();

        fRunAction->finalY =
            track->GetPosition().y();

        fRunAction->finalZ =
            track->GetPosition().z();
    }
}
*/
/*

void SteppingAction::UserSteppingAction(const G4Step* step) {
    auto track = step->GetTrack();
    if(!fRunAction || !fEventAction) return;
    fEventAction->IncrementStep(); //count all steps
    fEventAction->AddEdep(step->GetTotalEnergyDeposit()); // edep all particles
    if(track->GetParentID() > 0 &&
       track->GetCurrentStepNumber() == 1)
    {
        fRunAction->nSecondaries++;
	fRunAction->secTotalE += track->GetKineticEnergy();

        // ---- Particle type
        auto name =
          track->GetDefinition()->GetParticleName();

        if(name == "gamma") fRunAction->nGamma++;
        if(name == "e-")    fRunAction->nElectron++;
        if(name == "e+")    fRunAction->nPositron++;

        // ---- Z position
        double z = track->GetPosition().z();

        if(z < fRunAction->secFirstZ)
            fRunAction->secFirstZ = z;

        if(z > fRunAction->secLastZ)
            fRunAction->secLastZ = z;

	 // ---- Backward tracks
        if(track->GetMomentumDirection().z() < 0)
            fRunAction->nBackward++;

        // ---- Creator process
        auto proc = track->GetCreatorProcess();

        if(proc)
        {
            auto pname = proc->GetProcessName();

            if(pname == "compt")
                fRunAction->nCompton++;

            if(pname == "conv")
                fRunAction->nPairProd++;

            if(pname == "eIoni")
                fRunAction->nIonisation++;
        }
    

    }
    
 fRunAction->secTrackLength +=
        step->GetStepLength();
 
    if(track->GetParentID() != 0) return; // only primary


    //Initial truth
       if(fRunAction->E == 0) {
        fRunAction->E = track->GetKineticEnergy();
        fRunAction->x = track->GetPosition().x();
        fRunAction->y = track->GetPosition().y();
        fRunAction->z = track->GetPosition().z();

        fRunAction->px = track->GetMomentum().x();
        fRunAction->py = track->GetMomentum().y();
        fRunAction->pz = track->GetMomentum().z();

        fRunAction->theta = track->GetMomentumDirection().theta();
        fRunAction->phi = track->GetMomentumDirection().phi();
        fRunAction->costh = track->GetMomentumDirection().z();

    }


    
    // accumulate event info
       fEventAction->AddEdep(step->GetTotalEnergyDeposit());
       fEventAction->IncrementStep();
       fRunAction->finalE = track->GetKineticEnergy();

       
    if(track->GetTrackStatus() == fStopAndKill) {

        fRunAction->finalE = track->GetKineticEnergy();

        fRunAction->finalX = track->GetPosition().x();
        fRunAction->finalY = track->GetPosition().y();
        fRunAction->finalZ = track->GetPosition().z();
    }
}


*/
