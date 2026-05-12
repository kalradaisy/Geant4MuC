#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

class DetectorMessenger;   // Forward declaration

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();

    void ReadGDML(const G4String& filename);

private:
    G4GDMLParser fParser;
    DetectorMessenger* fMessenger;
};

#endif
