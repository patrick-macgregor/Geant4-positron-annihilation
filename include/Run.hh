#ifndef RUN_HH
#define RUN_HH 1

#include "G4Run.hh"
#include "Stats.hh"

class G4ParticleDefinition;

class Run : public G4Run {
    public:
        Run() = default;
        ~Run() = default;

        // Define merge command for merging results from multiple threads
        void Merge( const G4Run* run ) override;

        // Getters
        Stats& GetAllStats() const { return m_run_stats; }
        G4int GetStats( const STATS_INTS s ) const{ return m_run_stats.Get(s); }
        std::unordered_map<const G4ParticleDefinition*, G4int> GetStats( const STATS_MISC s ) const { return m_run_stats.Get(s); }

        // Setters
        void AddStats( const Stats& stats ) const { m_run_stats.AddStats( stats ); }
        void Add( const STATS_INTS s, G4int value = 1 ) const { m_run_stats.Add( s, value); }
        void Add( const STATS_MISC s, const G4ParticleDefinition* def, G4int value ) const { m_run_stats.Add( s, def, value ); }

        // Printer
        void PrintStats( std::ostream& os = G4cout ) const { m_run_stats.Print(os); }

    private:
        // Need this to be mutable so that it can be edited in const functions. Does not change state of program, but just keeps track of things so this is OK
        mutable Stats m_run_stats;

};


#endif
