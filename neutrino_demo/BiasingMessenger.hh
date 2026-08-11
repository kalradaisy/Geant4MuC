#ifndef BiasingMessenger_h
#define BiasingMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class Biasing;
class G4UIdirectory;
class G4UIcmdWithADouble;

class BiasingMessenger : public G4UImessenger
{
public:
    BiasingMessenger(Biasing* biasing);
    virtual ~BiasingMessenger();

    virtual void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    Biasing* fBiasing;
    G4UIdirectory* fBiasingDir;
    G4UIcmdWithADouble* fMuNuNucleusCcCmd;
    G4UIcmdWithADouble* fMuNuNucleusNcCmd;
    G4UIcmdWithADouble* fMuANuNucleusCcCmd;
    G4UIcmdWithADouble* fMuANuNucleusNcCmd;
    G4UIcmdWithADouble* fElNuNucleusCcCmd;
    G4UIcmdWithADouble* fElNuNucleusNcCmd;
    G4UIcmdWithADouble* fElANuNucleusCcCmd;
    G4UIcmdWithADouble* fElANuNucleusNcCmd;
    //G4UIcmdWithADouble* fNuElectronCmd;
    // Split CC and NC commands
    G4UIcmdWithADouble* fNuElectronCcCmd;
    G4UIcmdWithADouble* fNuElectronNcCmd;
};

#endif