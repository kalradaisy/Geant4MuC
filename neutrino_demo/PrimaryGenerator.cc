#include "PrimaryGenerator.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "RunAction.hh"
#include "TTree.h"
#include "TFile.h"
#include "PrimaryGeneratorMessenger.hh"
#include "G4RunManager.hh"
#include "CLHEP/Units/PhysicalConstants.h"

/* Thread-safe atomic counter so worker threads never simulate the same
incident particle from a flux file*/
static std::atomic<int> g_FluxEntryIndex{0};

PrimaryGenerator::PrimaryGenerator(RunAction* runAction)
: fRunAction(runAction), fFluxFile(nullptr), fFluxTree(nullptr)
{
    fMessenger = new PrimaryGeneratorMessenger(this);
    fParticleGun = new G4ParticleGun(1);

    // default to gun with some default parameters
    fMode = "gun";
    fParticleName = "nu_mu";          // default particle
    fEnergy = 1.0 * CLHEP::GeV;       // use CLHEP::GeV
    fPosition = G4ThreeVector(0, 0,0);
    // Initialize particle gun
    G4ParticleDefinition* particle
        = G4ParticleTable::GetParticleTable()->FindParticle(fParticleName);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(fEnergy);
    fParticleGun->SetParticlePosition(fPosition);

    
}

PrimaryGenerator::~PrimaryGenerator() {
  delete fParticleGun;
  delete fMessenger;

  // in the case we're given a flux file:
  if (fFluxFile) {
        fFluxFile->Close();
        delete fFluxFile;
    }
}

