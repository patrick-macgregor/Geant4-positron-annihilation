#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class G4ParticleDefinition;
class OutputFileMessenger;

// Run action class - called at the start and end of every run
class RunAction : public G4UserRunAction
{
    public:
        RunAction();
        ~RunAction() override = default;

        G4Run* GenerateRun() override;
        void BeginOfRunAction(const G4Run*) override;
        void EndOfRunAction(const G4Run*) override;

        void SetOutputFileName( const G4String value ){ m_output_file_name = value; }
        G4String GetOutputFileName() const { return m_output_file_name; }

    private:
        OutputFileMessenger* m_output_file_messenger;
        G4String m_output_file_name;

};

#endif
