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
#include <G4UIcommandStatus.hh>

void usage(){
               ////////////////////////////////////////////////////////////
    G4cout << "usage: positron_annihilation_13B_DH det_geom_macro.mac [run.mac]" << G4endl << G4endl;
    G4cout << "ESSENTIAL:" << G4endl;
    G4cout << "  det_geom_macro.mac  a macro file defining the properties" << G4endl;
    G4cout << "                      of all detectors in the system. This" << G4endl;
    G4cout << "                      program will crash if this full information" << G4endl;
    G4cout << "                      is not supplied!" << G4endl;
    G4cout << "OPTIONS: " << G4endl;
    G4cout << "  [run.mac]  Specifies the running conditions. If not supplied," << G4endl;
    G4cout << "             a visualiser window will open." << G4endl;
}

// USAGE: arg 1 = detector geometry
//        arg 2 = macro file (optional)
int main(int argc, char** argv)
{
    // If no detector geometry supplied - stop
    if ( argc == 1 ){
        usage();
        return 0;
    }
    // Detect interactive mode (if no arguments) and define UI session
    G4UIExecutive* ui = nullptr;
    if (argc == 2){
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
    G4String execute_command = "/control/execute ";
    G4String det_geom_file_name = argv[1];
    G4int det_geom_state = ui_manager->ApplyCommand( execute_command + det_geom_file_name );

    if ( det_geom_state != fCommandSucceeded ){
        G4cout << "Could not initialise detector geometry. Exiting..." << G4endl;
        return 1;
    }

    // Process macro or start UI session
    if (!ui) {
        // batch mode
        G4String file_name = argv[2];
        ui_manager->ApplyCommand(execute_command + file_name);
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
