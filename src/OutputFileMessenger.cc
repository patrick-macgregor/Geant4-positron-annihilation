#include "OutputFileMessenger.hh"
#include "RunAction.hh"
#include "G4UIcmdWithAString.hh"

////////////////////////////////////////////////////////////////////////////////
OutputFileMessenger::OutputFileMessenger( RunAction* run_action ) :
    m_run_action(run_action)
{
    G4String command = "/setOutputFileName";
    m_cmd_set_file_name = new G4UIcmdWithAString( command, this );
    m_cmd_set_file_name->SetParameterName("ouputFileName", false );
    m_cmd_set_file_name->SetGuidance("Output file name for a given detector geometry");
}
////////////////////////////////////////////////////////////////////////////////
OutputFileMessenger::~OutputFileMessenger()
{
    delete m_cmd_set_file_name;
}
////////////////////////////////////////////////////////////////////////////////
void OutputFileMessenger::SetNewValue( G4UIcommand* cmd, G4String value ){
    if ( cmd == m_cmd_set_file_name ){
        m_run_action->SetOutputFileName( value );
    }
    return;
}
