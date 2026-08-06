#ifndef RUNACTION_H
#define RUNACTION_H

#include "G4UserRunAction.hh"
#include "TFile.h"
#include "TTree.h"
#include "globals.hh"
#include "G4ThreeVector.hh"  // Add this                                                                                                                                                                                             
#include <vector>
#include <string>
#include "EventAction.hh"

class EventAction;
class RunAction : public G4UserRunAction {
public:
    RunAction();
    ~RunAction() override;

  void FillEvent(EventAction* evt);
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

      TTree* GetTree() { return fTree; }

    // Variables for ROOT branches (non-static)                                                                                                                                                                                      
  double E, x, y, z,vertexX,vertexY,vertexZ, vertexT,eventWeight;
  double finalE, finalX, finalY, finalZ;
  Double_t px, py, pz;
  Double_t theta, phi, finalPhi,finalPhiDeg;

  Double_t totalEdep;
  Int_t nSteps;
  Int_t nSecondaries;
  double costh;

  double finalPx, finalPy, finalPz;
  double finalCosth;

  std::vector<std::string> stepProcessNames;
  std::vector<std::string> stepcreatorProcessNames;
  //  std::vector<std::string> secNames;
  std::vector<double> secEnergies;

   std::vector<double> secPx;
  std::vector<double> secPy;
  std::vector<double> secPz;
   std::vector<int> secPDG;

  std::vector<double> secStartX;
  std::vector<double> secStartY;
  std::vector<double> secStartZ;

  std::vector<double> secEndX;
  std::vector<double> secEndY;
  std::vector<double> secEndZ;
  //  std::string interactionType;
  //std::string interactionModel;

double trackWeight;
double nuEleTotXscBias;
  //double eventWeight;
  
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
  
  //  std::map<int, double> secTrackLength;
 std::vector<double> secWeights; 

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


int eventID;
int primaryPDG;
int particleType;  // 11=electron, -11=positron, 2212=proton, etc.
int primaryFinalPDG;
bool primaryOscillationProcessInvoked;
bool primaryOscillationFlavorChanged;
int primaryOscillationPDGBefore;
int primaryOscillationPDGAfter;
int nOscillationSteps;

std::string nuInteractionProcess;
std::vector<std::string> allInteractionProcess;
bool isCC;
bool isNC;

// ========== LEPTON PROPERTIES (NEUTRINO & ELECTRON SCATTERING) ==========
int outgoingLeptonPDG;
double outgoingLeptonE;
double outgoingLeptonPx;
double outgoingLeptonPy;
double outgoingLeptonPz;
double leptonScatteringAngle;  // theta_l in radians
double leptonCosTheta;         // cos(theta_l)
double inelasticity;           // y = 1 - E_l / E_nu

// ========== HADRONIC SYSTEM ==========
double outgoingHadronE;
double hadronicEnergy;         // Total E_had
double hadronicVisibleEnergy;  // Sum of visible hadron energies
int hadronicMultiplicity;      // Total number of hadrons

  
// ========== NEUTRINO KINEMATICS ==========
double q0;         // Energy transfer
double Q2;         // Four-momentum transfer squared
double W;          // Invariant mass
double xBj;        // Bjorken-x
double yBj;        // Inelasticity

// ========== SHOWER PROPERTIES (ELECTRONS) ==========
int showerNSecondaries;        // Number of shower secondaries
double showerLongitudinalProfile;
double showerLateralSpread;

// ========== PARTICLE PRODUCTION COUNTERS ==========
int nPhotonsProduced;
int nElectronsProduced;
int nPositronsProduced;
int nGammasProduced;
int nPionsProduced;
int nProtonSecondaries;
int nNeutronSecondaries;
int nKaonsProduced;

  
std::string interactionType;
std::string interactionModel;
std::vector<double> step_stepLength;
  std::vector<double> step_time;
std::vector<double> step_preMomX;
std::vector<double> step_preMomY;
std::vector<double> step_preMomZ;

std::vector<double> step_postMomX;
std::vector<double> step_postMomY;
std::vector<double> step_postMomZ;

  // Step-level info                                                                                                                                                                                                                 
  std::vector<int> step_trackID, step_parentID, step_PDG;
    std::vector<double> step_preX, step_preY, step_preZ;
    std::vector<double> step_postX, step_postY, step_postZ;
    std::vector<double> step_kinE, step_edep;
    std::vector<std::string> step_proc;
   std::vector<std::string> step_creatorproc;
  //  std::vector<double> trk_birthPosX;
  //std::vector<double> trk_birthPosY;
  //std::vector<double> trk_birthPosZ;
  //std::vector<double> trk_birthKE;

private:
 EventAction* fEventAction;
  TFile* fFile;
  TTree* fTree;
};

#endif




