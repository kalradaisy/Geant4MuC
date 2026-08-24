#ifndef SplitNeutrinoPhysics_h
#define SplitNeutrinoPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"
#include <vector>

#include "G4NuMuNucleusCcModel.hh"
#include "G4HadFinalState.hh"
#include "G4HadSecondary.hh"
#include "G4ParticleDefinition.hh"

#include "G4NeutrinoElectronCcModel.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "G4NeutrinoElectronNcModel.hh"
#include "G4IonTable.hh"
#include "G4DynamicParticle.hh"

#include "G4VCrossSectionDataSet.hh"
#include "G4NeutrinoElectronNcXsc.hh"
#include "G4NeutrinoElectronCcXsc.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Isotope.hh"
#include "G4MuNeutrinoNucleusTotXsc.hh"
#include "G4ElNeutrinoNucleusTotXsc.hh"
#include <cmath>

// ----------------------------------------------------------------------
// Muon (Anti) Neutrtino Charged Current Nuclear Scattering Cross Section
// ----------------------------------------------------------------------
class MyMuNeutrinoNucleusCcXsc : public G4MuNeutrinoNucleusTotXsc {
public:
    MyMuNeutrinoNucleusCcXsc() : G4MuNeutrinoNucleusTotXsc() {}
    virtual ~MyMuNeutrinoNucleusCcXsc() = default;
    /* GetIsoCrossSection intrinsically performs a check on whether the particle
    is matter or antimatter and returns the appropriate cross section ratio
    based on that information, so we don't need a different class for the
    differing matter cases*/
    virtual G4double GetIsoCrossSection(const G4DynamicParticle* aPart, G4int Z, G4int A,  
                                        const G4Isotope* iso, 
                                        const G4Element* elm, 
                                        const G4Material* mat) override 
    {
        // 1. Call the base class to perform the native calculation
        // This calculates ccnuXsc and ncXsc internally, sets fCcTotRatio, and returns totXsc.
        G4double totXsc = G4MuNeutrinoNucleusTotXsc::GetIsoCrossSection(aPart, Z, A, iso, elm, mat);

        // 2. Extract only the CC portion using the freshly updated ratio for this specific isotope
        return totXsc * GetCcTotRatio();
    }
};

// ----------------------------------------------------------------------
// Muon (Anti) Neutrtino Neutral Current Nuclear Scattering Cross Section
// ----------------------------------------------------------------------
class MyMuNeutrinoNucleusNcXsc : public G4MuNeutrinoNucleusTotXsc {
public:
    MyMuNeutrinoNucleusNcXsc() : G4MuNeutrinoNucleusTotXsc() {}
    virtual ~MyMuNeutrinoNucleusNcXsc() = default;

    virtual G4double GetIsoCrossSection(const G4DynamicParticle* aPart, G4int Z, G4int A,  
                                        const G4Isotope* iso, 
                                        const G4Element* elm, 
                                        const G4Material* mat) override 
    {
        // Call the base class ...
        G4double totXsc = G4MuNeutrinoNucleusTotXsc::GetIsoCrossSection(aPart, Z, A, iso, elm, mat);

        // .. and extract only the NC portion
        return totXsc * (1.0 - GetCcTotRatio());
    }
};

