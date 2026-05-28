// Based on example B1 of Geant4 examples code
#include "ActionInitialisation.hh"
#include "DetectorConstruction.hh"
#include "QBBC.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4VUserPhysicsList.hh"


int main(int argc, char** argv)
{
    // Detect interactive mode (if no arguments) and define UI session
    G4UIExecutive* ui = nullptr;
    if (argc == 1){
        ui = new G4UIExecutive(argc, argv);
    }

    // use G4SteppingVerboseWithUnits
    G4int precision = 4;
    G4SteppingVerbose::UseBestUnit(precision);

    // Construct the default run manager - this SHOULD be auto!
    G4RunManager* run_manager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    // Set mandatory initialization classes
    // Detector construction
    run_manager->SetUserInitialization( new DetectorConstruction() );

    // Physics list
    G4VUserPhysicsList* physics_list = new QBBC;
    physics_list->SetVerboseLevel(0);
    run_manager->SetUserInitialization(physics_list);

    // User action initialization
    run_manager->SetUserInitialization( new ActionInitialisation() );

    // Initialize visualization with the default graphics system
    G4VisManager* vis_manager = new G4VisExecutive(argc, argv);
    // Constructors can also take optional arguments:
    // - a graphics system of choice, eg. "OGL"
    // - and a verbosity argument - see /vis/verbose guidance.
    // auto vis_manager = new G4VisExecutive(argc, argv, "OGL", "Quiet");
    // auto vis_manager = new G4VisExecutive("Quiet");
    vis_manager->Initialize();

    // Get the pointer to the User Interface manager
    G4UImanager* ui_manager = G4UImanager::GetUIpointer();

    // Process detector geometry file
    ui_manager->ApplyCommand("/control/execute detector_geometry.mac");

    // Process macro or start UI session
    if (!ui) {
        // batch mode
        G4String command = "/control/execute ";
        G4String file_name = argv[1];
        ui_manager->ApplyCommand(command + file_name);
    }
    else {
        // interactive mode
        ui_manager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }

    // Job termination
    // Free the store: user actions, physics_list and detector_description are
    // owned and deleted by the run manager, so they should not be deleted
    // in the main() program !

    delete vis_manager;
    delete run_manager;
    return 0;
}
