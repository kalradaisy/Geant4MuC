#include "EventAction.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "RunAction.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

EventAction::EventAction(RunAction* runAction)
  : fRunAction(runAction),
     totalEdep_(0),
    nSteps_(0)
{}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    if(!fRunAction) return;

    // Reset variables
    fRunAction->E = 0;
    fRunAction->x = 0;
    fRunAction->y = 0;
    fRunAction->z = 0;

    fRunAction->px = 0;
    fRunAction->py = 0;
    fRunAction->pz = 0;

    fRunAction->theta = 0;
    fRunAction->phi = 0;

    totalEdep_ = 0;
    nSteps_ = 0;

    // -----------------------------
    // Get primary particle info
    // -----------------------------
    auto vertex = event->GetPrimaryVertex();

    if(vertex) {
        auto primary = vertex->GetPrimary();

        if(primary) {
            fRunAction->E = primary->GetKineticEnergy();

            fRunAction->x = vertex->GetX0();
            fRunAction->y = vertex->GetY0();
            fRunAction->z = vertex->GetZ0();

            fRunAction->px = primary->GetPx();
            fRunAction->py = primary->GetPy();
            fRunAction->pz = primary->GetPz();

            G4ThreeVector p(primary->GetPx(),
                            primary->GetPy(),
                            primary->GetPz());

            fRunAction->theta = p.theta();
            fRunAction->phi   = p.phi();
        }
    }
}

/*

void EventAction::BeginOfEventAction(const G4Event*)
{
    if(!fRunAction) return;

    fRunAction->E = 0;
    fRunAction->x = 0;
    fRunAction->y = 0;
    fRunAction->z = 0;

    fRunAction->finalE = 0;
    fRunAction->finalX = 0;
    fRunAction->finalY = 0;
    fRunAction->finalZ = 0;

    fRunAction->px = 0; fRunAction->py = 0; fRunAction->pz = 0;
    fRunAction->theta = 0; fRunAction->phi = 0;
    fRunAction->totalEdep = 0; fRunAction->nSteps = 0; fRunAction->nSecondaries = 0;
    totalEdep_ = 0;
    nSteps_ = 0;
    neutrinoInteractionPrinted = false;

// Secondary counters
    
    fRunAction->nGamma = 0;
    fRunAction->nElectron = 0;
    fRunAction->nPositron = 0;
    fRunAction->nProtonSec = 0;
    fRunAction->nNeutron = 0;
    fRunAction->nPionPlus = 0;
    fRunAction->nPionMinus = 0;
    fRunAction->nPionZero = 0;
 

 
fRunAction->secTotalE = 0.0;
fRunAction->secMeanE = 0.0;
fRunAction->secTrackLength = 0.0;

fRunAction->secFirstZ = 999999.;   // large number
fRunAction->secLastZ  = -999999.;  // small number

fRunAction->nBackward = 0;

// Process counters
fRunAction->nCompton = 0;
fRunAction->nPairProd = 0;
fRunAction->nIonisation = 0;
}
*/

void EventAction::EndOfEventAction(const G4Event*)
{
    if(!fRunAction) return;

    fRunAction->totalEdep = totalEdep_;
    fRunAction->nSteps = nSteps_;

    if(fRunAction->nSecondaries > 0)
{
    fRunAction->secMeanE =
        fRunAction->secTotalE /
        fRunAction->nSecondaries;
}
else
{
    fRunAction->secMeanE = 0.0;
}
    

    
    // Fill ONCE
    fRunAction->GetTree()->Fill();
}

/*
void EventAction::EndOfEventAction(const G4Event* event) {
    if(!fRunAction) return;

    fRunAction->totalEdep = totalEdep_;
    fRunAction->nSteps = nSteps_;

    fRunAction->nSecondaries = 0;
    auto trackCol = event->GetTrajectoryContainer();
    if(trackCol) {
        for(size_t i = 0; i < trackCol->entries(); ++i) {
            auto traj = (G4Trajectory*)(*trackCol)[i];
            if(traj->GetParentID() == 1) fRunAction->nSecondaries++;
        }
    }

    fRunAction->GetTree()->Fill();
}
*/

