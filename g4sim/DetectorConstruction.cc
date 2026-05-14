#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Exception.hh"
#include "G4RunManager.hh"
#include "G4GDMLParser.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"


DetectorConstruction::DetectorConstruction()
{
    fMessenger = new DetectorMessenger(this);

    // Build a minimal air-filled world so Construct() always has a valid
    // world volume even before any GDML file is loaded via /detector/readGDML.
    G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    G4Box*           worldSolid = new G4Box("DefaultWorld", 5.*m, 5.*m, 5.*m);
    G4LogicalVolume* worldLV    = new G4LogicalVolume(worldSolid, air, "DefaultWorld");
    fWorld = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV,
                               "DefaultWorld", nullptr, false, 0);

    G4cout << "DetectorConstruction: default stub world created. "
           << "Use /detector/readGDML <file> in your macro to load a geometry."
           << G4endl;
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

    fWorld = fParser.GetWorldVolume();
    if (!fWorld) {
        G4cerr << "GDML read, but world volume is NULL!" << G4endl;
        G4cerr << "Check that your GDML defines a <world> and all solids/materials." << G4endl;

        G4Exception("DetectorConstruction::ReadGDML",
                    "NoGDML",
                    FatalException,
                    "World volume is NULL after reading GDML.");
    }

    G4cout << "GDML loaded successfully. World volume: "
           << fWorld->GetName() << G4endl;

    // Reinitialize geometry after macro reload
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    if (!fWorld) {
        G4Exception("DetectorConstruction::Construct()",
                    "NoGDML",
                    FatalException,
                    "World volume is NULL. Call /detector/readGDML before /run/initialize.");
    }

    G4LogicalVolume* worldLogical = fWorld->GetLogicalVolume();
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
    // Create target region for sensitive volumes
    // -----------------------------------------
    auto targetRegion = new G4Region("target");

    auto lvStore = G4LogicalVolumeStore::GetInstance();

    G4cout << "\n=== Assigning Target Region ===" << G4endl;

    for (auto lv : *lvStore) {

      //G4cout << "Logical volume: " << lv->GetName() << G4endl;

        // Select your detector volume
	//        if (lv->GetName() == "VertexBarrel_layer0_sens") {
        if (lv->GetName().find("_sens") != std::string::npos) {
	  // if (lv == world->GetLogicalVolume()){
	  //continue;   // Skip world
	  // }
            targetRegion->AddRootLogicalVolume(lv);

            G4cout << ">>> Target region set on: "
                   << lv->GetName() << G4endl;
	     }
    }

    G4cout << "===============================\n" << G4endl;

    return fWorld;
}
