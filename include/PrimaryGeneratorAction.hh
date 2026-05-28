#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;
class G4Box;
class DetectorConstruction;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        PrimaryGeneratorAction();
        ~PrimaryGeneratorAction() override;

        // method from the base class
        void GeneratePrimaries(G4Event*) override;

        // method to access particle gun
        const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

    private:
        G4ParticleGun* fParticleGun = nullptr;
        const DetectorConstruction* m_detector_construction;
};

#endif
