#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGenerator.hh"
#include "G4UIdirectory.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGenerator* gun)
: fGun(gun)
{
    auto dir = new G4UIdirectory("/gun/");
    dir->SetGuidance("Primary generator commands");

    fParticleCmd = new G4UIcmdWithAString("/gun/particle", this);
    fParticleCmd->SetGuidance("Set particle type");
    fParticleCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fEnergyCmd = new G4UIcmdWithADoubleAndUnit("/gun/energy", this);
    fEnergyCmd->SetGuidance("Set particle energy");
    fEnergyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fPositionCmd = new G4UIcmdWith3Vector("/gun/position", this);
    fPositionCmd->SetGuidance("Set particle position");
    fPositionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if(command == fParticleCmd) fGun->SetParticleName(newValue);
    else if(command == fEnergyCmd) fGun->SetEnergy(fEnergyCmd->GetNewDoubleValue(newValue));
    else if(command == fPositionCmd) fGun->SetPosition(fPositionCmd->GetNew3VectorValue(newValue));
}
