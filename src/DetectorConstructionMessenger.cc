#include "DetectorConstructionMessenger.hh"

#include "DetectorConstruction.hh"
#include <G4UIcommand.hh>
#include <G4UIdirectory.hh>
#include <G4UIparameter.hh>

namespace DETECTOR
{
    ////////////////////////////////////////////////////////////////////////////////
    Messenger::Messenger( Manager* manager ) :
        m_detector_manager(manager)
    {
        // Create custom directory for detector geometry
        m_cmd_directory = new G4UIdirectory("/det/");
        m_cmd_directory->SetGuidance("Detector controls");

        // Define half-thickness command
        G4String thickness_cmd_string = "/det/setHalfThickness";
        m_cmd_change_thickness = new G4UIcommand( thickness_cmd_string, this );
        G4UIparameter* ht_det_name = new G4UIparameter( "detName", 's', false );
        G4UIparameter* ht_half_thickness = new G4UIparameter( "half_thickness", 'd', false );
        G4UIparameter* ht_unit = new G4UIparameter( "unit", 's', false );
        m_cmd_change_thickness->SetParameter( ht_det_name );
        m_cmd_change_thickness->SetParameter( ht_half_thickness );
        m_cmd_change_thickness->SetParameter( ht_unit );
        m_cmd_change_thickness->SetGuidance( "Change half-thickness of detector: detectorName half_thickness unit" );

        // Define z command
        G4String z_cmd_string = "/det/setZ";
        m_cmd_change_z = new G4UIcommand( z_cmd_string, this );
        G4UIparameter* z_det_name = new G4UIparameter( "detName", 's', false );
        G4UIparameter* z_half_thickness = new G4UIparameter( "half_thickness", 'd', false );
        G4UIparameter* z_unit = new G4UIparameter( "unit", 's', false );
        m_cmd_change_z->SetParameter( z_det_name );
        m_cmd_change_z->SetParameter( z_half_thickness );
        m_cmd_change_z->SetParameter( z_unit );
        m_cmd_change_z->SetGuidance( "Change z-position of detector: detectorName z unit" );

    }

    ////////////////////////////////////////////////////////////////////////////////
    Messenger::~Messenger()
    {
        delete m_cmd_change_thickness;
        delete m_cmd_change_z;
    }

    ////////////////////////////////////////////////////////////////////////////////
    void Messenger::SetNewValue( G4UIcommand* cmd, G4String value )
    {
        // Check it's for us
        if ( cmd != m_cmd_change_thickness && cmd != m_cmd_change_z ) return;

        // Parse command
        std::stringstream ss(value);
        G4String name;
        G4double myvalue;
        G4String unit;
        ss >> name >> myvalue >> unit;

        if ( cmd == m_cmd_change_thickness )
        {
            m_detector_manager->SetHalfThickness(name, myvalue*G4UIcommand::ValueOf(unit) );
        }
        else if ( cmd == m_cmd_change_z )
        {
            m_detector_manager->SetZ( name, myvalue*G4UIcommand::ValueOf(unit) );
        }
        return;
    }
}
