#pragma once
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

class EventAction;
class RunAction;
class G4Step;
class RunAction;

class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction(EventAction* eventAction, RunAction* runAction );
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;

private:
    EventAction* fEventAction;
  RunAction* fRunAction;
};
