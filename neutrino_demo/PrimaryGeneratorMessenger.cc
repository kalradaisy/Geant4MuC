#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGenerator.hh"
#include "G4UIdirectory.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGenerator* gun)
: fGun(gun)
{
    // Directory for a gun
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

    /*Need to actually add this at some point
    /*
    fDirectionCmd = new G4UIcmdWith3Vector("/gun/direction", this);
    fDirectionCmd->SetGuidance("Set particle direction");
    fDirectionCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    */
    
    // Directory for flux
    fGeneratorDir = new G4UIdirectory("/generator/");
    fGeneratorDir->SetGuidance("Generator mode selection and file setup");
    
    fModeCmd = new G4UIcmdWithAString("/generator/mode", this);
    fModeCmd->SetGuidance("Set the generator mode: 'gun' or 'flux'");
    fModeCmd->SetParameterName("mode", false);
    fModeCmd->SetCandidates("gun flux genie");
    fModeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    
    fFluxFileCmd = new G4UIcmdWithAString("/generator/fluxFile", this);
    fFluxFileCmd->SetGuidance("Set the path to the ROOT flux file");
    fFluxFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if(command == fParticleCmd) fGun->SetParticleName(newValue);
    // Gun commands
    else if(command == fEnergyCmd) fGun->SetEnergy(fEnergyCmd->GetNewDoubleValue(newValue));
    else if(command == fPositionCmd) fGun->SetPosition(fPositionCmd->GetNew3VectorValue(newValue));
    //else if(command == fDirectionCmd) fGun->SetDirection(fDirectionCmd->GetNew3VectorValue(newValue));
    // Flux commands
    else if(command == fModeCmd) fGun->SetMode(newValue);
    else if(command == fFluxFileCmd) fGun->SetFluxFile(newValue);
}
