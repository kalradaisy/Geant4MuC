#include "EventAction.hh"
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "RunAction.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "CLHEP/Units/PhysicalConstants.h"

EventAction::EventAction(RunAction* runAction)
  : fRunAction(runAction),
    totalEdep_(0),
    nSteps_(0)
    //    trackWeight(1.0),
    //nuEleTotXscBias(1.0e9),
    //eventWeight(1.0e-9)
{}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event)
{
  //  secTotalE = 0;
  // secMeanE = 0;
  secTrackLength = 0;


  // ========== BASIC EVENT INITIALIZATION ==========
  eventID = event->GetEventID();
  primaryPDG = 0;
  particleType = 0;
  nuInteractionProcess = "None";
  allInteractionProcess.clear();
  
  // ========== INTERACTION FLAGS ==========
  isCC = false;
  isNC = false;
  decisionMade = false;
  hasCCLepton = false;
  hasOutNeutrino = false;
  hasHadron = false;
  foundOutgoingNeutrino = false;
  foundOtherNonNeutrino = false;
  
  foundCC = false;
  foundNC = false;
  foundCCLepton = false;
  foundOutNeutrino = false;
  foundHadron = false;
  foundLepton = false;
  // ========== LEPTON PROPERTIES (NEUTRINO CC) ==========
  outgoingLeptonPDG = 0;
  outgoingLeptonE = 0.0;
  outgoingLeptonPx = 0.0;
  outgoingLeptonPy = 0.0;
  outgoingLeptonPz = 0.0;
  leptonScatteringAngle = 0.0;
  leptonCosTheta = 1.0;
  inelasticity = 0.0;
  
  // ========== KINEMATICS ==========
  q0 = 0.0;
  Q2 = 0.0;
  W = 0.0;
  xBj = 0.0;
  yBj = 0.0;
  
  // ========== SHOWER PROPERTIES ==========
  showerLongitudinalProfile = 0.0;
  showerLateralSpread = 0.0;
  showerNSecondaries = 0;
  
  // ========== PROCESS COUNTERS ==========
  interactionProcesses.clear();
  nBremstrahlungProcesses = 0;
  nIonizationProcesses = 0;
  nComptonScatters = 0;
  nPairProductions = 0;
  nPhotoElectricEvents = 0;

  /*
  // ========== PARTICLE PRODUCTION ==========
  nGammasProduced = 0;
  nElectronsProduced = 0;
  nPositronsProduced = 0;
  nPhotonsProduced = 0;
  nPionsProduced = 0;
  nProtonSecondaries = 0;
  nNeutronSecondaries = 0;
  nKaonsProduced = 0;
  */
  
  
  // ========== VERTEX AND PROPAGATION ==========
  vertexX = 0.0;
  vertexY = 0.0;
  vertexZ = 0.0;
  vertexT = 0.0;
  eventWeight = 0.0;
  
  transverseVertexDistance = 0.0;
  vertexDepth = 0.0;
  interactionRecorded = false;
  
  // ========== RADIATION/INTERACTION LENGTHS ==========
  radiationLengthTraversed = 0.0;
  interactionLengthTraversed = 0.0;
  
  // ========== PRIMARY PARTICLE PROPERTIES ==========
  E = 0.0;
  x = 0.0;
  y = 0.0;
  z = 0.0;
  px = 0.0;
  py = 0.0;
  pz = 0.0;
  theta = 0.0;
  phi = 0.0;
  costh = 0.0;
  
  // ========== FINAL STATE ==========
  finalE = 0.0;
  finalX = 0.0;
  finalY = 0.0;
  finalZ = 0.0;
  finalPx = 0.0;
  finalPy = 0.0;
  finalPz = 0.0;
  finalCosth = 0.0;
  finalPhi = 0.0;
  finalPhiDeg = 0.0;
  primaryFinalPDG = 0;
  
  // ========== OSCILLATIONS ==========
  primaryOscillationProcessInvoked = false;
  primaryOscillationFlavorChanged = false;
  primaryOscillationPDGBefore = 0;
  primaryOscillationPDGAfter = 0;
  nOscillationSteps = 0;
  
  // ========== WEIGHTS ==========
  trackWeight = 1.0;
  nuEleTotXscBias = 1.0e9;
  eventWeight = trackWeight / nuEleTotXscBias;
  
  // ========== ENERGY ACCOUNTING ==========
  totalSecondaryE = 0.0;
  totalEdep_ = 0.0;
  nSecondaries = 0;
  nSteps_ = 0;
  
  // ========== INTERACTION TYPE ==========
  interactionType = "None";
  interactionModel = "None";
  neutrinoInteractionPrinted = false;
  primaryFinalPDG = 0;
  finalStatePDG.clear();
  fRunAction->nSteps = 0;
  fRunAction->nSecondaries = 0;
  
  steps.clear();
  secondaries.clear();
  secEnergies.clear();
  secPDG.clear();
  secTrackID.clear();
  secParentID.clear();
  
  secStartX.clear();
  secStartY.clear();
  secStartZ.clear();
  
  secEndX.clear();
  secEndY.clear();
  secEndZ.clear();
  
  stepProcessNames.clear();
  stepcreatorProcessNames.clear();
  
  // ========== SECONDARY COUNTERS RESET ==========
  fRunAction->nGamma = 0;
  fRunAction->nElectron = 0;
  fRunAction->nPositron = 0;
  fRunAction->nProtonSec = 0;
  fRunAction->nNeutron = 0;
  fRunAction->nPionPlus = 0;
  fRunAction->nPionMinus = 0;
  fRunAction->nPionZero = 0;
  fRunAction->nMuonMinus = 0;
  fRunAction->nMuonPlus = 0;
  fRunAction->nTauMinus = 0;
  fRunAction->nTauPlus = 0;
  fRunAction->nBackward = 0;
  fRunAction->nDecay = 0;
  fRunAction->nBremsstrahlung = 0;
  fRunAction->nPhotoElectric = 0;
  fRunAction->nAnnihilation = 0;
  fRunAction->nCompton = 0;
  fRunAction->nPairProd = 0;
  fRunAction->nIonisation = 0;
  fRunAction->nKaonPlus = 0;
  fRunAction->nKaonMinus = 0;
  fRunAction->nKaonZero = 0;
  fRunAction->nKaonZeroL = 0;
  fRunAction->nKaonZeroS = 0;
  fRunAction->targetZ = -1;
  fRunAction->targetA = -1;
  fRunAction->targetPDG = -1;

   fRunAction->secWeights.clear();
  
  // Get primary particle info
  auto vertex = event->GetPrimaryVertex(0);
  auto primary = vertex->GetPrimary(0);
  
  //primaryNuPDG = primary->GetPDGcode();
  primaryPDG = primary->GetPDGcode();;
  
}

