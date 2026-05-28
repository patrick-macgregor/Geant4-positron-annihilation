#ifndef TRACK_INFORMATION_HH_
#define TRACK_INFORMATION_HH_

#include "G4VUserTrackInformation.hh"
#include "G4ParticleDefinition.hh"

// Define a struct that holds information about parent process that created the child
struct MyTrackingInfo
{
    G4int positron_child_id;
    const G4ParticleDefinition* positron_child_particle_definition;
    bool is_511_gamma;

    MyTrackingInfo() : positron_child_id(-1), positron_child_particle_definition(nullptr), is_511_gamma(false) {}
    MyTrackingInfo( const G4int id, const G4ParticleDefinition* def, const bool x ) : positron_child_id(id), positron_child_particle_definition(def), is_511_gamma(x) {}
};

// Create a class that is able to track the information
class TrackInformation : public G4VUserTrackInformation
{
    public:
        TrackInformation() : m_info(MyTrackingInfo() ) {}
        TrackInformation( const MyTrackingInfo info ) : m_info(info) {}
        ~TrackInformation(){}

        // Getters
        inline const MyTrackingInfo GetTrackingInfo() const { return m_info; }

    private:
        const MyTrackingInfo m_info;
};
#endif
