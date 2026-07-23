#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

class DetectorMessenger;   // Forward declaration

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    std::vector<G4LogicalVolume*> fTargetVolumes;
    const std::vector<G4LogicalVolume*>& GetTargetVolumes() const
{
    return fTargetVolumes;
}
    DetectorConstruction();
    virtual ~DetectorConstruction() override;

    virtual G4VPhysicalVolume* Construct() override;

    void ConstructSDandField() override;
    // Adding this line to allow worker threads to bias neutrino interactions

    void ReadGDML(const G4String& filename);
    G4Region* GetTargetRegion() const { return fTargetRegion; }

private:
    G4GDMLParser fParser;
    G4Region* fTargetRegion = nullptr;
    DetectorMessenger* fMessenger;
    G4LogicalVolume* fWorldLogical = nullptr;
};

#endif
