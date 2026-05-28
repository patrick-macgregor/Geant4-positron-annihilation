#include "TrackingAction.hh"

#include "EventAction.hh"
#include "TrackInformation.hh"

#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include <CLHEP/Units/PhysicalConstants.h>
#include <G4ParticleDefinition.hh>

////////////////////////////////////////////////////////////////////////////////
void TrackingAction::PreUserTrackingAction( const G4Track* track ){
    // Register ID if primary annihilation gamma
    // Track ID 1 corresponds to positron (which has parent ID = 0)
    // All annihilation gammas therefore have track parent ID of 1
    if (
        track->GetParentID() == 1// i.e. parent is original positron
    ){
        // Work out track properties
        const G4ParticleDefinition* def = track->GetParticleDefinition();
        bool is_511_gamma = ( std::abs( track->GetKineticEnergy() - CLHEP::electron_mass_c2 ) < 1.0*keV ) && def == G4Gamma::Gamma();
        MyTrackingInfo my_tracking_info = MyTrackingInfo( track->GetTrackID(), track->GetParticleDefinition(), is_511_gamma );

        // Store track properties
        TrackInformation* track_info = new TrackInformation( my_tracking_info );
        const_cast<G4Track*>(track)->SetUserInformation(track_info);
    }
}

////////////////////////////////////////////////////////////////////////////////
void TrackingAction::PostUserTrackingAction( const G4Track* track ){
    // First store info in the map for each track
    TrackInformation* track_information = dynamic_cast<TrackInformation*>( track->GetUserInformation() );
    if ( !track_information ) return;
    m_event_action->AddTrackInfo( track->GetTrackID(), track_information->GetTrackingInfo() );

    // Try to get secondaries
    G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
    if (!secondaries) return;

    // Set parent gamma ID for all secondaries
    for ( G4Track* sec : *secondaries ){
        TrackInformation* info = new TrackInformation( track_information->GetTrackingInfo() );
        sec->SetUserInformation(info);
    }
}
