#ifndef MYNEUTRINOPHYSICS_HH
#define MYNEUTRINOPHYSICS_HH

#include "G4VPhysicsConstructor.hh"

class MyNeutrinoPhysics : public G4VPhysicsConstructor {
public:
    MyNeutrinoPhysics(const G4String& name="MyNeutrinoPhysics");
    virtual ~MyNeutrinoPhysics();

    virtual void ConstructParticle() override;
    virtual void ConstructProcess() override;
};

#endif
