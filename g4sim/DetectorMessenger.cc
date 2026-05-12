#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* det)
: fDetector(det)
{
    fReadGDMLCmd =
      new G4UIcmdWithAString("/detector/readGDML", this);

    fReadGDMLCmd->SetGuidance("Load GDML geometry file");
    fReadGDMLCmd->SetParameterName("filename", false);
}

DetectorMessenger::~DetectorMessenger()
{
    delete fReadGDMLCmd;
}

void DetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String val)
{
    if (cmd == fReadGDMLCmd) {
        fDetector->ReadGDML(val);
    }
}
