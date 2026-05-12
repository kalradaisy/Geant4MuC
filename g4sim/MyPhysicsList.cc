#include "MyPhysicsList.hh"
#include "G4UImanager.hh"
#include "MyNeutrinoPhysics.hh"


MyPhysicsList::MyPhysicsList() {
    // --- 1. Get base physics list from factory ---
    G4PhysListFactory factory;
    G4VModularPhysicsList* basePhysics = factory.GetReferencePhysList("QGSP_BERT");

    // --- 2. Add QGSP_BERT physics modules manually ---
    // You cannot copy them, so just register QGSP_BERT directly
    SetVerboseLevel(basePhysics->GetVerboseLevel()); // optional

        RegisterPhysics(new MyNeutrinoPhysics("NuBeam"));

    // --- 3. Register neutrino physics ---
    //    RegisterPhysics(new MyNeutrinoPhysics());


    // set detector name here
    //    G4UImanager* ui = G4UImanager::GetUIpointer();
    //ui->ApplyCommand("/physics_lists/em/NuDetectorName MAIADetector");
    
}
