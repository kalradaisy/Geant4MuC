#ifndef MYPHYSICSLIST_HH
#define MYPHYSICSLIST_HH

#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "MyNeutrinoPhysics.hh"

class MyPhysicsList : public G4VModularPhysicsList {
public:
    MyPhysicsList();
    ~MyPhysicsList() override {}
  //  void SetDetectorName(const G4String& name) { fDetectorName = name; }
  //G4String GetDetectorName() const { return fDetectorName; }
  
  //private:
  //G4String fDetectorName;

};

#endif