void PrimaryGenerator::GeneratePrimaries(G4Event* event)
{
    // First, if we are working with a gun, use the existing architecture:
    if (fMode == "gun") 
    {
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

        double E = 0.2*GeV + G4UniformRand()*4.8*GeV;
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleEnergy(fEnergy);
        fParticleGun->SetParticlePosition(fPosition);
        fParticleGun->SetParticleMomentumDirection(dir);

        fParticleGun->GeneratePrimaryVertex(event);


        // ---- Print info for debugging ----
        /*
        G4cout << "Primary generated: "
            << fParticleGun->GetParticleDefinition()->GetParticleName()
            << "  Energy: " << fParticleGun->GetParticleEnergy()/GeV << " GeV"
            << "  Position: " << fParticleGun->GetParticlePosition()
            << "  Direction: " << fParticleGun->GetParticleMomentumDirection()
            << G4endl;
        */
    }

    // and if we instead have flux...
    else if (fMode == "flux") 
    {
        // ==========================================
        //            FLUX FILE READER
        // ==========================================
        
        // Lazy-load the ROOT file on the very first event for this thread
        if (!fFluxFile) {
            fFluxFile = new TFile(fFluxFileName.c_str(), "READ");
            if (!fFluxFile || fFluxFile->IsZombie()) {
                G4Exception("PrimaryGenerator", "NoFluxFile", FatalException, "Could not open flux ROOT file!");
            }
            
            fFluxTree = (TTree*)fFluxFile->Get("NeutrinoFlux");
            if (!fFluxTree) {
                G4Exception("PrimaryGenerator", "NoTree", FatalException, "Could not find 'NeutrinoFlux' tree!");
            }
            
            fFluxTree->SetBranchAddress("PDG", &fPDG);
            fFluxTree->SetBranchAddress("Energy_GeV", &fE_GeV);
            fFluxTree->SetBranchAddress("x_mm", &fX_mm);
            fFluxTree->SetBranchAddress("y_mm", &fY_mm);
            fFluxTree->SetBranchAddress("px", &fPx);
            fFluxTree->SetBranchAddress("py", &fPy);
            fFluxTree->SetBranchAddress("pz", &fPz);
            
            fTotalEntries = fFluxTree->GetEntries();
        }

        // Safely grab a unique row index for this specific event
        int currentEntry = g_FluxEntryIndex.fetch_add(1);

        if (currentEntry >= fTotalEntries) {
            G4cout << "\n*** WARNING: Ran out of flux events! Stopping run early. ***\n" << G4endl;
            G4RunManager::GetRunManager()->AbortRun();
            return;
        }

        fFluxTree->GetEntry(currentEntry);

        // Convert the PDG code into a Geant4 Particle
        G4ParticleDefinition* fluxParticle = nullptr;
        if (fPDG == 12)       fluxParticle = G4ParticleTable::GetParticleTable()->FindParticle("nu_e");
        else if (fPDG == -12) fluxParticle = G4ParticleTable::GetParticleTable()->FindParticle("anti_nu_e");
        else if (fPDG == 14)  fluxParticle = G4ParticleTable::GetParticleTable()->FindParticle("nu_mu");
        else if (fPDG == -14) fluxParticle = G4ParticleTable::GetParticleTable()->FindParticle("anti_nu_mu");

        if (!fluxParticle) return; // Skip if it's some other random particle

        // Apply kinematics from the file (convert to Geant4 internal units!)
        fParticleGun->SetParticleDefinition(fluxParticle);
        fParticleGun->SetParticleEnergy(fE_GeV * CLHEP::GeV);
        fZ_mm = fPosition[2];
        fParticleGun->SetParticlePosition(G4ThreeVector(fX_mm * CLHEP::mm, fY_mm * CLHEP::mm, fZ_mm * CLHEP::mm));
        // the above line is the correct version aligned with the center

        // the below lines are used for testing, forcing the beam to be offset
        //fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -5.0 * CLHEP::m));
        //double y_offset = 00.0 * CLHEP::mm; 
        //ParticleGun->SetParticlePosition(G4ThreeVector(fX_mm * CLHEP::mm, (fY_mm * CLHEP::mm) + y_offset, -5.0 * CLHEP::m));
        
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(fPx, fPy, fPz));

        fParticleGun->GeneratePrimaryVertex(event);
    }
    else if (fMode == "genie") 
    {
        // ==========================================
        //            GENIE EVENT FILE READER
        // ==========================================
        // load ROOT tree on first event
        if (!fFluxFile) {
            fFluxFile = new TFile(fFluxFileName.c_str(), "READ");
            if (!fFluxFile || fFluxFile->IsZombie()) {
                G4Exception("PrimaryGenerator", "NoGenieFile", FatalException, "Could not open GENIE ROOT file!");
            }
            
            fFluxTree = (TTree*)fFluxFile->Get("gst");
            if (!fFluxTree) {
                G4Exception("PrimaryGenerator", "NoTree", FatalException, "Could not find 'gst' tree in ROOT file!");
            }
            
            fFluxTree->SetBranchAddress("nf",   &fNF);
            fFluxTree->SetBranchAddress("pdgf", fPDG_arr);
            fFluxTree->SetBranchAddress("pxf",  fPx_arr);
            fFluxTree->SetBranchAddress("pyf",  fPy_arr);
            fFluxTree->SetBranchAddress("pzf",  fPz_arr);
            fFluxTree->SetBranchAddress("vtxx", &fVtxX);
            fFluxTree->SetBranchAddress("vtxy", &fVtxY);
            fFluxTree->SetBranchAddress("vtxz", &fVtxZ);
            
            fTotalEntries = fFluxTree->GetEntries();
        }

        int currentEntry = g_FluxEntryIndex.fetch_add(1);

        if (currentEntry >= fTotalEntries) {
            G4cout << "\n*** Ran out of GENIE events! Stopping run. ***\n" << G4endl;
            G4RunManager::GetRunManager()->AbortRun();
            return;
        }

        fFluxTree->GetEntry(currentEntry);

        // GENIE vertex coordinates are in meters
        G4ThreeVector vtxPos(fVtxX * CLHEP::m, fVtxY * CLHEP::m, fVtxZ * CLHEP::m);
        G4PrimaryVertex* vertex = new G4PrimaryVertex(vtxPos, 0.0 * CLHEP::ns);

        auto* particleTable = G4ParticleTable::GetParticleTable();

        // Attach all final-state particles to the primary vertex
        for (int i = 0; i < fNF; ++i) {
            int pdgCode = fPDG_arr[i];
            G4ParticleDefinition* partDef = particleTable->FindParticle(pdgCode);
            if (!partDef) continue;

            // GENIE momenta are in GeV
            G4double px = fPx_arr[i] * CLHEP::GeV;
            G4double py = fPy_arr[i] * CLHEP::GeV;
            G4double pz = fPz_arr[i] * CLHEP::GeV;

            G4PrimaryParticle* primary = new G4PrimaryParticle(partDef, px, py, pz);
            vertex->SetPrimary(primary);
        }

        event->AddPrimaryVertex(vertex);
    }
}
