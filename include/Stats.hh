#ifndef STATS_HH_
#define STATS_HH_

#include "globals.hh"
#include <unordered_map>

class G4ParticleDefinition;

enum class STATS_INTS{
    INITAL_POSITRONS_THAT_LEAVE_WORLD,
    ANNIHILATION_MULT2_511_511,
    ANNIHILATION_MULT2_511_XXX,
    ANNIHILATION_MULT2_XXX_XXX,
    ANNIHILATION_MULT3_511_511_511,
    ANNIHILATION_MULT3_511_511_XXX,
    ANNIHILATION_MULT3_511_XXX_XXX,
    ANNIHILATION_MULT3_XXX_XXX_XXX,
    ANNIHILATION_MULT4_OR_MORE,
    N_511_FULLY_ABSORBED,
    N_511_PARTIALLY_ABSORBED,
    N_511_NOT_ABSORBED,
    N_511,
    N_NOT_511,
    COUNT
};


enum class STATS_MISC{
    PARTICLE_EXITING_WORLD
};


class Stats{
    public:
        Stats(){};
        ~Stats(){};

        // Getters
        G4int                                                  Get( const STATS_INTS s ) const;
        std::unordered_map<const G4ParticleDefinition*, G4int> Get( const STATS_MISC s ) const;

        // Add stats
        void AddStats( const Stats& stats );
        void Add( const STATS_INTS s, G4int value );
        void Add( const STATS_MISC s, const G4ParticleDefinition* def, G4int value );

        G4int GetInt( const STATS_INTS s ) const { return m_ints.at( static_cast<std::size_t>(s) ); }

        // Other methods
        void Print( std::ostream& os ) const;
        void Clear();

    private:
        static constexpr std::size_t N_INTS = static_cast<size_t>( STATS_INTS::COUNT );
        std::array<G4int, N_INTS> m_ints;

        // Misc run stats
        std::unordered_map<const G4ParticleDefinition*, G4int> m_particles_exiting_world_tally;
};

#endif
