#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH 1

#include "G4String.hh"
#include "G4UserSteppingAction.hh"
#include "MyVerboseMessenger.hh"

class G4LogicalVolume;
class G4Step;
class EventAction;
class RunAction;

/// Stepping action class
class SteppingAction : public G4UserSteppingAction
{
    public:
        SteppingAction(EventAction* event_action, RunAction* run_action);
        ~SteppingAction();

        // method from the base class
        void UserSteppingAction(const G4Step*) override;

        // Methods for verbosity
        void SetVerboseFlag( const G4bool x ){ m_verbose = x; }
        G4bool GetVerboseFlag() const { return m_verbose; }
        static const G4String GetVerboseCommandName(){ return "steppingaction/verbose"; }

    private:
        EventAction* m_event_action = nullptr;
        RunAction* m_run_action = nullptr;
        G4LogicalVolume* m_scoring_volume = nullptr;

        G4bool m_verbose = false;
        MyVerboseMessenger<SteppingAction>* m_messenger;
};


#endif