/*

void EventAction::EndOfEventAction(const G4Event* event) {
    if(!fRunAction) return;

    // Get primary particle
    auto vertex = event->GetPrimaryVertex(0);
    if(vertex) {
        auto primary = vertex->GetPrimary();
        fRunAction->px = primary->GetPx();
        fRunAction->py = primary->GetPy();
        fRunAction->pz = primary->GetPz();

        fRunAction->theta = primary->GetMomentumDirection().theta();
        fRunAction->phi   = primary->GetMomentumDirection().phi();
    }
    fRunAction->totalEdep = totalEdep_;
    fRunAction->nSteps = nSteps_;
fRunAction->nSecondaries = 0;
    auto trackCol = event->GetTrajectoryContainer();
    if(trackCol) {
              for(size_t i = 0; i < trackCol->entries(); ++i) {
            auto traj = (G4Trajectory*)(*trackCol)[i];
if(traj->GetParentID() == 1) fRunAction->nSecondaries++;
	      }
    }
    fRunAction->GetTree()->Fill();
    }*/

/*
    
  void EventAction::EndOfEventAction(const G4Event* event) {
    if(!fRunAction) return;

    // Get primary particle
    auto vertex = event->GetPrimaryVertex(0);
    if(vertex) {
        auto primary = vertex->GetPrimary();
        fRunAction->px = primary->GetPx();
        fRunAction->py = primary->GetPy();
        fRunAction->pz = primary->GetPz();

        fRunAction->theta = primary->GetMomentumDirection().theta();
        fRunAction->phi   = primary->GetMomentumDirection().phi();
    }

    // Copy accumulators from stepping
    fRunAction->totalEdep = totalEdep_;
    fRunAction->nSteps = nSteps_;

    
    
 G4PrimaryParticle* primary = event->GetPrimaryVertex(0)->GetPrimary();
    fRunAction->px = primary->GetPx();
    fRunAction->py = primary->GetPy();
    fRunAction->pz = primary->GetPz();

    fRunAction->theta = primary->GetMomentumDirection().theta();
    fRunAction->phi = primary->GetMomentumDirection().phi();

    fRunAction->totalEdep = 0;
    fRunAction->nSteps = 0;
    fRunAction->nSecondaries = 0;

    // loop over tracks to get energy deposition
    auto trackCol = event->GetTrajectoryContainer();
    for(int i=0; i<trackCol->entries(); ++i) {
        auto traj = (G4Trajectory*)(*trackCol)[i];
        fRunAction->totalEdep += traj->GetTrack()->GetTotalEnergyDeposit();
        fRunAction->nSteps += traj->GetTrack()->GetCurrentStepNumber();
        fRunAction->nSecondaries += traj->GetTrack()->GetDynamicParticle()->GetSecondary()->size();
    }

    fRunAction->GetTree()->Fill();
    }*/


/*  if(!fRunAction || !fRunAction->GetTree()) return;

    auto trajContainer = event->GetTrajectoryContainer();
    if(!trajContainer || trajContainer->size() == 0) return;

    for(size_t i = 0; i < trajContainer->size(); ++i) {
        auto traj = static_cast<G4Trajectory*>((*trajContainer)[i]);
        if(!traj) continue;

        size_t nPoints = traj->GetPointEntries();
        if(nPoints == 0) continue;  // <-- THIS IS CRITICAL

        if(traj->GetParentID() == 0) {
            auto finalPos = traj->GetPoint(nPoints-1)->GetPosition();
            fRunAction->finalE = traj->GetInitialMomentum().mag();
            fRunAction->finalX = finalPos.x();
            fRunAction->finalY = finalPos.y();
            fRunAction->finalZ = finalPos.z();
            fRunAction->GetTree()->Fill();
        }
    }
}
*/
/*
void EventAction::EndOfEventAction(const G4Event* event) {
     if(!fRunAction || !fRunAction->GetTree()) return;

    G4TrajectoryContainer* trajContainer = event->GetTrajectoryContainer();
    if(!trajContainer || trajContainer->size() == 0) return;

    for(size_t i=0; i<trajContainer->size(); ++i) {
        auto traj = static_cast<G4Trajectory*>((*trajContainer)[i]);
	//       if(traj->GetParentID() == 0) { // primary particle
        if(traj->GetParentID() != 0) continue;  // only primary
	if(traj->GetPointEntries() == 0) continue; // <--- safety check
	  auto finalPos = traj->GetPoint(traj->GetPointEntries()-1)->GetPosition();

            // Approximate energy by initial momentum magnitude
            fRunAction->finalE = traj->GetInitialMomentum().mag();
            fRunAction->finalX = finalPos.x();
            fRunAction->finalY = finalPos.y();
            fRunAction->finalZ = finalPos.z();

            fRunAction->GetTree()->Fill();
	    //   }
    }
}
*/
