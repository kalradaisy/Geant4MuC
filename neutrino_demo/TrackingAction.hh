#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class TrackingAction : public G4UserTrackingAction {
  public:
    TrackingAction();
    virtual ~TrackingAction();

    // Method called at the exact moment a track is created
    virtual void PreUserTrackingAction(const G4Track* track) override;
    
    // Method called at the exact moment a track is destroyed/finishes
    virtual void PostUserTrackingAction(const G4Track* track) override;
};

#endif