#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4Positron.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

#include <G4ThreeVector.hh>
#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1::PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr)
{
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    //设置入射粒子为gamma射线
    //G4ParticleDefinition* gamma = G4Gamma::GammaDefinition();
    //fParticleGun->SetParticleDefinition(gamma);
    //fParticleGun->SetParticleEnergy(0.144 * MeV);  // 144 keV 正电子

    //设置入射粒子为电子
    G4ParticleDefinition* positron = G4Positron::Positron();
    fParticleGun->SetParticleDefinition(positron);
    //fParticleGun->SetParticleEnergy(1.228 * MeV);  // 1.228 MeV 正电子
    fParticleGun->SetParticleEnergy(0.614 * MeV);  // 1.228 MeV 正电子
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1::PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1::PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // 靶参数
    // G4double targetRadius = 2.0 * cm;
    // G4double fixedDepth = 4.8484 * mm;  // 固定深度4.8484 mm
    //
    // // 固定深度：从表面(-6mm)向下5mm处
    // G4double surfaceZ = 5.0 * mm;
    // G4double startZ = surfaceZ - fixedDepth;     // 固定深度: -3.82 mm
    //
    // // 在靶横截面圆形区域内随机位置
    // G4double r = targetRadius * std::sqrt(G4UniformRand());
    // G4double phi = 2 * M_PI * G4UniformRand();
    //
    // G4double startX = r * std::cos(phi);
    // G4double startY = r * std::sin(phi);

    G4ThreeVector start = DETECTORS::QQQ1Array::GetRandomPoint();
    fParticleGun->SetParticlePosition( DETECTORS::QQQ1Array::GetRandomPoint() );

    // 正电子各向同性发射方向
    G4double cosTheta = 2 * G4UniformRand() - 1;  // -1 到 1
    G4double sinTheta = std::sqrt(1 - cosTheta * cosTheta);
    G4double phi_dir = 2 * M_PI * G4UniformRand();

    G4double dirX = sinTheta * std::cos(phi_dir);
    G4double dirY = sinTheta * std::sin(phi_dir);
    G4double dirZ = cosTheta;

    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(dirX, dirY, dirZ));

    fParticleGun->GeneratePrimaryVertex(anEvent);

    // 调试输出
    static int count = 0;
    if (count % 1000000 == 0) {
        G4cout << "Generated 1.228 MeV positron at (" 
               << start.x()/mm << ", " << start.y()/mm << ", " << start.z()/mm << ") mm"
               << " with direction (" << dirX << ", " << dirY << ", " << dirZ << ")"
               << G4endl;
    }
    count++;
}
