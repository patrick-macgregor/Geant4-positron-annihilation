#ifndef HISTOGRAM_IDS_HH
#define HISTOGRAM_IDS_HH

// Define histogram IDs for easier filling later
// Last element is a sentinel for keeping track of how many histograms are defined
namespace HISTOGRAM_IDS{
    enum H1
    {
        GAMMA_ENERGY_IN_LABRS,
        ESCAPED_POSITRON_THETA,
        ESCAPED_POSITRON_PHI,
        kH1Count
    };
    enum H2
    {
        ESCAPED_POSITRON_THETA_PHI,
        kH2Count
    };
}
#endif

