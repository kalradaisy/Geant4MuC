#ifndef EVENTACTION_H
#define EVENTACTION_H

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include <vector>
#include <string>
#include "G4String.hh"


class EventAction : public G4UserEventAction {
public:
    EventAction();
    ~EventAction() override;

    void BeginOfEventAction(const G4Event*) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEdep(double edep) { totalEdep += edep; }
    void IncrementStep() { nSteps++; }
    /* Updated Setter methods below*/
    // Process & Particle Counters
    void AddNuInteraction() { fNuInteractions++; }
    void AddCompton()       { nCompton++; }
    void AddPairProd()      { nPairProd++; }
    void AddIonisation()    { nIonisation++; }
    void AddBrem()          { nBremsstrahlung++; }
    void AddPhotoEl()       { nPhotoElectric++; }
    void AddAnnihilation()  { nAnnihilation++; }
    void AddDecay()         { nDecay++; }
    void AddSecondary()     { nSecondaries++; }
    void AddSecE(double Esec)     { secTotalE += Esec; }

    void SetTargetZ(int Z)     { targetZ = Z; }
    void SetTargetA(int A)     { targetA = A; }
    void SetTargetPDG(int PDG_val)     { targetPDG = PDG_val; }

    void AddSecStartPos(const G4ThreeVector& pos);
    void AddSecEndPos(const G4ThreeVector& pos);

    
    // Track length accumulator
    void AddSecTrackLength(double len) { secTrackLength += len; }

    void CountBackTracks(const G4ThreeVector& p){
        if(p.z() < 0) nBackward++;
    }

    double ReadE() {return E;}

    // Initial Kinematics Setters
    void SetInitialKinematics(double e, double p_x, double p_y, double p_z, double th, double ph, double c_th) {
        E = e; px = p_x; py = p_y; pz = p_z; theta = th; phi = ph; costh = c_th;
    }
    void SetInitialKinematics(double e, const G4ThreeVector& pos, const G4ThreeVector& p) {
        E = e; 
        px = pos.x(); 
        py = pos.y(); 
        pz = pos.z();
        theta = p.theta();
        phi = p.phi();
        costh = p.cosTheta();
    }

    // Final Kinematics Setters
    void SetFinalKinematics(double e, const G4ThreeVector& pos) {
        finalE = e; 
        finalX = pos.x(); 
        finalY = pos.y(); 
        finalZ = pos.z();
    }
    void SetFinalMomentum(const G4ThreeVector& p) {
        finalPx = p.x(); 
        finalPy = p.y(); 
        finalPz = p.z();
        finalCosth = p.cosTheta();
        finalPhi = p.phi();
        finalPhiDeg = finalPhi * 180.0 / CLHEP::pi;
    }

    void AddGamma() { nGamma++; }       // gamma
    void AddElectron() { nElectron++; }   // e-
    void AddPositron() { nPositron++; }   // e+
    void AddProtonSec() { nProtonSec++; }  // proton
    void AddNeutron() { nNeutron++; }    // neutron
    void AddPionPlus() { nPionPlus++; }   // pi+
    void AddPionMinus() { nPionMinus++; }  // pi-
    void AddPionZero() { nPionZero++; }   // pi0
    void AddMuonMinus() { nMuonMinus++; }  // mu-
    void AddTauPlus() { nTauPlus++; } // tau+
    void AddTauMinus() { nTauMinus++; }	// tau-
    void AddKaonPlus() { nKaonPlus++; } // kaon+
    void AddKaonMinus() { nKaonMinus++; } // kaon-
    void AddKaonZero() { nKaonZero++; } // kaon0
    void AddKaonZeroL() { nKaonZeroL++; } // kaon0 Long
    void AddKaonZeroS() { nKaonZeroS++; } // kaon0 short
    void AddMuonPlus() { nMuonPlus++; } // mu+

    // Quick getter so SteppingAction can check if E is 0
    double GetInitialE() const { return E; }

    G4bool neutrinoInteractionPrinted = false; //idk where this is used

    double E, x, y, z, costh;
    double vertexX, vertexY, vertexZ, vertexT;
    G4String interactionType;
    G4String interactionModel;
    G4int eventID;
    G4int primaryPDG;
    G4int particleType;
    G4int primaryFinalPDG;

    // Store bools as ints, we have to to use G4AnalysisManager. It's all good
    G4int primaryOscillationProcessInvoked;
    G4int primaryOscillationFlavorChanged;
    
    G4int primaryOscillationPDGBefore;
    G4int primaryOscillationPDGAfter;
    G4int nOscillationSteps;

    G4String nuInteractionProcess;
    
    G4int isCC;
    G4int isNC;
    
    G4int outgoingLeptonPDG;
    G4double outgoingLeptonE;
    G4double outgoingHadronE;
    
    G4double outgoingLeptonPx;
    G4double outgoingLeptonPy;
    G4double outgoingLeptonPz;

    G4bool decisionMade;

    G4double q0;
    G4double Q2;
    G4double W;
    G4double xBj;
    G4double yBj;
    
    // Validation & Lepton Kinematics
    G4double leptonScatteringAngle;
    G4double leptonCosTheta;
    G4double inelasticity;
    
    // Shower Properties
    G4int showerNSecondaries;

    // Biased interaction weight information, native to geant
    G4double eventWeight;
  
private:
    // --------------------------------------------------------
    // NEW: We moved these from RunAction to EventAction!
    // --------------------------------------------------------
    double finalE, finalX, finalY, finalZ;
    double px, py, pz;
    double theta, phi, finalPhi, finalPhiDeg;

    double totalEdep;
    int nSteps;
    int nSecondaries;

    double finalPx, finalPy, finalPz;
    double finalCosth;

    // Secondary info

    int targetZ = -1;
    int targetA = -1;
    int targetPDG = -1;   // optional

    int nGamma;
    int nElectron;
    int nPositron;

    double secTotalE;
    double secMeanE;
    double secTrackLength;

    int nBackward;

    // Process counters (optional but recommended)
    int nCompton =0 ;
    int nPairProd=0;
    int nIonisation=0;
    int nBremsstrahlung=0;
    int nDecay=0;
    int nProtonSec=0;
    int nNeutron=0;
    int nPionPlus=0;
    int nPionMinus=0;
    int nMuonPlus=0;
    int nMuonMinus=0;
    int nTauPlus=0;
    int nTauMinus=0;
    int nPionZero=0;
    int nPhotoElectric   = 0;
    int nAnnihilation    = 0;
    int nKaonPlus = 0;
    int nKaonMinus=0;
    int nKaonZero=0;
    int nKaonZeroL=0;
    int nKaonZeroS=0;
    int fNuInteractions = 0;

};


#endif
