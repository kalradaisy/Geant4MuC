#ifndef PRIMARYGENERATOR_H
#define PRIMARYGENERATOR_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "RunAction.hh"
#include <atomic>

// Forward declarations for ROOT
class TFile;
class TTree;
class PrimaryGeneratorMessenger;

class PrimaryGeneratorMessenger;
// Could the above be removed?

class PrimaryGenerator : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGenerator(RunAction* runAction);
    ~PrimaryGenerator() override;

    void GeneratePrimaries(G4Event* event) override;

    // setters for messenger
    void SetParticleName(const G4String& name) { fParticleName = name; }
    void SetEnergy(G4double energy) { fEnergy = energy; }
    void SetPosition(const G4ThreeVector& pos) { fPosition = pos; }
    void SetDirection(const G4ThreeVector& dir) { fDirection = dir; }

    // setters for dual mode system
    void SetMode(const G4String& mode) { fMode = mode; }
    void SetFluxFile(const G4String& file) { fFluxFileName = file; }
  
private:
    PrimaryGeneratorMessenger* fMessenger;
    G4ParticleGun* fParticleGun;

    // Gun variables
    G4ThreeVector fPosition;
    G4ThreeVector fDirection;
    G4double fEnergy;
    G4String fParticleName;

    // Dual-Mode Variables
    G4String fMode;
    G4String fFluxFileName;
    
    // Flux Variables
    TFile* fFluxFile;
    TTree* fFluxTree;
    int fTotalEntries;
    // Variables to hold current row data from ROOT for flux
    int fPDG;
    double fE_GeV, fX_mm, fY_mm, fPx, fPy, fPz;
    double fZ_mm; // just for positioning the flux in the appropriate place
    
    // --- GENIE Summary Tree (gst) Variables ---
    int fNF{0};
    int fPDG_arr[250]{0};
    double fPx_arr[250]{0.0};
    double fPy_arr[250]{0.0};
    double fPz_arr[250]{0.0};
    double fVtxX{0.0}, fVtxY{0.0}, fVtxZ{0.0};

    RunAction* fRunAction;
};

#endif
