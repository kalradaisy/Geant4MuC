#include "TrackingAction.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>

TrackingAction::TrackingAction(EventAction* eventAction)
    : fEventAction(eventAction) {}

void TrackingAction::PreUserTrackingAction(const G4Track* track)
{
    // ================================================================
    // Fix broken oscillation secondaries BEFORE they take any step.
    //
    // The Geant4 nuVacOscillation process creates secondaries with
    // 0 energy and (0,0,0) momentum. By the time SteppingAction sees
    // them, Geant4 has already tried to look up cross sections at E=0
    // and thrown a fatal exception.
    //
    // TrackingAction::PreUserTrackingAction fires BEFORE the first
    // step, so we can fix kinematics here in time.
    // ================================================================

    if (track->GetParentID() == 0) return;  // skip primary

    const G4VProcess* creator = track->GetCreatorProcess();
    if (!creator) return;

    if (creator->GetProcessName() != "nuVacOscillation") return;

    int absPDG = std::abs(track->GetDefinition()->GetPDGEncoding());
    bool isNeutrino = (absPDG == 12 || absPDG == 14 || absPDG == 16);
    if (!isNeutrino) return;

    // Check if kinematics are broken
    if (track->GetKineticEnergy() < 1.0 * eV)
    {
        G4cout << "\n*** TrackingAction: Fixing broken oscillation secondary ***"
               << "\n    PDG = " << track->GetDefinition()->GetPDGEncoding()
               << "\n    KE  = " << track->GetKineticEnergy() / GeV << " GeV (before fix)"
               << G4endl;

        // We need the parent's kinematics. Since vacuum oscillation only
        // changes flavor (not energy or direction), the parent's info at
        // the oscillation vertex is what we need.
        //
        // Strategy: get it from EventAction (the primary's recorded info)
        // since oscillation typically happens to the primary neutrino.

        G4double fixedKE = 0.0;
        G4ThreeVector fixedDir(0, 0, 1);

        if (fEventAction) {
            // Use the primary's recorded energy and direction
            fixedKE = fEventAction->E;  // primary KE recorded at step 1
            G4ThreeVector pMom(fEventAction->px, fEventAction->py, fEventAction->pz);
            G4double pMag = pMom.mag();
            if (pMag > 0) {
                fixedDir = pMom.unit();
            }
        }

        if (fixedKE < 1.0 * eV) {
            // Fallback: if EventAction doesn't have it yet, use a safe default
            // This shouldn't happen if the primary was already tracked
            G4cout << "    WARNING: Could not retrieve parent kinematics from EventAction."
                   << "\n    Killing this track to prevent crash." << G4endl;
            const_cast<G4Track*>(track)->SetTrackStatus(fStopAndKill);
            return;
        }

        // Apply the fix
        G4Track* mutableTrack = const_cast<G4Track*>(track);
        mutableTrack->SetKineticEnergy(fixedKE);
        mutableTrack->SetMomentumDirection(fixedDir);

        G4cout << "    Fixed KE  = " << fixedKE / GeV << " GeV"
               << "\n    Fixed dir = " << fixedDir
               << G4endl;
    }
}
