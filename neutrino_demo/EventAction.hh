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

    //    steps.push_back({trackID, prePos, postPos, kineticE, edep, processName});                                                                                                                                                  
    //}
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


  void AddEdep(double edep) { totalEdep_ += edep; }
  void AddSecondaryE(double e) { totalSecondaryE += e; }

  void IncrementStep() { nSteps_++; }

  G4bool neutrinoInteractionPrinted = false;
  double totalEdep_;
  int nSteps_;
  double totalSecondaryE;
  int nSecondaries;
  //G4ThreeVector vertex;                                                                                                                                                                                                            
  //  std::vector<std::string> secNames;                                                                                                                                                                                             
  //  std::vector<double> secEnergies;                                                                                                                                                                                               
  std::string interactionType;
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
  
private:
    RunAction* fRunAction;
 // Per-event accumulators                                                                                                                                                                                                           
  //    double totalEdep_ = 0;                                                                                                                                                                                                       
  //  int nSteps_ = 0;                                                                                                                                                                                                               
};

#endif



