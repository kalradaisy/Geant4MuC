#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Exception.hh"
#include "G4RunManager.hh"
#include "G4GDMLParser.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"
#include "G4Material.hh"


DetectorConstruction::DetectorConstruction()
{
    fMessenger = new DetectorMessenger(this);
    /* Moved the old default geometry loading out of here so that it is only
    loaded if the macro specifies no geometry itself. This way, macros can
    define their own geometries without need for recompilation.*/

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

G4VPhysicalVolume* DetectorConstruction::Construct(){
    
    G4VPhysicalVolume* world = fParser.GetWorldVolume();
    // If no macro loaded a geometry, load the default now
    if (!world) {
        G4String defaultGDML = "../MAIA.gdml";
        G4cout << "No macro geometry specified. Loading default GDML: " << defaultGDML << G4endl;
        
        fParser.Read(defaultGDML, false); // false disables schema validation
        world = fParser.GetWorldVolume();

        if (!world) {
            G4Exception("DetectorConstruction::Construct()",
                        "NoGDML",
                        FatalException,
                        "World volume is NULL. GDML was not loaded correctly.");
        }
        G4cout << "Default GDML loaded successfully. World volume: "
               << world->GetName() << G4endl;
    }

    G4LogicalVolume* worldLogical = world->GetLogicalVolume();

    // Save the world volume for the worker threads (ADDED FOR BETTER BIASING)
    fWorldLogical = worldLogical;

    if (worldLogical) {
        G4double minRange = 1.0*mm;  // you can reduce if needed
      // maxStep = 10*cm, maxTrackLength = infinity, maxTime = infinity, minKineticEnergy = 0, minRange = 1*mm
    G4UserLimits* stepLimits = new G4UserLimits(10*cm, DBL_MAX, DBL_MAX, 0., 1.0*mm);
    worldLogical->SetUserLimits(stepLimits);
 
      G4cout << "Minimum step size set for world: " << minRange/mm << " mm" << G4endl;
    } else {
        G4cerr << "World logical volume not found!" << G4endl;
    }
    

    // -----------------------------------------
    // Create neutrino target region
    // -----------------------------------------
    //auto targetRegion = new G4Region("target");
    fTargetRegion = new G4Region("target");

    auto lvStore = G4LogicalVolumeStore::GetInstance();

    G4cout << "\n=== Assigning Neutrino Region ===" << G4endl;

    for (auto lv : *lvStore) {
        // 1. Skip the World volume
        if (lv == fWorldLogical) continue;

        // 2. Skip Air, Vacuum, or Galactic volumes
        if (lv->GetMaterial()) {
            G4String matName = lv->GetMaterial()->GetName();
            if (matName.find("Air") != std::string::npos || 
                matName.find("Vacuum") != std::string::npos ||
                matName.find("vacuum") != std::string::npos ||
                matName.find("Galactic") != std::string::npos) {
                continue; 
            }
        }

        // 3. Add everything else to the target region
        fTargetRegion->AddRootLogicalVolume(lv);
        fTargetVolumes.push_back(lv);
        G4cout << ">>> Neutrino target set on: " << lv->GetName() << G4endl;
    }

    G4cout << "===============================\n" << G4endl;

    return world;
}
