#include "PrimaryGenerator.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "RunAction.hh"
#include "TTree.h"
#include "PrimaryGeneratorMessenger.hh"
#include "G4RunManager.hh"
#include "CLHEP/Units/PhysicalConstants.h"

/*
e-
e+
gamma
proton
neutron
pi+
pi-
pi0
mu-
mu+
kaon+
*/
PrimaryGenerator::PrimaryGenerator(RunAction* runAction)
: fRunAction(runAction)
{
  fMessenger = new PrimaryGeneratorMessenger(this);
  fParticleGun = new G4ParticleGun(1);

 fParticleName = "nu_mu";          // default particle
    fEnergy = 1.0 * CLHEP::GeV;       // use CLHEP::GeV
    //    fPosition = G4ThreeVector(0, 0, -1 * CLHEP::cm);
    fPosition = G4ThreeVector(0, 0,0);
    // Initialize particle gun
    G4ParticleDefinition* particle
        = G4ParticleTable::GetParticleTable()->FindParticle(fParticleName);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(fEnergy);
    fParticleGun->SetParticlePosition(fPosition);

    
  //auto particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  //fParticleGun->SetParticleDefinition(particle);
  // fParticleGun->SetParticleEnergy(1.0*GeV);
  //fParticleGun->SetParticlePosition(G4ThreeVector(0,0,0));
  
  //fMessenger = new PrimaryGeneratorMessenger(this);
  
  // Default values (can be overridden by macro)
  //fParticleName = "e-";             
  //fEnergy = 1.0*GeV;
  //fPosition = G4ThreeVector(0,0,0*cm);
  //fDirection = G4ThreeVector(0,0,0);
}

PrimaryGenerator::~PrimaryGenerator() {
  delete fParticleGun;
  delete fMessenger;
}

void PrimaryGenerator::GeneratePrimaries(G4Event* event)
{
    // Get particle
   if(fParticleName.empty()) {
      G4Exception("PrimaryGenerator","NoParticle",FatalException,
                  "Particle name not set.");
   }

  auto particle =
     G4ParticleTable::GetParticleTable()->FindParticle(fParticleName);

  if(!particle) {
       G4Exception("PrimaryGenerator","NoParticle",FatalException,
                  ("Particle not found: "+fParticleName).c_str());
   }

    // Update gun from current variables
    //    fParticleGun->SetParticleDefinition(particle);
    // fParticleGun->SetParticleEnergy(fEnergy);
    // fParticleGun->SetParticlePosition(fPosition);

    
    // -----------------------------
    // 1) Random direction (4pi)
    // -----------------------------
    double costh = 2.0*G4UniformRand() - 1.0;
    double sinth = std::sqrt(1.0 - costh*costh);
    double phi   = 2.0*CLHEP::pi*G4UniformRand();

    G4ThreeVector dir(
        sinth*std::cos(phi),
        sinth*std::sin(phi),
        costh
    );

    // -----------------------------
    // 2) Beam spot (optional)
    // -----------------------------
    /* double sx = 0.01*cm;
    double sy = 0.01*cm;
    double sz = 5.0*cm;

    double x = G4RandGauss::shoot(0., sx);
    double y = G4RandGauss::shoot(0., sy);
    double z = G4RandGauss::shoot(0., sz);

    G4ThreeVector pos(x,y,z);
    */
    
    // -----------------------------
    // 3) Energy (optional)
    // -----------------------------
     double E = 0.2*GeV + G4UniformRand()*4.8*GeV;
       //      double E = fEnergy;


     //double zMin = -20*cm, zMax = 2*cm;
     //double xMin = -2*cm, xMax = 2*cm;
     //double yMin = -2*cm, yMax = 2*cm;

    //       fPosition = G4ThreeVector(
    //			      xMin + G4UniformRand()*(xMax-xMin),
    //			      yMin + G4UniformRand()*(yMax-yMin),
    //			      zMin + G4UniformRand()*(zMax-zMin)
    ///			      );
    
    
    // -----------------------------
    // 4) Set gun
    // -----------------------------
         fParticleGun->SetParticleDefinition(particle);
     fParticleGun->SetParticleEnergy(fEnergy);
     fParticleGun->SetParticlePosition(fPosition);
    fParticleGun->SetParticleMomentumDirection(dir);

    fParticleGun->GeneratePrimaryVertex(event);


     // ---- Print info ----
    G4cout << "Primary generated: "
           << fParticleGun->GetParticleDefinition()->GetParticleName()
           << "  Energy: " << fParticleGun->GetParticleEnergy()/GeV << " GeV"
           << "  Position: " << fParticleGun->GetParticlePosition()
           << "  Direction: " << fParticleGun->GetParticleMomentumDirection()
           << G4endl;
    
}


