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

    // Command for Neutrino-Electron Charged Current (CC) Bias
    fNuElectronCcCmd = new G4UIcmdWithADouble("/custom/biasing/NuElectronCcBias", this);
    fNuElectronCcCmd->SetGuidance("Set the bias factor for neutrino-electron CC interactions (nuElectronCC).");
    fNuElectronCcCmd->SetParameterName("factor", false);
    fNuElectronCcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Neutrino-Electron Neutral Current (NC) Bias
    fNuElectronNcCmd = new G4UIcmdWithADouble("/custom/biasing/NuElectronNcBias", this);
    fNuElectronNcCmd->SetGuidance("Set the bias factor for neutrino-electron NC interactions (nuElectronNC).");
    fNuElectronNcCmd->SetParameterName("factor", false);
    fNuElectronNcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Muon Neutrino-Nucleus CC Bias
    fMuNuNucleusCcCmd = new G4UIcmdWithADouble("/custom/biasing/MuNuNucleusCcBias", this);
    fMuNuNucleusCcCmd->SetGuidance("Set the bias factor for muon neutrino-nucleus processes (muNuNucleusCC).");
    fMuNuNucleusCcCmd->SetParameterName("factor", false);
    fMuNuNucleusCcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Muon Neutrino-Nucleus NC Bias
    fMuNuNucleusNcCmd = new G4UIcmdWithADouble("/custom/biasing/MuNuNucleusNcBias", this);
    fMuNuNucleusNcCmd->SetGuidance("Set the bias factor for muon neutrino-nucleus processes (muNuNucleusNC).");
    fMuNuNucleusNcCmd->SetParameterName("factor", false);
    fMuNuNucleusNcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Muon Anti Neutrino-Nucleus CC Bias
    fMuANuNucleusCcCmd = new G4UIcmdWithADouble("/custom/biasing/MuANuNucleusCcBias", this);
    fMuANuNucleusCcCmd->SetGuidance("Set the bias factor for muon anti neutrino-nucleus processes (muANuNucleusCC).");
    fMuANuNucleusCcCmd->SetParameterName("factor", false);
    fMuANuNucleusCcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Muon Anti Neutrino-Nucleus NC Bias
    fMuANuNucleusNcCmd = new G4UIcmdWithADouble("/custom/biasing/MuANuNucleusNcBias", this);
    fMuANuNucleusNcCmd->SetGuidance("Set the bias factor for muon anti neutrino-nucleus processes (muANuNucleusNC).");
    fMuANuNucleusNcCmd->SetParameterName("factor", false);
    fMuANuNucleusNcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Electron Neutrino-Nucleus CC Bias
    fElNuNucleusCcCmd = new G4UIcmdWithADouble("/custom/biasing/ElNuNucleusCcBias", this);
    fElNuNucleusCcCmd->SetGuidance("Set the bias factor for electron neutrino-nucleus processes (elNuNucleusCC).");
    fElNuNucleusCcCmd->SetParameterName("factor", false);
    fElNuNucleusCcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Electron Neutrino-Nucleus NC Bias
    fElNuNucleusNcCmd = new G4UIcmdWithADouble("/custom/biasing/ElNuNucleusNcBias", this);
    fElNuNucleusNcCmd->SetGuidance("Set the bias factor for electron neutrino-nucleus processes (elNuNucleusNC).");
    fElNuNucleusNcCmd->SetParameterName("factor", false);
    fElNuNucleusNcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Electron Neutrino-Nucleus CC Bias
    fElANuNucleusCcCmd = new G4UIcmdWithADouble("/custom/biasing/ElANuNucleusCcBias", this);
    fElANuNucleusCcCmd->SetGuidance("Set the bias factor for electron anti neutrino-nucleus processes (elANuNucleusCC).");
    fElANuNucleusCcCmd->SetParameterName("factor", false);
    fElANuNucleusCcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Command for Electron Neutrino-Nucleus NC Bias
    fElANuNucleusNcCmd = new G4UIcmdWithADouble("/custom/biasing/ElANuNucleusNcBias", this);
    fElANuNucleusNcCmd->SetGuidance("Set the bias factor for electron anti neutrino-nucleus processes (elANuNucleusNC).");
    fElANuNucleusNcCmd->SetParameterName("factor", false);
    fElANuNucleusNcCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

BiasingMessenger::~BiasingMessenger()
{
    delete fMuNuNucleusCcCmd;
    delete fMuNuNucleusNcCmd;
    delete fMuANuNucleusCcCmd;
    delete fMuANuNucleusNcCmd;
    delete fElNuNucleusCcCmd;
    delete fElNuNucleusNcCmd;
    delete fElANuNucleusCcCmd;
    delete fElANuNucleusNcCmd;
    delete fNuElectronCcCmd;
    delete fNuElectronNcCmd;
    delete fBiasingDir;
}

void BiasingMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    //TODO: MAKE THESE ENUMS, THIS LOOKS DISGUSTING
    // Route the command to the Biasing class based on the process name
    if (command == fMuNuNucleusCcCmd) {
        fBiasing->SetBiasFactor("muNuNucleusCC", fMuNuNucleusCcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fMuNuNucleusNcCmd) {
        fBiasing->SetBiasFactor("muNuNucleusNC", fMuNuNucleusNcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fMuANuNucleusCcCmd) {
        fBiasing->SetBiasFactor("muANuNucleusCC", fMuANuNucleusCcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fMuANuNucleusNcCmd) {
        fBiasing->SetBiasFactor("muANuNucleusNC", fMuANuNucleusNcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fElNuNucleusCcCmd) {
        fBiasing->SetBiasFactor("elNuNucleusCC", fElNuNucleusCcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fElNuNucleusNcCmd) {
        fBiasing->SetBiasFactor("elNuNucleusNC", fElNuNucleusNcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fElANuNucleusCcCmd) {
        fBiasing->SetBiasFactor("elANuNucleusCC", fElANuNucleusCcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fElANuNucleusNcCmd) {
        fBiasing->SetBiasFactor("elANuNucleusNC", fElANuNucleusNcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fNuElectronCcCmd) {
        fBiasing->SetBiasFactor("nuElectronCC", fNuElectronCcCmd->GetNewDoubleValue(newValue));
    }
    else if (command == fNuElectronNcCmd) {
        fBiasing->SetBiasFactor("nuElectronNC", fNuElectronNcCmd->GetNewDoubleValue(newValue));
    }
}