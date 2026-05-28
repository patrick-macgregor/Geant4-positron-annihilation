#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "MyVerboseMessenger.hh"
#include "RunAction.hh"
#include "Stats.hh"
#include "TrackInformation.hh"

#include "G4Gamma.hh"
#include "G4LogicalVolume.hh"
#include "G4Positron.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"
#include <G4TrackStatus.hh>

////////////////////////////////////////////////////////////////////////////////
SteppingAction::SteppingAction(EventAction* event_action, RunAction* run_action) :
    m_event_action(event_action),
    m_run_action(run_action)
{
    m_messenger = new MyVerboseMessenger<SteppingAction>(this);
}

////////////////////////////////////////////////////////////////////////////////
SteppingAction::~SteppingAction()
{
    delete m_messenger;
}

////////////////////////////////////////////////////////////////////////////////
void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // store scoring volume pointer if not already obtained
    if (!m_scoring_volume) {
        const auto detConstruction = static_cast<const DetectorConstruction*>( G4RunManager::GetRunManager()->GetUserDetectorConstruction() );
        m_scoring_volume = detConstruction->GetScoringVolume();
    }

    // get volume of the current step
    G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    G4Track* track = step->GetTrack();

    if ( m_verbose ){
        G4cout << "  >> " << track->GetTrackID() << "[" << track->GetParentID() << "] = " << track->GetParticleDefinition()->GetParticleName() << "  " << track->GetVertexKineticEnergy() << G4endl;
    }

    // get energy deposited in detectors
    G4double energy_deposited_in_step = 0.0;
    if ( volume == m_scoring_volume ){
        energy_deposited_in_step = step->GetTotalEnergyDeposit();
    }

    // regardless of energy, store track information for EVERY particle, storing parent ID if possible
    // note that the parent ID is stored iff it's a child of the original positron
    TrackInformation* info = dynamic_cast<TrackInformation*>( track->GetUserInformation() );
    if (!info ){
        m_event_action->AddEdep( track->GetTrackID(), energy_deposited_in_step );
    }
    else{
        m_event_action->AddEdep( info->GetTrackingInfo().positron_child_id, energy_deposited_in_step );
    }

    // check if we've hit the world boundary
    G4StepPoint* pre = step->GetPreStepPoint();
    G4StepPoint* post = step->GetPostStepPoint();
    bool is_original_positron = ( track->GetParentID() == 0 && track->GetTrackID() == 1 && track->GetDefinition() == G4Positron::Positron() );

    // Deal with particles leaving world volume
    if ( post->GetStepStatus() == fWorldBoundary ){
        // A particle has left the world - register this fact!
        m_event_action->AddStats( STATS_MISC::PARTICLE_EXITING_WORLD, track->GetDefinition(), 1 );

        // Check if the particle leaving the world is the ORIGINAL positron fired from the particle gun
        if ( is_original_positron ){
            m_event_action->AddStats( STATS_INTS::INITAL_POSITRONS_THAT_LEAVE_WORLD, 1 );
        }

        // Record angles if it's the original positron
        if ( is_original_positron ){
            G4ThreeVector momentum_direction = pre->GetMomentumDirection();
            G4double theta_deg = momentum_direction.theta()/deg;
            G4double phi_deg = momentum_direction.phi()/deg;
            m_event_action->StoreAnglesOfEscapingPositron(theta_deg, phi_deg);
        }
    }

    // Get secondaries created in this step and work out whether they're from the annihilation of the positron
    const G4VProcess* process = post->GetProcessDefinedStep(); // positron about to decay
    G4TrackStatus status = track->GetTrackStatus();

    if ( is_original_positron && process && process->GetProcessName() == "annihil" && status == fStopAndKill ){
        // We're annihilating - start looking at secondaries
        const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
        G4int sum_511s = 0;
        G4int sum_non_511s = 0;

        for (const G4Track* sec_track : *secondaries) {
            // Check: is it a gamma?
            if (sec_track->GetDefinition() != G4Gamma::Gamma()) continue;

            // Check: is the kinetic energy of the gamma ~511 keV?
            G4double energy = sec_track->GetKineticEnergy();
            bool is_511_gamma = std::abs( energy - CLHEP::electron_mass_c2 ) < 1.0 * keV;

            if ( is_511_gamma ) sum_511s++;
            else sum_non_511s++;
        }

        // Now that we know how many gammas there are, we can distinguish between decays
        STATS_INTS s;

        // MULTIPLICITY = 2
        if ( sum_511s + sum_non_511s == 2 ){
            if ( sum_511s == 2 && sum_non_511s == 0 ){ s = STATS_INTS::ANNIHILATION_MULT2_511_511; }
            else if ( sum_511s == 0 && sum_non_511s == 2 ){ s = STATS_INTS::ANNIHILATION_MULT2_XXX_XXX; }
            else{ s = STATS_INTS::ANNIHILATION_MULT2_511_XXX; }
        }
        // MULTIPLICITY == 3
        else if ( sum_511s + sum_non_511s == 3 ){
            if ( sum_511s == 2 && sum_non_511s == 1 ){ s = STATS_INTS::ANNIHILATION_MULT3_511_511_XXX; }
            else if ( sum_511s == 1 && sum_non_511s == 2 ){ s = STATS_INTS::ANNIHILATION_MULT3_511_XXX_XXX; }
            else if ( sum_511s == 0 && sum_non_511s == 3 ){ s = STATS_INTS::ANNIHILATION_MULT3_XXX_XXX_XXX; }
            else{ s = STATS_INTS::ANNIHILATION_MULT3_511_511_511; }
        }
        else { s = STATS_INTS::ANNIHILATION_MULT4_OR_MORE; }

        // Now add the statistics
        m_event_action->AddStats( STATS_INTS::N_511, sum_511s );
        m_event_action->AddStats( s, 1 );
    }

}

