#ifndef TRACKINGACTION_HH
#define TRACKINGACTION_HH

#include "G4UserTrackingAction.hh"
#include "EventAction.hh"

class TrackingAction : public G4UserTrackingAction
{
public:
    TrackingAction(EventAction* eventAction);
    ~TrackingAction() override = default;

    void PreUserTrackingAction(const G4Track* track) override;

private:
    EventAction* fEventAction;
};

#endif
