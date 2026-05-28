#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4Positron.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <G4ThreeVector.hh>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////
PrimaryGeneratorAction::PrimaryGeneratorAction() :
    G4VUserPrimaryGeneratorAction(),
    fParticleGun(nullptr)
{
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    // Metastable state has energy of 2251 keV
    // Energy creation of e-e+ pair is 1022 keV
    // Remaining energy is 1229 keV, which is shared between the three bodies
    G4ParticleDefinition* positron = G4Positron::Positron();
    fParticleGun->SetParticleDefinition(positron);
    //fParticleGun->SetParticleEnergy(1.228 * MeV);  // 1.228 MeV 正电子
    fParticleGun->SetParticleEnergy(0.614 * MeV);  // 1.228 MeV 正电子

    m_detector_construction = static_cast<const DetectorConstruction*>( G4RunManager::GetRunManager()->GetUserDetectorConstruction() );
    G4cout << "PrimaryGeneratorAction DetectorConstruction " << m_detector_construction << G4endl;
}

////////////////////////////////////////////////////////////////////////////////
PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

////////////////////////////////////////////////////////////////////////////////
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4ThreeVector start = m_detector_construction->GetParticleGunOriginDetector()->GetRandomPoint();
    fParticleGun->SetParticlePosition(start);

    // 正电子各向同性发射方向
    G4double cosTheta = 2 * G4UniformRand() - 1; // -1 到 1
    G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
    G4double phi_dir = 2 * M_PI * G4UniformRand();

    G4double dirX = sinTheta * std::cos(phi_dir);
    G4double dirY = sinTheta * std::sin(phi_dir);
    G4double dirZ = cosTheta;

    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dirX, dirY, dirZ));
    fParticleGun->GeneratePrimaryVertex(anEvent);

}
