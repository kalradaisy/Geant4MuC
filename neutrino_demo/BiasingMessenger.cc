#include "BiasingMessenger.hh"
#include "Biasing.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"

BiasingMessenger::BiasingMessenger(Biasing* biasing)
  : G4UImessenger(), fBiasing(biasing)
{
    // Create the directory for your custom commands
    fBiasingDir = new G4UIdirectory("/custom/biasing/");
    fBiasingDir->SetGuidance("Custom UI commands for neutrino biasing.");

    // Command for Muon Neutrino-Nucleus Bias
    fMuNuNucleusCmd = new G4UIcmdWithADouble("/custom/biasing/MuNuNucleusBias", this);
    fMuNuNucleusCmd->SetGuidance("Set the bias factor for muon neutrino-nucleus processes (muNuNucleus).");
    fMuNuNucleusCmd->SetParameterName("factor", false);
    fMuNuNucleusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Electron Neutrino-Nucleus Bias
    fElNuNucleusCmd = new G4UIcmdWithADouble("/custom/biasing/ElNuNucleusBias", this);
    fElNuNucleusCmd->SetGuidance("Set the bias factor for electron neutrino-nucleus processes (elNuNucleus).");
    fElNuNucleusCmd->SetParameterName("factor", false);
    fElNuNucleusCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Neutrino-Electron Bias
    fNuElectronCmd = new G4UIcmdWithADouble("/custom/biasing/NuElectronBias", this);
    fNuElectronCmd->SetGuidance("Set the bias factor for neutrino-electron processes (nuElectron).");
    fNuElectronCmd->SetParameterName("factor", false);
    fNuElectronCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

BiasingMessenger::~BiasingMessenger()
{
    delete fMuNuNucleusCmd;
    delete fElNuNucleusCmd;
    delete fNuElectronCmd;
    delete fBiasingDir;
}

void BiasingMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    // Route the command to the Biasing class based on the process name
    if (command == fMuNuNucleusCmd) {
        fBiasing->SetBiasFactor("muNuNucleus", fMuNuNucleusCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fElNuNucleusCmd) {
        fBiasing->SetBiasFactor("elNuNucleus", fElNuNucleusCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fNuElectronCmd) {
        fBiasing->SetBiasFactor("nuElectron", fNuElectronCmd->GetNewDoubleValue(newValue));
    }
}