#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH 1

#include "G4UserRunAction.hh"

class G4Run;
class G4ParticleDefinition;

// Run action class - called at the start and end of every run
class RunAction : public G4UserRunAction
{
    public:
        RunAction();
        ~RunAction() override = default;

        G4Run* GenerateRun() override;
        void BeginOfRunAction(const G4Run*) override;
        void EndOfRunAction(const G4Run*) override;
};

#endif
