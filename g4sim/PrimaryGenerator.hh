#ifndef PRIMARYGENERATOR_H
#define PRIMARYGENERATOR_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "RunAction.hh"
//#include "PrimaryGeneratorMessenger.hh"

class PrimaryGeneratorMessenger;

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
  
private:
    PrimaryGeneratorMessenger* fMessenger;
    G4ParticleGun* fParticleGun;
    G4ThreeVector fPosition;
    G4ThreeVector fDirection;
    G4double fEnergy;
    G4String fParticleName;

    RunAction* fRunAction;
};

#endif
