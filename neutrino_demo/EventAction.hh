#ifndef EVENTACTION_H
#define EVENTACTION_H

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "RunAction.hh"       // if you use RunAction inside EventAction                                                                                                                                                             
#include "globals.hh"
#include "G4ThreeVector.hh"  // Add this                                                                                                                                                                                             
#include <vector>
#include <string>
class RunAction;
// Structs for event display                                                                                                                                                                                                         
// -----------------------------                                                                                                                                                                                                     
struct StepInfo {
  int trackID;
  int parentID;
  int PDG;
  G4ThreeVector prePos;
  G4ThreeVector postPos;
  G4ThreeVector preMom;
  G4ThreeVector postMom;
  double kineticE;
  double edep;
  double globalTime;
  double stepLength;
  std::string processName;
  std::string creatorprocessName;
  G4ThreeVector birthPos;
  G4double birthKE;
};

struct SecondaryInfo {
    int trackID;
    int parentID;
    double energy;
    std::string name;
};
struct TrackInfo {
    int trackID;
    int parentID;
    int PDG;
    G4ThreeVector startPos;
    G4ThreeVector endPos;
    double startE;
    double endE;
    G4ThreeVector startMom;
    G4ThreeVector endMom;
    std::string name;
    std::string creatorProcess;
};

class EventAction : public G4UserEventAction {
public:
    EventAction(RunAction* runAction);   // pass pointer to RunAction if needed                                                                                                                                                      
    ~EventAction() override;

    // Override correct function                                                                                                                                                                                                     
  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event* event) override;

  int eventID;
int primaryPDG;

std::string nuInteractionProcess;
  
std::vector<std::string> allInteractionProcess;
bool isCC;
bool isNC;

  int outgoingLeptonPDG;
double outgoingLeptonE;
double outgoingHadronE;
  bool decisionMade = false;
  bool hasCCLepton = false;
  bool hasOutNeutrino = false;
  bool hasHadron = false;
  double outgoingLeptonPx;
double outgoingLeptonPy;
double outgoingLeptonPz;

double q0;
double Q2;
double W;
double xBj;
double yBj;
double trackWeight;
double nuEleTotXscBias;
double eventWeight;

  void SetNeutrinoWeight(double trackWeightIn, double biasFactorIn);
int primaryTrackID;
  int PDG;
  std::vector<StepInfo> steps;
   // Step info                                                                                                                                                                                                                      
    // -----------------------------                                                                                                                                                                                                 
  void AddStepInfo(int trackID, int parentID,
                   int PDG, const G4ThreeVector& prePos,
                   const G4ThreeVector& postPos, const G4ThreeVector& preMom,
                 const G4ThreeVector& postMom, double kineticE,
                   double edep,double globalTime,
                 double stepLength, const std::string& processName, const std::string& creatorprocessName,G4ThreeVector birthPos,
                   G4double birthKE);

  //to reconstruct CC vs NC
  std::vector<int> finalStatePDG;
  int primaryNuPDG = 0;
  //  std::string interactionType = "Unknown";

  std::vector<SecondaryInfo> secondaries; // all secondaries                                                                                                                                                                         

    // optional flattened step info if you want                                                                                                                                                                                      
  std::vector<int> secTrackID;
  std::vector<int> secParentID;
  std::vector<double> secEnergies;
  std::vector<std::string> secNames;
  std::vector<int> secPDG;
  std::vector<double> step_preX, step_preY, step_preZ;
  std::vector<double> step_postX, step_postY, step_postZ;
  std::vector<double> step_kinE, step_edep;
  std::vector<std::string> stepProcessNames;
    std::vector<std::string> stepcreatorProcessNames;

  int primaryFinalPDG = 0;
  bool primaryOscillationProcessInvoked = false;
  bool primaryOscillationFlavorChanged = false;
  int primaryOscillationPDGBefore = 0;
  int primaryOscillationPDGAfter = 0;
  int nOscillationSteps = 0;

  void AddEdep(double edep) { totalEdep_ += edep; }
  void AddSecondaryE(double e) { totalSecondaryE += e; }

  void IncrementStep() { nSteps_++; }

  G4bool neutrinoInteractionPrinted = false;
  double totalEdep_;
  int nSteps_;
  double totalSecondaryE;
  int nSecondaries;
  std::string interactionType;
  std::string interactionModel;
  std::vector<double> secStartX;
  std::vector<double> secStartY;
  std::vector<double> secStartZ;

  std::vector<double> secEndX;
  std::vector<double> secEndY;
  std::vector<double> secEndZ;

   double E, x, y, z;
  double px, py, pz;
  double theta, phi, costh;

  double vertexX, vertexY, vertexZ, vertexT;
  bool interactionRecorded;

  double finalE, finalX, finalY, finalZ;
  double finalPx, finalPy, finalPz;
  double finalTheta, finalPhi, finalCosth, finalPhiDeg;

 // ========== UNIVERSAL VALIDATION VARIABLES ==========
  // Applicable to electrons, protons, and neutrinos
  
  // Particle classification
  int particleType; // 11=electron, -11=positron, 2212=proton, 2112=neutron, neutrino=12/14/16

  // Lepton scattering angle (for neutrino CC and electron scattering)
  double leptonScatteringAngle;      // theta_l in radians
  double leptonCosTheta;             // cos(theta_l)
  
  // Inelasticity (y) - applicable to neutrino interactions
  double inelasticity;               // y = 1 - E_l / E_nu
  
// Shower properties (electrons, photons, protons)
    double showerLongitudinalProfile;  // For depth analysis
  double showerLateralSpread; // For Moliere radius
  int showerNSecondaries;            // Shower secondary count
  
  // Process analysis
  std::vector<std::string> interactionProcesses;  // All processes in event
  int nBremstrahlungProcesses;       // For electron showers
  int nIonizationProcesses;
  int nComptonScatters;
  int nPairProductions;
  int nPhotoElectricEvents;
  // Particle-specific counters
  int nGammasProduced;               // From brem, pair production, pion decay
  int nElectronsProduced;            // From pair production, ionization
  int nPositronsProduced;            // From pair production
  int nPhotonsProduced;              // All photons
  
  // Hadronic interaction analysis (protons, neutrinos)
  int nPionsProduced;
  int nProtonSecondaries;
  int nNeutronSecondaries;
  int nKaonsProduced;
  //  double meanSecondaryEnergy;        // Average energy per secondary
  //double maxSecondaryEnergy;         // Highest secondary energy
  
  // Vertex and propagation
  double transverseVertexDistance;   // sqrt(x_v^2 + y_v^2)
  double vertexDepth;                // z_v (interaction depth)
  
  // Radiation length and interaction length tracking
  double radiationLengthTraversed;
  double interactionLengthTraversed;

  private:
    RunAction* fRunAction;
 // Per-event accumulators                                                                                                                                                                                                           
  //    double totalEdep_ = 0;                                                                                                                                                                                                       
  //  int nSteps_ = 0;                                                                                                                                                                                                               
};

#endif