// ----------------------------------------------------------------------
// Electron (Anti) Neutrtino Charged Current Nuclear Scattering Cross Section
// ----------------------------------------------------------------------
class MyElNeutrinoNucleusCcXsc : public G4ElNeutrinoNucleusTotXsc {
public:
    MyElNeutrinoNucleusCcXsc() : G4ElNeutrinoNucleusTotXsc() {}
    virtual ~MyElNeutrinoNucleusCcXsc() = default;
    /* GetIsoCrossSection intrinsically performs a check on whether the particle
    is matter or antimatter and returns the appropriate cross section ratio
    based on that information, so we don't need a different class for the
    differing matter cases*/
    virtual G4double GetIsoCrossSection(const G4DynamicParticle* aPart, G4int Z, G4int A,  
                                        const G4Isotope* iso, 
                                        const G4Element* elm, 
                                        const G4Material* mat) override 
    {
        // Call the base class to perform the native calculation
        // This calculates ccnuXsc and ncXsc internally, sets fCcTotRatio, and returns totXsc.
        G4double totXsc = G4ElNeutrinoNucleusTotXsc::GetIsoCrossSection(aPart, Z, A, iso, elm, mat);

        // Extract only the CC portion using the freshly updated ratio for this specific isotope
        return totXsc * GetCcTotRatio();
    }
};
// ----------------------------------------------------------------------
// Electron (Anti) Neutrtino Neutral Current Nuclear Scattering Cross Section
// ----------------------------------------------------------------------
class MyElNeutrinoNucleusNcXsc : public G4ElNeutrinoNucleusTotXsc {
public:
    MyElNeutrinoNucleusNcXsc() : G4ElNeutrinoNucleusTotXsc() {}
    virtual ~MyElNeutrinoNucleusNcXsc() = default;

    virtual G4double GetIsoCrossSection(const G4DynamicParticle* aPart, G4int Z, G4int A,  
                                        const G4Isotope* iso, 
                                        const G4Element* elm, 
                                        const G4Material* mat) override 
    {
        // Call the base class
        G4double totXsc = G4ElNeutrinoNucleusTotXsc::GetIsoCrossSection(aPart, Z, A, iso, elm, mat);

        // Extract only the NC portion
        return totXsc * (1.0 - GetCcTotRatio());
    }
};


class SplitNeutrinoPhysics : public G4VPhysicsConstructor
{
public:
    SplitNeutrinoPhysics(const G4String& name = "SplitNeutrinoPhysics");
    virtual ~SplitNeutrinoPhysics() override = default;

    virtual void ConstructParticle() override {} // Particles constructed by reference physics list
    virtual void ConstructProcess() override;

private:
    // Sector-specific construction helpers
    void ConstructNuElectronProcesses();
    void ConstructNuNucleusProcesses();

    // Helper to cleanly register new processes and purge deprecated ones across all neutrino species
    void RegisterProcessesForNeutrinos(
        const std::vector<G4String>& neutrinos,
        const std::vector<G4VProcess*>& newProcesses,
        const std::vector<G4String>& processesToRemove = {}
    );
};


class MyNeutrinoElectronNcXsc : public G4NeutrinoElectronNcXsc {
public:
    MyNeutrinoElectronNcXsc() : G4NeutrinoElectronNcXsc() {
        SetForAllAtomsAndEnergies(true);
    }
    virtual ~MyNeutrinoElectronNcXsc() {}

    // Claim applicability for any target element with atomic electrons

    /* NB: We don't need to check dp to make sure the dynamic particle is a
    neutrino since our biasing wrapper is already coded to only attach to
    neutrinos, so we can safely leve out dp*/
    virtual G4bool IsElementApplicable(const G4DynamicParticle* /*dp*/, 
                                       G4int Z, 
                                       const G4Material* /*mat*/ = nullptr) override 
    {
        return (Z >= 1);
    }

    // Claim applicability for any isotope with Z >= 1
    // Otherwise, GEANT4 gets very concerned with isotopes
    virtual G4bool IsIsoApplicable(const G4DynamicParticle* /*dp*/, 
                                   G4int Z, G4int /*A*/,
                                   const G4Element* /*elm*/ = nullptr, 
                                   const G4Material* /*mat*/ = nullptr) override 
    {
        return (Z >= 1);
    }

    virtual G4double GetElementCrossSection(const G4DynamicParticle* dp, 
                                            G4int Z, 
                                            const G4Material* mat = nullptr) override 
    {
        // Enforce native Geant4 particle & threshold checks
        if (!G4NeutrinoElectronNcXsc::IsElementApplicable(dp, Z, mat)) {
            return 0.0;
        }
        return G4NeutrinoElectronNcXsc::GetElementCrossSection(dp, Z, mat);
    }

