#pragma once
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "PrimaryGenerator.hh"

class PrimaryGeneratorMessenger : public G4UImessenger {
public:
    PrimaryGeneratorMessenger(PrimaryGenerator* gun);
    ~PrimaryGeneratorMessenger() override {}

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    PrimaryGenerator* fGun;

    G4UIcmdWithAString* fParticleCmd;
    G4UIcmdWithADoubleAndUnit* fEnergyCmd;
    G4UIcmdWith3Vector* fPositionCmd;
};
