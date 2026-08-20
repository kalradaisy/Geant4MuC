#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4RunManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4PhysListFactory.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGenerator.hh"
#include "RunAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "EventAction.hh"
#include "G4NeutrinoPhysics.hh"
//#include "MyNeutrinoPhysics.hh"
#include "MyPhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "SteppingAction.hh"
#include "ActionInitialization.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4GenericBiasingPhysics.hh"
#include "Biasing.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4HadronicProcessStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "SplitNeutrinoPhysics.hh"

#include "TROOT.h"

int main(int argc, char** argv) {

    // Tell ROOT to make its global state thread-safe!
    ROOT::EnableThreadSafety();

  G4PhysListFactory factory;
       auto available = factory.AvailablePhysLists();
   for (auto& name : available) {
      std::cout << name << std::endl;
    }

  // Create the run manager
    //G4RunManager* runManager = new G4RunManager;
    auto* runManager = G4RunManagerFactory::CreateRunManager(
        G4RunManagerType::Default);
    /*Above modernizes the run manager and enables the multithreading
    architecture set up by ActionInitialization.hh/cc*/
    //G4UImanager* UIManager = G4UImanager::GetUIpointer();


    // Detector construction
    auto detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);

    G4VModularPhysicsList* physics = factory.GetReferencePhysList("FTFP_BERT");
    /*FTFP_BERT is used for general hadronic and collider physics. NuBeam is
    designed for neutrino physics specifically. To use one means to gain its
    benefits but lose the granularity of the other, stay vigilant.*/
    
    physics->RegisterPhysics(new G4NeutrinoPhysics());
    physics->RegisterPhysics(new SplitNeutrinoPhysics());
    auto biasing = new G4GenericBiasingPhysics();
    std::vector<G4String> processesToBias;
    processesToBias.push_back("muNuNucleusCC");
    processesToBias.push_back("muNuNucleusNC");
    processesToBias.push_back("elNuNucleusCC");
    processesToBias.push_back("elNuNucleusNC");
    processesToBias.push_back("nuElectronCC");
    processesToBias.push_back("nuElectronNC");
    biasing->PhysicsBias("nu_mu", processesToBias);
    biasing->PhysicsBias("anti_nu_mu", processesToBias);
    biasing->PhysicsBias("nu_e", processesToBias);
    biasing->PhysicsBias("anti_nu_e", processesToBias);
    physics->RegisterPhysics(biasing);

    runManager->SetUserInitialization(physics);

    runManager->SetUserInitialization(new ActionInitialization());
    // Modernized, multithreading-capable action instructions
    G4VisManager *visManager = new G4VisExecutive(); // prepares visualization
	visManager->Initialize(); // intialize

    G4UImanager *UImanager = G4UImanager::GetUIpointer();
    // prepares the UI manager, which is what we will need to tell to handle all
    // of the logic 

    G4cout << "=======================================================\n" << G4endl;

	G4UIExecutive *ui = nullptr;
    if(argc == 1){ /* if the number of command line arguments is 1 (which is
        actually 0 because the name of the function is the first command line
        argument). This pairs with the if statement below to say, if there is no
        command line argument passed, then just use the standard ui manager
        settings from the visualization macro, but, if there is a command line argument,
        treat it as the name of a macro file script containing specific ui
        manager instructions to be read and used for the ui.*/
        // if that is true, use this UI manager
        //LLM
        ui = new G4UIExecutive(argc, argv);
        UImanager->ApplyCommand("/control/execute ../newvis.mac");
        ui->SessionStart();
        delete ui;

    }

    else if (argc == 3 && G4String(argv[1]) == "-i") {
        /* If passed the -i flag followed by the name of a GENIE macro, launch
        in the interactive GENIE mode*/
        ui = new G4UIExecutive(argc, argv);
        G4String command = "/control/execute ";
        G4String fileName = argv[2];
        UImanager->ApplyCommand(command + fileName);
        ui->SessionStart(); // Keeps GUI open!
        delete ui;
    }

    
    else{ /* if we aren't doing visualization, run in batch mode
        as the name of our input script*/
        G4String command = "/control/execute ";
        // string for the command
        G4String fileName = argv[1];
        // command line argument (which is the filename here)
        UImanager->ApplyCommand(command+fileName);
        // tells the ui manager to use the settings described in the file arg
    }

    /*the code below is only run when the ui session is terminated this cleans
    up after we are done with our gui. Note that this should always run whether
    or not you're actually displaying the gui, just to ensure a graceful exit */
    delete ui;  // once the session is over, delete the ui, ...
    delete visManager; // ... the visualization manager, ...
    delete runManager; // ... and the run manager.

    return 0;
}
