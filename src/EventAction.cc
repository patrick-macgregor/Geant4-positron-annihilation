//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file EventAction.cc
/// \brief Implementation of the B1::EventAction class

#include "EventAction.hh"
#include "HistogramIDs.hh"
#include "RunAction.hh"
#include "Run.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "Stats.hh"
#include "TrackInformation.hh"
#include <CLHEP/Units/PhysicalConstants.h>
#include <G4ParticleDefinition.hh>
#include <G4Positron.hh>

#include <G4Run.hh>
#include <atomic>

////////////////////////////////////////////////////////////////////////////////
EventAction::EventAction(RunAction* runAction) :
    fRunAction(runAction)
{}

////////////////////////////////////////////////////////////////////////////////
void EventAction::BeginOfEventAction(const G4Event* event)
{
    m_energy_deposited.clear();
    m_event_stats.Clear();
}

////////////////////////////////////////////////////////////////////////////////
void EventAction::EndOfEventAction(const G4Event* event)
{
    // Add energy deposited to histogram and classify energy deposition
    G4double total_energy_deposition = 0.0;
    G4double tolerance = 1.0*keV;

    // for ( const auto& [id, info] : m_map_id_info ){
    //     G4cout << id << "  -> " << info.positron_child_id << ", " << info.positron_child_particle_definition->GetParticleName() << ", " << info.is_511_gamma << G4endl;
    // }

    for ( const auto [id,energy] : m_energy_deposited ){
        // Don't care about where the energy came from - just want to add the total (won't be able to distinguish in reality)
        total_energy_deposition += energy;

        // Count the statistics of 511 keV annihilation photons
        // ID == 1 is original positron - reject this
        if ( id > 1 ){
            // Now check if energy deposited originates from a 511 keV gamma
            if ( m_map_id_info[id].is_511_gamma ){
                if( std::abs( energy - CLHEP::electron_mass_c2 ) < tolerance ){
                    m_event_stats.Add( STATS_INTS::N_511_FULLY_ABSORBED, 1 );
                }
                else if ( energy < tolerance ){
                    m_event_stats.Add( STATS_INTS::N_511_NOT_ABSORBED, 1 );
                }
                else{
                    m_event_stats.Add( STATS_INTS::N_511_PARTIALLY_ABSORBED, 1 );
                }
            }
            else{
                m_event_stats.Add( STATS_INTS::N_NOT_511, 1 );
            }
        }
    }

    // Fill histogram
    G4AnalysisManager::Instance()->FillH1( 0, total_energy_deposition );

    // Get Run object
    const Run* myrun = static_cast<const Run*>( G4RunManager::GetRunManager()->GetCurrentRun() );

    // Add event stats to run object and clear
    myrun->AddStats( m_event_stats );
    m_event_stats.Clear();

    // Print progress bar - make atomic integer
    static std::atomic<int> progress_counter{0};

    // Make local copy
    int local_progress_counter = ++progress_counter;

    // Print progress bar
    PrintProgressBar(local_progress_counter);

    return;
}

////////////////////////////////////////////////////////////////////////////////
void EventAction::StoreAnglesOfEscapingPositron( const G4double theta, const G4double phi )
{
    G4AnalysisManager* analysis_manager = G4AnalysisManager::Instance();
    analysis_manager->FillH1(HISTOGRAM_IDS::H1::ESCAPED_POSITRON_THETA, theta );
    analysis_manager->FillH1(HISTOGRAM_IDS::H1::ESCAPED_POSITRON_PHI, phi );
    analysis_manager->FillH2(HISTOGRAM_IDS::H2::ESCAPED_POSITRON_THETA_PHI, theta, phi );
    return;
}
////////////////////////////////////////////////////////////////////////////////
void EventAction::AddTrackInfo( G4int id, const MyTrackingInfo info ){
    m_map_id_info[id] = info;
}
////////////////////////////////////////////////////////////////////////////////
void EventAction::PrintProgressBar(int count)
{
    if ( count % 1000 == 0 && G4Threading::G4GetThreadId() == 0 ){
        int total_events = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed();
        constexpr int width = 60;
        double fraction = static_cast<double>(count)/total_events;
        int filled = width*fraction;
        G4cout << "\r[" << std::setfill('=') << std::setw(filled) << "" << std::setfill(' ') << std::setw( width - filled ) << "" << "] " << std::fixed << std::setprecision(1) << (fraction*100.0) << "% [" << std::setw(10) << count << "/" << std::setw(10) << total_events << "]" << std::flush;
    }
    return;
}
