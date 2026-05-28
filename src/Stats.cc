#include "Stats.hh"
#include "G4ParticleDefinition.hh"

////////////////////////////////////////////////////////////////////////////////
G4int Stats::Get( const STATS_INTS s ) const
{
    return m_ints[ static_cast<std::size_t>(s) ];
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_map<const G4ParticleDefinition*, G4int> Stats::Get( const STATS_MISC s ) const
{
    switch(s){
        case STATS_MISC::PARTICLE_EXITING_WORLD :
            return m_particles_exiting_world_tally;
    };
}

////////////////////////////////////////////////////////////////////////////////
void Stats::AddStats( const Stats& stats )
{
    // Add integer counters
    for ( std::size_t i = 0; i < N_INTS; ++i ){
        m_ints[i] += stats.Get( static_cast<STATS_INTS>(i) );
    }

    // Add other counters
    for ( const auto& [particle, count] : stats.Get( STATS_MISC::PARTICLE_EXITING_WORLD ) ){
        m_particles_exiting_world_tally[particle] += count;
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
void Stats::Add( const STATS_INTS s, G4int value )
{
    m_ints[static_cast<std::size_t>(s)] += value;
    return;

}

////////////////////////////////////////////////////////////////////////////////
void Stats::Add( const STATS_MISC s, const G4ParticleDefinition* def, G4int value )
{
    switch(s){
        case STATS_MISC::PARTICLE_EXITING_WORLD:
            m_particles_exiting_world_tally[def] += value;
            break;
    };
    return;
}

////////////////////////////////////////////////////////////////////////////////
void Stats::Clear()
{
    for ( std::size_t i = 0; i < N_INTS; ++i ){
        m_ints.at(i) = 0;
    }
    m_particles_exiting_world_tally.clear();
    return;
}

////////////////////////////////////////////////////////////////////////////////
void Stats::Print( std::ostream& os ) const {
    // TODO - change this to print proper table
    // Table table = Table(80, border type etc.)
    // table.SetTitle(X)
    // table.SetWidth(80)
    // table.AddRow(description, value)
    // table.Print()
    G4int width = 8;

    // Title
    os << "!!!! PATRICK'S STATISTICS !!!!" << G4endl;

    // Check how the annihilation went
    os << "Positron annihilations" << G4endl;
    os << "| # 511's | " << std::setw(width) << 0 << " | " << std::setw(width) << 1 << " | " << std::setw(width) << 2 << " | " << std::setw(width) << 3 << " |" << G4endl;
    os << "| MULT 2  | " << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT2_XXX_XXX )
           << " | "          << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT2_511_XXX )
           << " | "          << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT2_511_511 )
           << " | "          << std::setw(width) << 0 << " |" << G4endl;
    os << "| MULT 3  | " << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT3_XXX_XXX_XXX )
           << " | "          << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT3_511_XXX_XXX )
           << " | "          << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT3_511_511_XXX )
           << " | "          << std::setw(width) << GetInt( STATS_INTS::ANNIHILATION_MULT3_511_511_511 ) << " |" << G4endl;
    os << "MULT 4+ = " << GetInt( STATS_INTS::ANNIHILATION_MULT4_OR_MORE ) << G4endl;

    // List parameters
    os << "Initial positrons that leave world = " <<               GetInt( STATS_INTS::INITAL_POSITRONS_THAT_LEAVE_WORLD ) << G4endl;
    os << "Number of non-511 keV gammas that deposit energy = " << GetInt( STATS_INTS::N_NOT_511)                          << G4endl;
    os << "Number of 511 keV gammas = " <<                         GetInt( STATS_INTS::N_511 )                             << G4endl;
    os << "    > fully absorbed by LaBr's = " <<                   GetInt( STATS_INTS::N_511_FULLY_ABSORBED )              << G4endl;
    os << "    > partially absorbed by LaBr's = " <<               GetInt( STATS_INTS::N_511_PARTIALLY_ABSORBED )          << G4endl;
    os << "    > not absorbed by LaBr's = " <<                     GetInt( STATS_INTS::N_511_NOT_ABSORBED )                << G4endl;
    os << "CHECK: total 511 gammas add up : sum = " <<
         GetInt( STATS_INTS::N_511_FULLY_ABSORBED ) +
         GetInt( STATS_INTS::N_511_PARTIALLY_ABSORBED ) +
         GetInt( STATS_INTS::N_511_NOT_ABSORBED )
        << " v.s. " << GetInt( STATS_INTS::N_511) << G4endl;

    os << "CHECK: total annihilations + positrons leaving world = number emitted = " <<
        GetInt( STATS_INTS::INITAL_POSITRONS_THAT_LEAVE_WORLD ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT2_511_511 ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT2_511_XXX ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT2_XXX_XXX ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT3_511_511_511 ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT3_511_511_XXX ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT3_511_XXX_XXX ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT3_XXX_XXX_XXX ) +
        GetInt( STATS_INTS::ANNIHILATION_MULT4_OR_MORE) << G4endl;

    // Breakdown of particles leaving world
    os << "Particle leaving world breakdown:" << G4endl;
    for ( const auto& [def, num] : m_particles_exiting_world_tally ){
        os << "  > " << def->GetParticleName() << ": " << num << G4endl;
    }

    // Define efficiency parameters (which is what I'm interested in)
    G4int total_511s_annihilation = GetInt( STATS_INTS::ANNIHILATION_MULT2_511_511 );
    G4int full_absorptions = GetInt( STATS_INTS::N_511_FULLY_ABSORBED );
    os << "Number of annihilations with 2 511's = " << total_511s_annihilation << G4endl;
    os << "Number of full absorptions in LaBr's = " << full_absorptions << G4endl;
    os << "Therefore, efficiency of detector    = " << (double)full_absorptions/(double)total_511s_annihilation;
    return;
}

