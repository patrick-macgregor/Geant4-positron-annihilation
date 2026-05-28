#ifndef OUTPUT_FILE_MESSENGER_HH
#define OUTPUT_FILE_MESSENGER_HH

#include "G4UImessenger.hh"

class G4UIcmdWithAString;
class RunAction;

// This class manages the name of the output file
class OutputFileMessenger : public G4UImessenger
{
    public:
        OutputFileMessenger( RunAction* run_action );
        ~OutputFileMessenger();

        void SetNewValue( G4UIcommand* cmd, G4String value ) override;

    private:
        G4UIcmdWithAString* m_cmd_set_file_name;
        RunAction* m_run_action;
};




#endif
