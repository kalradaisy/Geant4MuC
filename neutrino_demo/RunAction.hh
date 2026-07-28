#ifndef RUNACTION_H
#define RUNACTION_H

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"

class G4Run;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    // You no longer need to store variables like "E, x, y, z" in RunAction!
    // The Analysis Manager handles the memory internally.

    // We do still need to store some vectors here because of the way GEANT4
    // handles memory

    std::vector<G4double> secEnergies; // Stores seconadry energies
    std::vector<G4double> secWeights; // Stores secondary statistical weights

private:
    G4String fBaseName;
};

#endif
