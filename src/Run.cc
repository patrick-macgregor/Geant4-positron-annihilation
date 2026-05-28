#include "Run.hh"
#include <G4AccumulableManager.hh>

////////////////////////////////////////////////////////////////////////////////
void Run::Merge( const G4Run* run )
{
    // Cast to a Run object
    const Run* local_run = static_cast<const Run*>( run );

    // Merge the stats from the local run
    AddStats( local_run->GetAllStats() );

    // Merge the runs with the base class
    G4Run::Merge( run );

    return;
}
