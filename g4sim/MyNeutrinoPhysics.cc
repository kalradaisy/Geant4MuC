#include "MyNeutrinoPhysics.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

// Neutrino processes
#include "G4NeutrinoElectronProcess.hh"
#include "G4MuNeutrinoNucleusProcess.hh"

// Neutrino particles
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

#include "MyNeutrinoPhysics.hh"

MyNeutrinoPhysics::MyNeutrinoPhysics(const G4String& name)
 : G4VPhysicsConstructor(name)
{
    // constructor body
}

MyNeutrinoPhysics::~MyNeutrinoPhysics() {}

void MyNeutrinoPhysics::ConstructParticle() {
    // Ensure all neutrinos exist
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();
    G4NeutrinoTau::NeutrinoTauDefinition();
    G4AntiNeutrinoTau::AntiNeutrinoTauDefinition();
}

void MyNeutrinoPhysics::ConstructProcess() {
    auto particleIterator = GetParticleIterator();
    particleIterator->reset();

    while ((*particleIterator)()) {
        auto particle = particleIterator->value();
        auto pmanager = particle->GetProcessManager();
        if (!pmanager) continue; // safety check

	if (!pmanager) {
    G4cout << "ERROR: no process manager for " << particle->GetParticleName() << G4endl;
    continue;
}
        // Unique envelope name for each particle
        G4String envelopeName = particle->GetParticleName() + "_Envelope";

	G4cout << "Attaching process to particle: " << particle->GetParticleName() << G4endl;

        // === Electron neutrino / antineutrino: neutrino-electron scattering ===
        if (particle == G4NeutrinoE::NeutrinoE() || particle == G4AntiNeutrinoE::AntiNeutrinoE()) {
            auto nuProc = new G4NeutrinoElectronProcess(envelopeName, "neutrino-electron");
            pmanager->AddDiscreteProcess(nuProc);
        }

        // === Muon neutrino / antineutrino: neutrino-nucleus scattering ===
        if (particle == G4NeutrinoMu::NeutrinoMu() || particle == G4AntiNeutrinoMu::AntiNeutrinoMu()) {
            auto muProc = new G4MuNeutrinoNucleusProcess(envelopeName, "mu-neutrino-nucleus");
            pmanager->AddDiscreteProcess(muProc);
        }

        // === Tau neutrino / antineutrino: neutrino-electron scattering ===
        if (particle == G4NeutrinoTau::NeutrinoTau() || particle == G4AntiNeutrinoTau::AntiNeutrinoTau()) {
            auto tauProc = new G4NeutrinoElectronProcess(envelopeName, "tau-neutrino-electron");
            pmanager->AddDiscreteProcess(tauProc);
        }
    }
}
