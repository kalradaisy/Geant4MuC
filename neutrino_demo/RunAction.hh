#ifndef RUNACTION_H
#define RUNACTION_H

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"

// With any luck, the includes below can be 86'd
#include "TFile.h"
#include "TTree.h"

class G4Run;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    // You no longer need to store variables like "E, x, y, z" in RunAction!
    // The Analysis Manager handles the memory internally.

private:
    G4String fBaseName;
};

#endif
