#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"
#include "Biasing.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VBiasingOperator.hh"
#include "G4Material.hh"

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

void DetectorConstruction::ConstructSDandField()
{
    // This is called by EVERY worker thread!
    auto biasOp = new Biasing("NuBias");
    auto lvStore = G4LogicalVolumeStore::GetInstance();

    G4cout << "\n=== Attaching Biasing Operator on Worker Thread ===" << G4endl;
    for (auto lv : *lvStore)
    {
        // Skip World volume
        if (lv == fWorldLogical) continue;

        // Skip Air, Vacuum, or Galactic volumes (not interested in interactions there)
        // Can also add other materials we don't want to bias in
        if (lv->GetMaterial()) {
            G4String matName = lv->GetMaterial()->GetName();
            if (matName.find("Air") != std::string::npos || 
                matName.find("Vacuum") != std::string::npos ||
                matName.find("vacuum") != std::string::npos ||
                matName.find("Galactic") != std::string::npos) {
                continue; 
            }
        }

        // Attach the biasing operator to all remaining detector components
        if (G4VBiasingOperator::GetBiasingOperator(lv) == nullptr) 
        { 
            biasOp->AttachTo(lv);
            // Optional: comment out this G4cout if it prints too many lines
            G4cout << ">>> Biasing attached to: " << lv->GetName() << G4endl;
        }
    }
}
