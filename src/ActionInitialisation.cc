#include "ActionInitialisation.hh"

#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"

////////////////////////////////////////////////////////////////////////////////
void ActionInitialisation::BuildForMaster() const
{
    RunAction* run_action = new RunAction;
    SetUserAction(run_action);
}

////////////////////////////////////////////////////////////////////////////////
void ActionInitialisation::Build() const
{
    SetUserAction(new PrimaryGeneratorAction);

    RunAction* run_action = new RunAction;
    SetUserAction( run_action );

    EventAction* event_action = new EventAction( run_action );
    SetUserAction( event_action );

    SetUserAction( new SteppingAction( event_action, run_action ) );
    SetUserAction( new TrackingAction( event_action ) );
}