    // Electron density is not a function of isotope, so just use standard xsec
    virtual G4double GetIsoCrossSection(const G4DynamicParticle* dp, 
                                        G4int Z, G4int /*A*/,
                                        const G4Isotope* /*iso*/ = nullptr, 
                                        const G4Element* /*elm*/ = nullptr, 
                                        const G4Material* mat = nullptr) override 
    {
        return GetElementCrossSection(dp, Z, mat);
    }
};

class MyNeutrinoElectronCcXsc : public G4NeutrinoElectronCcXsc {
public:
    MyNeutrinoElectronCcXsc() : G4NeutrinoElectronCcXsc() {
        SetForAllAtomsAndEnergies(true);
    }
    virtual ~MyNeutrinoElectronCcXsc() {}

    // Broad applicability to satisfy G4CrossSectionDataStore registration
    virtual G4bool IsElementApplicable(const G4DynamicParticle* /*dp*/, 
                                       G4int Z, 
                                       const G4Material* /*mat*/ = nullptr) override 
    {
        return (Z >= 1);
    }

    virtual G4bool IsIsoApplicable(const G4DynamicParticle* /*dp*/, 
                                   G4int Z, G4int /*A*/,
                                   const G4Element* /*elm*/ = nullptr, 
                                   const G4Material* /*mat*/ = nullptr) override 
    {
        return (Z >= 1);
    }

    // Let GEANT4 do the cross section calculation
    virtual G4double GetElementCrossSection(const G4DynamicParticle* dp, 
                                            G4int Z, 
                                            const G4Material* mat = nullptr) override 
    {
        // Enforce native Geant4 particle & threshold checks
        if (!G4NeutrinoElectronCcXsc::IsElementApplicable(dp, Z, mat)) {
            return 0.0;
        }
        return G4NeutrinoElectronCcXsc::GetElementCrossSection(dp, Z, mat);
    }

    virtual G4double GetIsoCrossSection(const G4DynamicParticle* dp, 
                                        G4int Z, G4int /*A*/,
                                        const G4Isotope* /*iso*/ = nullptr, 
                                        const G4Element* /*elm*/ = nullptr, 
                                        const G4Material* mat = nullptr) override 
    {
        return GetElementCrossSection(dp, Z, mat);
    }
};


/* NB: This type of subclassing SHOULD be extended to the other models. It is
a TODO for the future, assuming you, for some reason, want to simulate neutrino
interactions in Geant4, which, from my investigations, seems inadvisable in
version 11.4.2 due to energy, momentum, and charge imbalances, incorrect energy
thresholds for IMD & ITD, and ignoring of applicability checks allowing for
unphysical interactions such as muon anti neutrino charged-current electron
scattering.*/
class MyNuMuNucleusCcModel : public G4NuMuNucleusCcModel {
public:
    MyNuMuNucleusCcModel() : G4NuMuNucleusCcModel() {}
    // Have to subclass our models because G4 does not handle energy, momentum,
    // and charge conservation very thoroughly.

    virtual const std::pair<G4double, G4double> GetFatalEnergyCheckLevels() const override {
        // 0.02% relative, 10 MeV absolute, for some reason I still get ~50 MeV
        // momentum imbalances. I don't understand why this happens, but altering
        // the strictness of the checks below does indeed make noticable changes
        // to the accuracy of the output momentum as compared to the input
        return std::pair<G4double, G4double>(0.0002, 10.0 * CLHEP::MeV);
    }

