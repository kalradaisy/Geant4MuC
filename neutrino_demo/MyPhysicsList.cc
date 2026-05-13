#include "MyPhysicsList.hh"
#include "G4UImanager.hh"
#include "G4NeutrinoPhysics.hh"
#include "G4EmExtraPhysics.hh"


MyPhysicsList::MyPhysicsList() {
    // --- 1. Get base physics list from factory ---
    G4PhysListFactory factory;
    G4VModularPhysicsList* physics = factory.GetReferencePhysList("FTFP_BERT");


    // Add neutrino physics
    physics->RegisterPhysics(new G4NeutrinoPhysics());

    // Optional but commonly used
    //    physics->RegisterPhysics(new G4EmExtraPhysics());

    //    runManager->SetUserInitialization(physics);
    
}
