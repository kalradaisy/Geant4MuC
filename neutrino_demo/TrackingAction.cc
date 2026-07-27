#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

TrackingAction::TrackingAction() 
: G4UserTrackingAction()
{}

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    // Extract these data for the track's initial state
    auto analysisManager = G4AnalysisManager::Instance();
    int currentEventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    G4ThreeVector birthPos = track->GetPosition();
    G4double birthKE = track->GetKineticEnergy();

    // Fill Ntuple 2 (Track Births)
    analysisManager->FillNtupleIColumn(2, 0, currentEventID);
    analysisManager->FillNtupleIColumn(2, 1, track->GetTrackID());
    analysisManager->FillNtupleDColumn(2, 2, birthPos.x() / mm);
    analysisManager->FillNtupleDColumn(2, 3, birthPos.y() / mm);
    analysisManager->FillNtupleDColumn(2, 4, birthPos.z() / mm);
    analysisManager->FillNtupleDColumn(2, 5, birthKE / MeV);

    analysisManager->AddNtupleRow(2);
}

void TrackingAction::PostUserTrackingAction(const G4Track* /*track*/)
{
    // We do not need to do anything at the end of the track for now
}