#ifndef MY_MESSENGER_HH
#define MY_MESSENGER_HH

#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"

// This class turns on a verbose flag for printing diagnostic messages in a
// class's member functions. Note that the class MUST define:
// - static G4String GetVerboseCommandName();
// - void SetVerboseFlag();
template<class C>
class MyVerboseMessenger : public G4UImessenger
{
    public:
        MyVerboseMessenger( C* cls );
        ~MyVerboseMessenger();

        void SetNewValue( G4UIcommand* cmd, G4String value ) override;

    private:
        C* m_class;
        G4UIcmdWithABool* m_verbose_command;

};


////////////////////////////////////////////////////////////////////////////////
template<class C>
MyVerboseMessenger<C>::MyVerboseMessenger( C* cls ) :
    m_class(cls)
{
    G4String command_string = "/ptmsim/" + C::GetVerboseCommandName();
    m_verbose_command = new G4UIcmdWithABool(command_string, this);
    m_verbose_command->SetGuidance( "Enable terminal output" );
    m_verbose_command->SetParameterName( "verbose", false );
}

////////////////////////////////////////////////////////////////////////////////
template<class C>
MyVerboseMessenger<C>::~MyVerboseMessenger()
{
    delete m_verbose_command;
}

////////////////////////////////////////////////////////////////////////////////
template<class C>
void MyVerboseMessenger<C>::SetNewValue( G4UIcommand* cmd, G4String value )
{
    if ( cmd == m_verbose_command ){
        m_class->SetVerboseFlag( m_verbose_command->GetNewBoolValue(value) );
    }
    return;
}


#endif
