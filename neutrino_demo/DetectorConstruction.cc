#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Exception.hh"
#include "G4RunManager.hh"
#include "G4GDMLParser.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"


DetectorConstruction::DetectorConstruction()
{
    fMessenger = new DetectorMessenger(this);
    // Load default GDML immediately so Construct() can return a valid world
    G4String defaultGDML = "neutrino_demo/output.gdml";
    G4cout << "Loading default GDML: " << defaultGDML << G4endl;

    fParser.Read(defaultGDML, false); // false disables schema validation

    G4VPhysicalVolume* world = fParser.GetWorldVolume();
    if (!world) {
        G4cerr << "Failed to load default GDML world volume!" << G4endl;
        G4Exception("DetectorConstruction::DetectorConstruction",
                    "NoGDML",
                    FatalException,
                    "World volume is NULL after reading default GDML.");
    }

    G4cout << "Default GDML loaded successfully. World volume: "
           << world->GetName() << G4endl;

}

DetectorConstruction::~DetectorConstruction()
{
    delete fMessenger;
}

// Allows macro-based GDML reloading
void DetectorConstruction::ReadGDML(const G4String& filename)
{
    G4cout << "Reading GDML file: " << filename << G4endl;

    fParser.Read(filename, false);  // false disables schema validation

    G4VPhysicalVolume* world = fParser.GetWorldVolume();
    if (!world) {
        G4cerr << "GDML read, but world volume is NULL!" << G4endl;
        G4cerr << "Check that your GDML defines a <world> and all solids/materials." << G4endl;

        G4Exception("DetectorConstruction::ReadGDML",
                    "NoGDML",
                    FatalException,
                    "World volume is NULL after reading GDML.");
    }

    G4cout << "GDML loaded successfully. World volume: "
           << world->GetName() << G4endl;

    // Reinitialize geometry after macro reload
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4VPhysicalVolume* world = fParser.GetWorldVolume();

    if (!world) {
        G4Exception("DetectorConstruction::Construct()",
                    "NoGDML",
                    FatalException,
                    "World volume is NULL. GDML was not loaded correctly.");
    }

    G4LogicalVolume* worldLogical = world->GetLogicalVolume();
    if (worldLogical) {
             G4double minStep = 1.0*mm;  // you can reduce if needed
      G4UserLimits* stepLimits = new G4UserLimits(minStep);
      worldLogical->SetUserLimits(stepLimits);
      G4double maxStep = 10*cm;  // limit max step to 10 cm
      worldLogical->SetUserLimits(new G4UserLimits(maxStep));
 
      G4cout << "Minimum step size set for world: " << minStep/mm << " mm" << G4endl;
    } else {
        G4cerr << "World logical volume not found!" << G4endl;
    }
    

    // -----------------------------------------
    // Create neutrino target region
    // -----------------------------------------
    auto targetRegion = new G4Region("target");

    auto lvStore = G4LogicalVolumeStore::GetInstance();

    G4cout << "\n=== Assigning Neutrino Region ===" << G4endl;

    for (auto lv : *lvStore) {

      //G4cout << "Logical volume: " << lv->GetName() << G4endl;

        // Select your detector volume
	//        if (lv->GetName() == "VertexBarrel_layer0_sens") {
        if (lv->GetName().find("_sens") != std::string::npos) {
	  // if (lv == world->GetLogicalVolume()){
	  //continue;   // Skip world
	  // }
            targetRegion->AddRootLogicalVolume(lv);

            G4cout << ">>> Neutrino target set on: "
                   << lv->GetName() << G4endl;
	     }
    }

    G4cout << "===============================\n" << G4endl;

    return world;
}
