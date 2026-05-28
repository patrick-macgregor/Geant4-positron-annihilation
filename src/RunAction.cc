#include "RunAction.hh"

#include "DetectorConstruction.hh"
#include "HistogramIDs.hh"
#include "OutputFileMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "Run.hh"

#include "G4AccumulableManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <G4AnalysisManager.hh>

////////////////////////////////////////////////////////////////////////////////
RunAction::RunAction()
{
    // Create output file messenger
    m_output_file_messenger = new OutputFileMessenger( this );
    m_output_file_name = "positron_annihilation_13B_DH_results";

    // Get analysis manager
    G4AnalysisManager* analysis_manager = G4AnalysisManager::Instance();
    analysis_manager->SetDefaultFileType("root");

    // Use asserts to make sure everything is working as expected
    analysis_manager->CreateH1( "energy", "Ideal gamma spectrum", 4000, 0.0, 4.*MeV );
    assert( analysis_manager->GetH1Id( "energy" ) == HISTOGRAM_IDS::H1::GAMMA_ENERGY_IN_LABRS );

    analysis_manager->CreateH1( "theta", "Positron emission angle (#theta)", 180, 0.0, 180.0 );
    assert( analysis_manager->GetH1Id( "theta" ) == HISTOGRAM_IDS::H1::ESCAPED_POSITRON_THETA );

    analysis_manager->CreateH1( "phi", "Positron emission angle (#phi)", 360, -180.0, 180.0 );
    assert( analysis_manager->GetH1Id( "phi" ) == HISTOGRAM_IDS::H1::ESCAPED_POSITRON_PHI );

    analysis_manager->CreateH2( "theta_phi", "Positron emission angles (#theta,#phi)", 180, 0.0, 180.0, 360, -180.0, 180.0 );
    assert( analysis_manager->GetH2Id( "theta_phi" ) == HISTOGRAM_IDS::H2::ESCAPED_POSITRON_THETA_PHI );
}

////////////////////////////////////////////////////////////////////////////////
G4Run* RunAction::GenerateRun()
{
    return new Run();
}


////////////////////////////////////////////////////////////////////////////////
void RunAction::BeginOfRunAction(const G4Run*)
{
    // Create output file and histograms
    G4AnalysisManager* analysis_manager = G4AnalysisManager::Instance();
    analysis_manager->Reset();
    analysis_manager->OpenFile( m_output_file_name + ".root" );

    // inform the runManager to save random number seed
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);

    // reset accumulables to their initial values
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Reset();

}

////////////////////////////////////////////////////////////////////////////////
void RunAction::EndOfRunAction(const G4Run* run)
{
    G4int nofEvents = run->GetNumberOfEvent();
    if (nofEvents == 0) return;

    // Cast to Run object
    const Run* myrun = static_cast<const Run*>(run);

    // Run conditions
    // note: There is no primary generator action object for "master"
    //       run manager for multi-threaded mode.
    const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
            G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

    G4String runCondition;
    if (generatorAction) {
        const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
        runCondition += particleGun->GetParticleDefinition()->GetParticleName();
        runCondition += " of ";
        G4double particleEnergy = particleGun->GetParticleEnergy();
        runCondition += G4BestUnit(particleEnergy, "Energy");
    }

    // Print
    if (IsMaster()) {
        G4cout << G4endl << "--------------------End of Global Run-----------------------" << G4endl;
        // Print stats to console and to file
        myrun->PrintStats(G4cout);
        std::ofstream myfile;
        myfile.open( m_output_file_name + ".txt");
        if ( myfile.is_open() ){
            myrun->PrintStats(myfile);
            myfile.close();
        }
        else{
            G4cout << "COULD NOT PRINT RESULTS TO FILE." << G4endl;
        }
    }
    else {
        G4cout << G4endl << "--------------------End of Local Run------------------------";
    }

    G4cout << G4endl << " The run is " << nofEvents << " " << runCondition << G4endl << G4endl;

    // Write histograms and close file
    G4AnalysisManager* analysis_manager = G4AnalysisManager::Instance();
    analysis_manager->Write();
    analysis_manager->CloseFile(false);
    return;
}
