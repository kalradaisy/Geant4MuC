#include "MyNeutrinoPhysics.hh"
#include "MyNeutrinoPhysicsMessenger.hh"

#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

// Particle definitions
#include "G4Electron.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

// Neutrino-Electron Process, Datasets, and Models
#include "G4NeutrinoElectronProcess.hh"
#include "G4NeutrinoElectronTotXsc.hh"
#include "G4NeutrinoElectronCcModel.hh"
#include "G4NeutrinoElectronNcModel.hh"

MyNeutrinoPhysics::MyNeutrinoPhysics(const G4String& name)
  : G4VPhysicsConstructor(name)
{
    fMessenger = new MyNeutrinoPhysicsMessenger(this);
}

MyNeutrinoPhysics::~MyNeutrinoPhysics()
{
    delete fMessenger;
}

void MyNeutrinoPhysics::SetNuEleCcBias(G4double bf)
{
    if (bf > 0.0) fNuEleCcBias = bf;
}

void MyNeutrinoPhysics::SetNuEleNcBias(G4double bf)
{
    if (bf > 0.0) fNuEleNcBias = bf;
}

void MyNeutrinoPhysics::SetNuDetectorName(const G4String& name)
{
    fNuDetectorName = name;
}

void MyNeutrinoPhysics::ConstructParticle()
{
    G4Electron::Electron();
    G4NeutrinoE::NeutrinoE();
    G4AntiNeutrinoE::AntiNeutrinoE();
    G4NeutrinoMu::NeutrinoMu();
    G4AntiNeutrinoMu::AntiNeutrinoMu();
    G4NeutrinoTau::NeutrinoTau();
    G4AntiNeutrinoTau::AntiNeutrinoTau();
}

void MyNeutrinoPhysics::ConstructProcess()
{
    const G4ParticleDefinition* neutrinos[6] = {
        G4AntiNeutrinoE::AntiNeutrinoE(),
        G4NeutrinoE::NeutrinoE(),
        G4AntiNeutrinoMu::AntiNeutrinoMu(),
        G4NeutrinoMu::NeutrinoMu(),
        G4AntiNeutrinoTau::AntiNeutrinoTau(),
        G4NeutrinoTau::NeutrinoTau()
    };

    // 1. Create process bound to target detector region name
    auto nuEleProcess = new G4NeutrinoElectronProcess(fNuDetectorName);
    auto nuEleTotXsc  = new G4NeutrinoElectronTotXsc();

    // 2. Set CC and NC bias factors on both process and cross-section dataset
    nuEleProcess->SetBiasingFactors(fNuEleCcBias, fNuEleNcBias);
    nuEleTotXsc->SetBiasingFactors(fNuEleCcBias, fNuEleNcBias);

    // 3. Register cross-section dataset (Prevents had001 fatal crash)
    nuEleProcess->AddDataSet(nuEleTotXsc);

    // 4. Register CC and NC interaction models
    auto ccModel = new G4NeutrinoElectronCcModel();
    auto ncModel = new G4NeutrinoElectronNcModel();
    nuEleProcess->RegisterMe(ccModel);
    nuEleProcess->RegisterMe(ncModel);

    // 5. Attach process to all neutrino particle species
    for (G4int i = 0; i < 6; ++i) {
        neutrinos[i]->GetProcessManager()->AddDiscreteProcess(nuEleProcess);
    }
}