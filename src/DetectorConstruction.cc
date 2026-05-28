#include "DetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VisAttributes.hh"
#include "Randomize.hh"


////////////////////////////////////////////////////////////////////////////////
namespace DETECTOR
{
    ////////////////////////////////////////////////////////////////////////////
    Manager::Manager()
    {
        m_detector_messenger = new Messenger(this);
    }

    ////////////////////////////////////////////////////////////////////////////
    Manager::~Manager()
    {
        delete m_detector_messenger;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Note this does nothing if the detector already exists!
    void Manager::AddDetector( const G4String& name )
    {
        m_config_map.insert( { name, Config() } );
        m_config_map.at(name).name = name;
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    void Manager::SetHalfThickness( const G4String& name, G4double t )
    {
        G4cout << "DETECTOR::Manager::SetHalfThickness -> " << std::setw(10) << name << " = " << t << G4endl;
        AddDetector(name);
        m_config_map.at( name ).half_thickness = t;
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    void Manager::SetZ( const G4String& name, G4double z )
    {
        G4cout << "DETECTOR::Manager::SetZ             -> " << std::setw(10) << name << " = " << z << G4endl;
        AddDetector(name);
        m_config_map.at(name).z = z;
        return;
    }
    ////////////////////////////////////////////////////////////////////////////
    Config Manager::GetConfig( const G4String& name )
    {
      // NOTE THAT THIS WILL THROW AN ERROR IF THE CONFIG DOES NOT EXIST, WHICH
      // IS A GOOD THING
      return m_config_map.at(name);
    }
    ////////////////////////////////////////////////////////////////////////////
    void Manager::PrintConfig() const
    {
        G4cout << "PRINTING CONFIGS" << G4endl;
        for ( const auto& [key,config] : m_config_map )
        {
            config.Print();
        }
        return;
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    void Geometry::SetColour( G4Colour colour )
    {
        m_logical_volume->SetVisAttributes( new G4VisAttributes( colour ) );
        return;
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    void QQQ1Array::Construct( G4LogicalVolume* parent, const Config& config )
    {
        // Safety first
        if ( !parent ) return;

        // Store configuration
        m_config = config;
        config.Print();

        // Define material
        G4Material* material_Si = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

        // Define physical and logical volumes
        SetPhysicalVolume( new G4Tubs( "QQQ1", INNER_RADIUS, OUTER_RADIUS, m_config.half_thickness, 0.0, SECTOR_ANGLE ) );
        SetLogicalVolume(  new G4LogicalVolume( GetPhysicalVolume(), material_Si, "QQQ1") );

        // Begin placement by constructing rotation matrices
        G4RotationMatrix* rot[4] = {
            new G4RotationMatrix(),
            new G4RotationMatrix(),
            new G4RotationMatrix(),
            new G4RotationMatrix()
        };
        G4double qqq1_beam_axis_separation = INNER_RADIUS + BEAM_AXIS_SEPARATION;

        for ( G4int i = 0; i < 4; i++ ){
            G4double angle = PHI + 90.0*i*deg;
            rot[i]->rotateZ(angle);
            new G4PVPlacement( rot[i], G4ThreeVector(0,0,m_config.z), GetLogicalVolume(), "QQQ1", parent, false, i, true);
        }
        return;
    }

    ////////////////////////////////////////////////////////////////////////////
    G4ThreeVector QQQ1Array::GetRandomPoint()
    {
        // Generate uniform phi and correctly scaled uniform r so we have equal probabilities of being anywhere inside detector
        G4double r = std::sqrt( INNER_RADIUS*INNER_RADIUS + G4UniformRand()*( OUTER_RADIUS*OUTER_RADIUS - INNER_RADIUS*INNER_RADIUS ) );
        G4double phi_sector = G4UniformRand()*SECTOR_ANGLE;
        G4double z = m_config.z + G4UniformRand()*2*m_config.half_thickness;
        G4double sector = (int)( G4UniformRand()*4 );
        G4double phi = phi_sector + 90.0*sector + PHI;
        return G4ThreeVector( r*cos(phi), r*sin(phi), z );
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    void LaBrDetectorArray::Construct( G4LogicalVolume* parent, const Config& config )
    {
        // Safety first
        if (!parent) return;

        // Set config
        m_config = config;

        // Construct material
        G4Element* element_La = G4NistManager::Instance()->FindOrBuildElement( 57 );
        G4Element* element_Br = G4NistManager::Instance()->FindOrBuildElement( 35 );
        G4Material* material_LaBr = new G4Material( "LaBr3", 5.06*g/cm3, 2, G4State::kStateSolid );
        material_LaBr->AddElement( element_La, 1 );
        material_LaBr->AddElement( element_Br, 3 );

        // Construct physical and logical volumes
        SetPhysicalVolume( new G4Tubs( "LaBr3Detector", 0.0, RADIUS, LENGTH*0.5, 0.0, 360.0*deg ) );
        SetLogicalVolume( new G4LogicalVolume( GetPhysicalVolume(), material_LaBr, "LaBr3Detector" ) );

        // Place detectors
        G4double cos45 = std::sqrt( 2.0 )/2.0;
        G4double sep = RADIUS*( std::sqrt(2.0) - 1 ) + SEPARATION;

        G4ThreeVector positions[4] = {
            G4ThreeVector(  cos45*( RADIUS + sep ),  cos45*( RADIUS + sep ), 0.5*LENGTH + m_config.z ),
            G4ThreeVector( -cos45*( RADIUS + sep ),  cos45*( RADIUS + sep ), 0.5*LENGTH + m_config.z ),
            G4ThreeVector(  cos45*( RADIUS + sep ), -cos45*( RADIUS + sep ), 0.5*LENGTH + m_config.z ),
            G4ThreeVector( -cos45*( RADIUS + sep ), -cos45*( RADIUS + sep ), 0.5*LENGTH + m_config.z ),
        };

        // Set colour
        SetColour( G4Colour( 0.3, 0.3, 1.0 ) );

        for (G4int i = 0; i < 4; i++)
        {
            new G4PVPlacement( 0, positions[i], GetLogicalVolume(), "LaBr3Detector", parent, false, i, true );
        }


        return;
    }
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    void StopperPlate::Construct( G4LogicalVolume* parent, const Config& config )
    {
        // Safety first
        if ( !parent ) return;

        // Set config
        m_config = config;

        // Make material
        G4Material* mat_plate = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

        // Construct physical and logical volumes
        SetPhysicalVolume( new G4Tubs( "StopperPlate", INNER_RADIUS, OUTER_RADIUS, m_config.half_thickness, 0.0, 360.0*deg ) );
        SetLogicalVolume( new G4LogicalVolume( GetPhysicalVolume(), mat_plate, "StopperPlate" ) );

        // Set colour
        SetColour( G4Colour( 1.0, 0.3, 0.3 ) );

        // Place in 3D space
        new G4PVPlacement( 0, G4ThreeVector(0,0,m_config.z), GetLogicalVolume(), "StopperPlate", parent, false, 0, true );
        return;
    }

} // namespace DETECTORS


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DetectorConstruction::DetectorConstruction()
{
    m_detector_manager = new DETECTOR::Manager();
}

////////////////////////////////////////////////////////////////////////////////
DetectorConstruction::~DetectorConstruction()
{
    delete m_detector_manager;
}

////////////////////////////////////////////////////////////////////////////////
void DetectorConstruction::ConstructWorld()
{
    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* world_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

    // Construct physical and logical volumes
    m_world_physical_volume = new G4Box("World", 0.5*WORLD_SIZE_X, 0.5*WORLD_SIZE_Y, 0.5*WORLD_SIZE_Z );
    m_world_logical_volume= new G4LogicalVolume( m_world_physical_volume, world_mat, "World");

    // Place world volume
    m_world_placement = new G4PVPlacement(
        nullptr,                    // no rotation
        G4ThreeVector(),            // at (0,0,0)
        m_world_logical_volume,     // its logical volume
        "World",                    // its name
        nullptr,                    // its mother volume
        false,                      // no boolean operation
        0,                          // copy number
        true                        // overlaps checking
    );
    return;
}

////////////////////////////////////////////////////////////////////////////////
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Construct world volume first - always needed first
    ConstructWorld();

    // Print the configuration from the input file - highlights errors here!
    m_detector_manager->PrintConfig();

    // Construct the different detectors
    DETECTOR::Geometry* labr_array       = DETECTOR::Geometry::Create<DETECTOR::LaBrDetectorArray>( m_world_logical_volume, GetConfig( "LaBr" ) );
    G4cout << "LaBr OK" << G4endl;
    DETECTOR::Geometry* qqq1_array_E     = DETECTOR::Geometry::Create<DETECTOR::QQQ1Array>( m_world_logical_volume, GetConfig( "RecoilE" ) );
    G4cout << "RecoilE OK" << G4endl;
    DETECTOR::Geometry* qqq1_array_dE    = DETECTOR::Geometry::Create<DETECTOR::QQQ1Array>( m_world_logical_volume, GetConfig( "RecoilDE" ) );
    G4cout << "RecoilDE OK" << G4endl;
    // DETECTOR::Geometry* stopper_plate    = DETECTOR::Geometry::Create<DETECTOR::StopperPlate>( m_world_logical_volume, GetConfig( "Stopper" ) );
    // G4cout << "Stopper OK" << G4endl;

    // Tell the code that we're measuring the energy deposited in the LaBr's
    m_scoring_volume = labr_array->GetLogicalVolume();

    // Tell the code to place positrons randomly in the QQQ1 E detector
    m_particle_gun_origin_detector = qqq1_array_E;

    // ALWAYS return world placement
    return m_world_placement;
}
