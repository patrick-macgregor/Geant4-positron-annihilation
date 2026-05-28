#include <G4Log.hh>
#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class DetectorConstructionMessenger;
class G4Colour;
class G4LogicalVolume;
class G4Tubs;
class G4VPhysicalVolume;
class G4PVPlacement;
class G4VSolid;

namespace DETECTOR
{
    class Messenger;
    ////////////////////////////////////////////////////////////////////////////
    // These parameters are specified by the macros!
    struct Config
    {
        G4String name;
        G4double half_thickness;
        G4double z;
        inline void Print() const {
            G4cout << "CONFIG: " << name << ", half-thickness = " << half_thickness << ", z = " << z << G4endl;
        }
    };

   ////////////////////////////////////////////////////////////////////////////
    // This helps map the macro commands to each detector
    class Manager
    {
        public:
            Manager();
            ~Manager();

            void AddDetector( const G4String& name );
            void SetHalfThickness( const G4String& name, const G4double t );
            void SetZ( const G4String& name, const G4double z );
            Config GetConfig( const G4String& name );
            void PrintConfig() const;

        private:
            std::map<G4String,Config> m_config_map;
            Messenger* m_detector_messenger;
    };

    ////////////////////////////////////////////////////////////////////////////
    // The detector class is primarily responsible for handling the different
    // materials and geometries that are needed.
    class Geometry{
        public:
            // No base constructor, default destructor
            virtual ~Geometry() = default;

            // Getters
            inline G4LogicalVolume* GetLogicalVolume(){ return m_logical_volume; }
            inline G4VSolid* GetPhysicalVolume(){ return m_physical_volume; }

            // Setters
            // inline void SetZ( const G4double z ){ m_z = z; }
            // inline void SetHalfThickness( const G4double t ){ m_half_thickness = t; }

            // Create detectors with this function!
            template<typename T, typename... Args>
            static DETECTOR::Geometry* Create(G4LogicalVolume* parent, const Config& config, Args&&... args)
            {
                // Check the function call is valid - derives from Geometry
                static_assert(std::is_base_of_v<Geometry, T>, "T must derive from Geometry");

                // Make the unique pointer
                Geometry* det = new T( std::forward<Args>(args)... );

                // Create the object using the (overridden) construct object
                det->Construct(parent, config);          // geometry built once - this must be defined for every derived class!
                return det;
            }

            // Get a random point inside the detector
            virtual G4ThreeVector GetRandomPoint(){ return G4ThreeVector(0,0,0); }


        // These are not public but are exposed to the derived class
        protected:
            // Prevent direct construction
            Geometry() = default;

            // Helper setters for derived classes
            inline void SetLogicalVolume( G4LogicalVolume* log_vol ){ m_logical_volume = log_vol; }
            inline void SetPhysicalVolume( G4VSolid* phys_vol ){ m_physical_volume = phys_vol; }
            void SetColour( G4Colour colour, G4LogicalVolume* log_vol = nullptr );

            // Members - access in derived classes as private members
            G4LogicalVolume* m_logical_volume;
            G4VSolid* m_physical_volume;
            Config m_config;
            // Getters
            // G4double GetZ() const { return m_z; }
            // G4double GetHalfThickness() const { return m_half_thickness; }


        // These are never exposed to the derived class (unless overridden)
        private:
            // This method must be overwritten by every derived class
            virtual void Construct( G4LogicalVolume* parent, const Config& config) = 0;

            // G4double m_z;
            // G4double m_half_thickness;
    };
    ////////////////////////////////////////////////////////////////////////////
    class QQQ1Array : public Geometry
    {
        public:
            QQQ1Array() = default;
            ~QQQ1Array() = default;
            static constexpr G4double INNER_RADIUS = 0.5*11.5*mm;
            static constexpr G4double OUTER_RADIUS = 0.5*103*mm;
            static constexpr G4double SECTOR_ANGLE = 82*deg;
            static constexpr G4double BEAM_AXIS_SEPARATION = 1.0*mm;
            static constexpr G4double PHI = -4.0*deg;

            G4ThreeVector GetRandomPoint() override;

        private:
            void Construct( G4LogicalVolume* parent, const Config& config ) override;
    };
    ////////////////////////////////////////////////////////////////////////////
    class LaBrDetectorArray : public Geometry
    {
        public:
            LaBrDetectorArray() = default;
            ~LaBrDetectorArray() = default;
            static constexpr G4double RADIUS = 1.075*2.54*cm;
            static constexpr G4double LENGTH = 3*2.54*cm;
            static constexpr G4double SEPARATION = 1.65*2.54*cm - RADIUS;
            static constexpr G4double FRAME_RADIUS = 6.2*0.5*2.54*cm;
            static constexpr G4double FRAME_HALF_THICKNESS = 0.889*cm;

        private:
            void Construct( G4LogicalVolume* parent, const Config& config ) override;

    };
    ////////////////////////////////////////////////////////////////////////////
    class StopperPlate : public Geometry
    {
        public:
            StopperPlate() = default;
            ~StopperPlate() = default;
            static constexpr G4double INNER_RADIUS = 0.0*mm;
            static constexpr G4double OUTER_RADIUS = 6.2*0.5*2.54*cm;

        private:
            void Construct( G4LogicalVolume* parent, const Config& config ) override;
    };
}

/// Detector construction class to define materials and geometry.
class DetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        DetectorConstruction();
        ~DetectorConstruction();

        static constexpr G4double WORLD_SIZE_X = 20*cm;
        static constexpr G4double WORLD_SIZE_Y = 20*cm;
        static constexpr G4double WORLD_SIZE_Z = 30*cm;

        G4VPhysicalVolume* Construct() override;

        DETECTOR::Geometry* GetParticleGunOriginDetector() const { return m_particle_gun_origin_detector; }
        G4LogicalVolume* GetScoringVolume() const { return m_scoring_volume; }

    protected:
        G4LogicalVolume* m_scoring_volume = nullptr;

    private:
        void ConstructWorld();
        DETECTOR::Config GetConfig( const G4String& name ) const { return m_detector_manager->GetConfig( name ); }
        DETECTOR::Manager* m_detector_manager;
        DETECTOR::Geometry* m_particle_gun_origin_detector;
        G4VSolid* m_world_physical_volume;
        G4LogicalVolume* m_world_logical_volume;
        G4PVPlacement* m_world_placement;
};

#endif
