#ifndef MyNeutrinoPhysics_h
#define MyNeutrinoPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class MyNeutrinoPhysicsMessenger;

class MyNeutrinoPhysics : public G4VPhysicsConstructor
{
public:
    explicit MyNeutrinoPhysics(const G4String& name = "MyNeutrinoPhys");
    ~MyNeutrinoPhysics() override;

    void ConstructParticle() override;
    void ConstructProcess() override;

    void SetNuEleCcBias(G4double bf);
    void SetNuEleNcBias(G4double bf);
    void SetNuDetectorName(const G4String& name);

private:
    G4double fNuEleCcBias = 1.0;
    G4double fNuEleNcBias = 1.0;
    G4String fNuDetectorName = "0";

    MyNeutrinoPhysicsMessenger* fMessenger = nullptr;
};

#endif