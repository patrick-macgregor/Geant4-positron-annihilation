#ifndef TRACKING_ACTION_HH
#define TRACKING_ACTION_HH

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class EventAction;
class MyTrackingInfo;

// This calls functions at the start and end of every particle track - used for tracking parentage of particles
class TrackingAction : public G4UserTrackingAction
{
    public:
        TrackingAction( EventAction* event_action ) : m_event_action(event_action) {}
        ~TrackingAction(){}
        void PreUserTrackingAction( const G4Track* ) override;
        void PostUserTrackingAction( const G4Track*) override;

    private:
        EventAction* m_event_action;
};
#endif
