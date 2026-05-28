#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH 1

#include <G4ParticleDefinition.hh>
#include "G4UserEventAction.hh"
#include "Stats.hh"

class G4Event;
class G4ParticleDefinition;
class MyTrackingInfo;
class RunAction;

// Class that is called at the beginning and end of every event (i.e. one positron annihilation)
class EventAction : public G4UserEventAction
{
    public:
        EventAction(RunAction* runAction);
        ~EventAction() override = default;

        void BeginOfEventAction(const G4Event* event) override;
        void EndOfEventAction(const G4Event* event) override;

        void AddEdep(G4int parent_particle_id, G4double edep) { m_energy_deposited[parent_particle_id] += edep; }
        void StoreAnglesOfEscapingPositron( const G4double theta, const G4double phi );
        void AddStats( const STATS_INTS s, G4int value ){ m_event_stats.Add( s, value ); }
        void AddStats( const STATS_MISC s, const G4ParticleDefinition* def, G4int value ){ m_event_stats.Add( s, def, value ); }
        void AddTrackInfo( G4int id, const MyTrackingInfo info );
        static void PrintProgressBar( int count );


    private:
        RunAction* fRunAction;
        std::map<G4int,G4double> m_energy_deposited;
        Stats m_event_stats;
        std::unordered_map<G4int, MyTrackingInfo> m_map_id_info;
};

#endif
