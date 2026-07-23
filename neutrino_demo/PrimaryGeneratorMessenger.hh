#pragma once
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIdirectory.hh"
#include "PrimaryGenerator.hh"

class PrimaryGeneratorMessenger : public G4UImessenger {
public:
    PrimaryGeneratorMessenger(PrimaryGenerator* gun);
    ~PrimaryGeneratorMessenger() override {}

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    PrimaryGenerator* fGun;

    // General commands
    G4UIcmdWith3Vector* fPositionCmd;
    //G4UIcmdWith3Vector* fDirectionCmd;

    // Commands for firing a gun
    G4UIcmdWithAString* fParticleCmd;
    G4UIcmdWithADoubleAndUnit* fEnergyCmd;

    // Commands for using a flux distribution
    G4UIdirectory* fGeneratorDir;
    G4UIcmdWithAString* fModeCmd;
    G4UIcmdWithAString* fFluxFileCmd;
};
