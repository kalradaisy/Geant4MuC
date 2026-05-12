#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4PhysListFactory.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGenerator.hh"
#include "RunAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "EventAction.hh"
//#include "G4NeutrinoPhysics.hh"
#include "MyNeutrinoPhysics.hh"
#include "MyPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "SteppingAction.hh"
//#include "ActionInitialization.hh"
#include "G4EmExtraPhysics.hh"


int main(int argc, char** argv) {

  G4PhysListFactory factory;
       auto available = factory.AvailablePhysLists();
   for (auto& name : available) {
      std::cout << name << std::endl;
    }

  // Create the run manager
    G4RunManager* runManager = new G4RunManager;

    // Detector construction
    auto detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);

    // 3. Physics list (QGSP_BERT + neutrino physics)
    //    G4PhysListFactory factory;
    G4VModularPhysicsList* physics = factory.GetReferencePhysList("FTFP_BERT"); // FTFP_BERT"); //NuBeam");
    physics->RegisterPhysics(new G4EmExtraPhysics());
    runManager->SetUserInitialization(physics);

    //  runManager->SetUserInitialization(
    //   new ActionInitialization()
    //);


    
    // Run action (must be created before PrimaryGenerator if PG uses its TTree)
     auto runAction = new RunAction();
    runManager->SetUserAction(runAction);

     auto eventAction = new EventAction(runAction);
    runManager->SetUserAction(eventAction);

    
    // Primary generator, pass runAction pointer
    auto primary = new PrimaryGenerator(runAction);
    //auto messenger = new PrimaryGeneratorMessenger(primary);
    runManager->SetUserAction(primary);

    //    auto messenger = new PrimaryGeneratorMessenger(primary);
    //runManager->SetUserAction(new EventAction(runAction));
    runManager->SetUserAction(new SteppingAction(eventAction, runAction));

    // UI / macro execution
    G4UImanager* uiManager = G4UImanager::GetUIpointer();
    //uiManager->ApplyCommand("/tracking/storeTrajectory 1");
 
    if (argc == 2) {
        uiManager->ApplyCommand("/control/execute " + std::string(argv[1]));
    }

    // Cleanup
    delete runManager;

    return 0;
}