    virtual G4HadFinalState* ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& targetNucleus) override {
        G4HadFinalState* result = nullptr;
        bool isConserved = false;
        
        G4int attempts = 0;
        const G4int maxAttempts = 1000; // Prevent infinite loops if the model gets stuck

        // Target and Projectile initial properties
        G4double initialCharge = aTrack.GetDefinition()->GetPDGCharge() + targetNucleus.GetZ_asInt();
        G4ThreeVector initialMomentum = aTrack.Get4Momentum().vect();

        while (!isConserved && attempts < maxAttempts) {
            attempts++;

            // Call the original model to generate an event
            // Note: The base class calls theParticleChange.Clear() internally, 
            // so we do not need to worry about secondaries piling up between attempts.
            result = G4NuMuNucleusCcModel::ApplyYourself(aTrack, targetNucleus);

            // If the neutrino bypassed the interaction (e.g. below threshold, cascade failed),
            // it will have no secondaries. We just pass it through immediately.
            if (result->GetNumberOfSecondaries() == 0) {
                return result;
            }

            // Tally the final state
            G4double finalCharge = 0.0;
            G4ThreeVector finalMomentum(0., 0., 0.);

            for (std::size_t i = 0; i < result->GetNumberOfSecondaries(); ++i) {
                G4DynamicParticle* sec = result->GetSecondary(i)->GetParticle();
                finalCharge += sec->GetDefinition()->GetPDGCharge();
                finalMomentum += sec->GetMomentum();
            }

            // 4. Check Conservation
            G4double chargeDiff = std::abs(initialCharge - finalCharge);
            G4double momentumDiff = (initialMomentum - finalMomentum).mag();

            // Strict constraints: exact charge match, 100 MeV/c momentum match
            bool chargeConserved = (chargeDiff < 0.1); 
            bool momentumConserved = (momentumDiff < 100.0 * CLHEP::MeV);
            // If we tighten the momentum conservation much more, G4 has a very
            // hard time satisfying our request. This is because G4 has very
            // poor momentum bookkeping in these neutrino interactions, so we
            // are essentially asking it to guess the right answer.

            if (chargeConserved && momentumConserved) {
                isConserved = true;
            }
        }

        // If the model cannot generate a valid event after 1000 tries,
        // we veto the interaction entirely.
        if (!isConserved) {
            result->Clear(); // Wipe the bad secondaries
            result->SetStatusChange(isAlive); // Keep the neutrino alive
            result->SetEnergyChange(aTrack.GetKineticEnergy()); 
            result->SetMomentumChange(aTrack.Get4Momentum().vect().unit());
        }

        return result;
    }
};

class MyNuElectronNcModel : public G4NeutrinoElectronNcModel {
public:
    MyNuElectronNcModel() : G4NeutrinoElectronNcModel() {}

    G4HadFinalState* ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& targetNucleus) override {
        // Execute the standard model calculation
        G4HadFinalState* result = G4NeutrinoElectronNcModel::ApplyYourself(aTrack, targetNucleus);

        // Add the spectator target nucleus at rest to balance target mass
        if (result) {
            G4int Z = targetNucleus.GetZ_asInt();
            G4int A = targetNucleus.GetA_asInt();
            G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, 0.0);
            
            if (ion) {
                G4DynamicParticle* spectator = new G4DynamicParticle(ion, G4ThreeVector(0,0,1), 0.0);
                result->AddSecondary(spectator);
            }
        }
        return result;
    }
};

class MyNuElectronCcModel : public G4NeutrinoElectronCcModel {
public:
    MyNuElectronCcModel() : G4NeutrinoElectronCcModel() {}

    G4HadFinalState* ApplyYourself(const G4HadProjectile& aTrack, G4Nucleus& targetNucleus) override {
        // Execute the standard model calculation
        G4HadFinalState* result = G4NeutrinoElectronCcModel::ApplyYourself(aTrack, targetNucleus);

        // Add the spectator target nucleus at rest to balance target mass
        if (result) {
            G4int Z = targetNucleus.GetZ_asInt();
            G4int A = targetNucleus.GetA_asInt();
            G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, 0.0);
            
            if (ion) {
                // Set the spectator hadron with some unit momentum and 0 energy
                // This way its rest mass only contributes to E_{out}
                G4DynamicParticle* spectator = new G4DynamicParticle(ion, G4ThreeVector(0,0,1), 0.0);
                result->AddSecondary(spectator);
            }
        }
        return result;
    }
};

#endif