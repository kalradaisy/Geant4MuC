#include "Biasing.hh"

#include "G4BOptnChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4BiasingProcessSharedData.hh"
#include "G4NeutrinoMu.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4VParticleChange.hh"
#include "BiasingMessenger.hh"

#include <cfloat>

Biasing::Biasing(const G4String& name, G4double defaultFactor) :
G4VBiasingOperator(name), 
//fParticleToBias(G4NeutrinoMu::NeutrinoMu()),
fSetup(true)
{
    // Processes to bias:
    fProcessesToBias.insert("muNuNucleus");
    fProcessesToBias.insert("elNuNucleus");

    fProcessesToBias.insert("nuElectron");

    // Initialize with default values for the biasing
    fBiasFactors["muNuNucleus"] = defaultFactor;
    fBiasFactors["elNuNucleus"] = defaultFactor;
    fBiasFactors["nuElectron"]  = defaultFactor;

    // Prepare the messenger
    fMessenger = new BiasingMessenger(this);

    G4cout << "[Biasing] Operator created. Listening for macro commands..."
    << G4endl;
}

Biasing::~Biasing()
{
    for (auto& entry : fOperations)
    {
        delete entry.second;
    }
    delete fMessenger;
}

void Biasing::SetBiasFactor(const G4String& processName, G4double factor)
{
    fBiasFactors[processName] = factor;
    G4cout << "[Biasing] Updated " << processName << " bias factor to: " << factor << G4endl;
}

void Biasing::StartRun()
{
    fSetup = false;
}

/*void Biasing::StartTracking(
    const G4Track* track)
{
    G4cout
        << "[Biasing] StartTracking "
        << track->GetTrackID()
        << " "
        << track->GetDefinition()->GetParticleName()
        << G4endl;
}*/
// Debug Print Statement

G4VBiasingOperation* Biasing::ProposeOccurenceBiasingOperation(
const G4Track* track, const G4BiasingProcessInterface* callingProcess)
{
    // Only bias nu_mu, which we don't want to do
    /*if (track->GetDefinition() != fParticleToBias)
    {
        return nullptr;
    }*/

    auto processName =
        callingProcess->GetWrappedProcess()->GetProcessName();

    if (!fProcessesToBias.count(processName))
    {
        return nullptr;
    }

    /*Below are some commands that were needed in debugging. They are
    essentially scaffolding to make sure tracks are always behaving as intended,
    but they are not necessary when all machinery is running smoothly. The may
    be safely deleted, but they are retained in case of future debugging need.*/
    /*if (track->GetTrackStatus() != fAlive) {
        return nullptr; // Do not propose biasing if the track is already dying
    }
    if (track->GetKineticEnergy() == 0.0 || track->GetTrackStatus() != fAlive) {
    return nullptr;
    }*/

    auto operationIter = fOperations.find(callingProcess);

    /*if (operationIter == fOperations.end())
    {
        return nullptr;
    }

    auto operation = operationIter->second;*/

    //  multi-particle biasing 
    // ---- LAZY INITIALIZATION ----
    if (operationIter == fOperations.end())
    {
        // If the operation doesn't exist for this process yet, create it!
        G4cout << "[Biasing] Creating XS operation for " << processName << G4endl;
        auto newOperation = new G4BOptnChangeCrossSection("XSBias-" + processName);
        
        fOperations[callingProcess] = newOperation;
        operationIter = fOperations.find(callingProcess);
    }

    auto operation = operationIter->second;

    G4double analogInteractionLength = callingProcess->GetWrappedProcess()
    ->GetCurrentInteractionLength();
    G4double analogXS = 0.0;

    if (analogInteractionLength > DBL_MAX / 10.0)
    {
        return nullptr;
    }

    if (analogInteractionLength > 0.0 && analogInteractionLength < DBL_MAX) {
            analogXS = 1.0 / analogInteractionLength;
        }

    //G4double analogXS = callingProcess->GetWrappedProcess()
    //->GetCrossSection(track, step);

    G4double currentFactor = 1.0;
    if (fBiasFactors.find(processName) != fBiasFactors.end()) {
            currentFactor = fBiasFactors[processName];
        }

    G4double biasedXS = currentFactor * analogXS;

    G4VBiasingOperation* previousOperation = callingProcess
            ->GetPreviousOccurenceBiasingOperation();

    if (previousOperation == nullptr)
    {
        operation->SetBiasedCrossSection(biasedXS);

        operation->Sample();

        /*G4cout
            << "[Biasing] First sample for "
            << processName
            << " analogXS="
            << analogXS
            << " biasedXS="
            << biasedXS
            << G4endl;*/
        //^^^^^^^^ biasing debug print out
    }
    else
    {
        if (operation->GetInteractionOccured())
        {
            operation->SetBiasedCrossSection(biasedXS);

            operation->Sample();

            G4cout << "[Biasing] Resampling after interaction" << G4endl;
        }
        else
        {
            operation->UpdateForStep(callingProcess->GetPreviousStepSize());

            operation->SetBiasedCrossSection(biasedXS);

        }
    }

    return operation;
}

void Biasing::OperationApplied(
    const G4BiasingProcessInterface* callingProcess,
    G4BiasingAppliedCase,
    G4VBiasingOperation* occurenceOperationApplied,
    G4double,
    G4VBiasingOperation*,
    const G4VParticleChange*)
{
    auto iter = fOperations.find(callingProcess);

    if (iter == fOperations.end())
    {
        return;
    }

    auto operation = iter->second;

    if (operation == occurenceOperationApplied)
    {
        operation->SetInteractionOccured();

        G4cout
            << "\n ===========================================\n"
            << "[Biasing] Interaction occurred in "
            << callingProcess
                   ->GetWrappedProcess()
                   ->GetProcessName()
            << "\n ===========================================\n"
            << G4endl;
    }
}