void EventAction::AddStepInfo(int trackID, int parentID,
                              int PDG,
                              const G4ThreeVector& prePos,
                              const G4ThreeVector& postPos,
                              const G4ThreeVector& preMom,
                              const G4ThreeVector& postMom, double kinE,
                              double edep, double globalTime,
                              double stepLength,
                              const std::string& processName,const std::string& creatorprocessName) //,  G4ThreeVector birthPos,
//G4double birthKE)
{
    StepInfo s;
    s.trackID = trackID;
    s.parentID =  parentID;
    s.PDG = PDG;
    s.prePos = prePos;
    s.postPos = postPos;
    s.preMom = preMom;
    s.postMom = postMom;
    s.kineticE = kinE;
    s.edep = edep;
    s.globalTime = globalTime;
    s.stepLength = stepLength;
    s.processName = processName;
    s.creatorprocessName = creatorprocessName;
    //  s.birthPos = birthPos;
    //s.birthKE = birthKE;

    steps.push_back(s);   // steps is a std::vector<StepInfo>                                                                                                                                                                        
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  if(!fRunAction) return;
  
  const double nucleonMass = 939.565 * CLHEP::MeV;
  const double electronMass = 0.511 * CLHEP::MeV;
  int primaryAbsPDG = std::abs(primaryPDG);
  
   int nuPDG;
  int expectedLepton = 0;
  // ========== NEUTRINO INTERACTION ANALYSIS ==========
  if (abs(primaryPDG) == 12 || abs(primaryPDG) == 14 || abs(primaryPDG) == 16){
    nuPDG = primaryPDG;
    if (std::abs(nuPDG) == 12 || std::abs(nuPDG) == 14 || std::abs(nuPDG) == 16) {
      expectedLepton = nuPDG - (nuPDG > 0 ? 1 : -1);
    }
  }
    
  int nPions = 0;
  int nNucleons = 0;
  /*
 
  // ========== NEUTRINO KINEMATICS (CC events) ==========
  if (E > 0.0) {
    double Enu = E;
    G4ThreeVector pNu(px, py, pz);
    double pNuMag = pNu.mag();

    double Elep = outgoingLeptonE;
    G4ThreeVector pLep(outgoingLeptonPx, outgoingLeptonPy, outgoingLeptonPz);
    double pLepMag = pLep.mag();

    double qEnergy = Enu - Elep;
    G4ThreeVector qVec = pNu - pLep;

    q0 = qEnergy;
    Q2 = qVec.mag2() - qEnergy * qEnergy;

    // ========== INVARIANT MASS (W) ==========
    double W2 = nucleonMass * nucleonMass + 2.0 * nucleonMass * qEnergy - Q2;
    W = (W2 > 0.0) ? std::sqrt(W2) : 0.0;

    // ========== BJORKEN-X ==========
    xBj = (2.0 * nucleonMass * qEnergy > 0.0)
      ? Q2 / (2.0 * nucleonMass * qEnergy)
      : 0.0;

    // ========== INELASTICITY (Y) ==========
    yBj = (Enu > 0.0) ? qEnergy / Enu : 0.0;
    inelasticity = (Enu > 0.0) ? (1.0 - Elep / Enu) : 0.0;
    
    // ========== LEPTON SCATTERING ANGLE ==========
    if (pNuMag > 0 && pLepMag > 0) {
      double cosThetaLep = pNu.dot(pLep) / (pNuMag * pLepMag);
      // Clamp to valid range [-1, 1]
      cosThetaLep = std::max(-1.0, std::min(1.0, cosThetaLep));
      leptonCosTheta = cosThetaLep;
      leptonScatteringAngle = std::acos(cosThetaLep);
    } else {
      leptonCosTheta = 1.0;
      leptonScatteringAngle = 0.0;
    }
    
   else {
    q0 = 0.0;
    Q2 = 0.0;
    W = 0.0;
    xBj = 0.0;
    yBj = 0.0;
    inelasticity = 0.0;
    leptonCosTheta = 1.0;
    leptonScatteringAngle = 0.0;
  } 
  }
  */
  
  // ========== FILL RUNACTION DATA ==========
  fRunAction->eventID = eventID;
  fRunAction->primaryPDG = primaryPDG;
  //  fRunAction->particleType = particleType;
  
  fRunAction->nuInteractionProcess = nuInteractionProcess;
  fRunAction->allInteractionProcess = allInteractionProcess;
  fRunAction->secTrackLength = secTrackLength;
  
  /*fRunAction->q0 = q0;
  fRunAction->Q2 = Q2;
  fRunAction->W = W;
  fRunAction->xBj = xBj;
  fRunAction->yBj = yBj;

  
  fRunAction->leptonScatteringAngle = leptonScatteringAngle;
  fRunAction->leptonCosTheta = leptonCosTheta;
  fRunAction->inelasticity = inelasticity;
  */
  
  
  fRunAction->nSecondaries = nSecondaries;
  /*  fRunAction->showerNSecondaries = showerNSecondaries;
  fRunAction->nPhotonsProduced = nPhotonsProduced;
  fRunAction->nElectronsProduced = nElectronsProduced;
  fRunAction->nPositronsProduced = nPositronsProduced;
  fRunAction->nGammasProduced = nGammasProduced;
  fRunAction->nPionsProduced = nPionsProduced;
  fRunAction->nProtonSecondaries = nProtonSecondaries;
  fRunAction->nNeutronSecondaries = nNeutronSecondaries;
  fRunAction->nKaonsProduced = nKaonsProduced;
  */
  fRunAction->E = E;
  fRunAction->x = x;
  fRunAction->y = y;
  fRunAction->z = z;
  
  fRunAction->px = px;
  fRunAction->py = py;
  fRunAction->pz = pz;
  
  fRunAction->vertexX = vertexX;
  fRunAction->vertexY = vertexY;
  fRunAction->vertexZ = vertexZ;
  fRunAction->vertexT = vertexT;
  fRunAction->eventWeight = eventWeight;
  
  fRunAction->finalE = finalE;
  fRunAction->finalX = finalX;
  fRunAction->finalY = finalY;
  fRunAction->finalZ = finalZ;
  fRunAction->finalPx = finalPx;
  fRunAction->finalPy = finalPy;
  fRunAction->finalPz = finalPz;
  
  fRunAction->costh = costh;
  fRunAction->finalCosth = finalCosth;
  fRunAction->finalPhiDeg = finalPhiDeg;
  
  fRunAction->totalEdep = totalEdep_;
  fRunAction->secTotalE = totalSecondaryE;

  fRunAction->primaryFinalPDG = primaryFinalPDG;
  fRunAction->primaryOscillationProcessInvoked = primaryOscillationProcessInvoked;
  fRunAction->primaryOscillationFlavorChanged = primaryOscillationFlavorChanged;
  fRunAction->primaryOscillationPDGBefore = primaryOscillationPDGBefore;
  fRunAction->primaryOscillationPDGAfter = primaryOscillationPDGAfter;
  fRunAction->nOscillationSteps = nOscillationSteps;

  fRunAction->secEnergies = secEnergies;
  fRunAction->secStartX = secStartX;
  fRunAction->secStartY = secStartY;
  fRunAction->secStartZ = secStartZ;
  fRunAction->secEndX = secEndX;
  fRunAction->secEndY = secEndY;
  fRunAction->secEndZ = secEndZ;

  fRunAction->secPx = secPx;
  fRunAction->secPy = secPy;
  fRunAction->secPz = secPz;
  
  fRunAction->nSteps = nSteps_;
  fRunAction->nSecondaries = nSecondaries;
  fRunAction->FillEvent(this);
  

  // Fill tree ONCE per event
  fRunAction->GetTree()->Fill();
}
