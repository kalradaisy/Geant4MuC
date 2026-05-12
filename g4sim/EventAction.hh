#ifndef EVENTACTION_H
#define EVENTACTION_H

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "RunAction.hh"       // if you use RunAction inside EventAction
#include "globals.hh"


class EventAction : public G4UserEventAction {
public:
    EventAction(RunAction* runAction);   // pass pointer to RunAction if needed
    ~EventAction() override;

    // Override correct function
  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event* event) override;

  void AddEdep(double edep) { totalEdep_ += edep; }
  void IncrementStep() { nSteps_++; }
  G4bool neutrinoInteractionPrinted = false;
  
private:
    RunAction* fRunAction;
 // Per-event accumulators
    double totalEdep_ = 0;
    int nSteps_ = 0;
};

#endif
