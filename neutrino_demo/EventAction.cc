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
{}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event)
{
   if(!this) return;
eventID = event->GetEventID();

primaryPDG = 0;
nuInteractionProcess = "None";

isCC = false;
isNC = false;

outgoingLeptonPDG = 0;
outgoingLeptonE = 0.0;
outgoingLeptonPx = 0.0;
outgoingLeptonPy = 0.0;
outgoingLeptonPz = 0.0;

q0 = 0.0;
Q2 = 0.0;
W = 0.0;
xBj = 0.0;
yBj = 0.0;

    E=0; x=0; y=0; z=0; px=0; py=0; pz=0;
    finalX=0; finalY=0; finalZ=0;
    theta=0; phi=0; costh=0;
    //secTotalE=0;                                                                                                                                                                                                                   
    finalCosth =0; finalPhi=0; finalPhiDeg=0;
    vertexX=0; vertexY=0,vertexZ=0, vertexT=0;
    interactionRecorded = false;
    totalSecondaryE = 0.0;
    nSecondaries = 0;
    fRunAction->nSteps = 0; fRunAction->nSecondaries = 0;
    totalEdep_ = 0;
    nSteps_ = 0;
    neutrinoInteractionPrinted = false;
    interactionType = "None";
    finalStatePDG.clear();
    
    steps.clear();
    secondaries.clear();
    //secNames.clear();
    secEnergies.clear();

    secStartX.clear();
    secStartY.clear();
    secStartZ.clear();

    secEndX.clear();
    secEndY.clear();
    secEndZ.clear();

    // Secondary counters                                                                                                                                                                                                            

    fRunAction->nGamma = 0;
    fRunAction->nElectron = 0;
    fRunAction->nPositron = 0;
    fRunAction->nProtonSec = 0;
    fRunAction->nNeutron = 0;
    fRunAction->nPionPlus = 0;
    fRunAction->nPionMinus = 0;
    fRunAction->nPionZero = 0;
fRunAction->secMeanE = 0.0;
fRunAction->secTrackLength = 0.0;


fRunAction->nBackward = 0;
fRunAction->nDecay = 0;
fRunAction->nBremsstrahlung = 0;
fRunAction->nPhotoElectric = 0;
fRunAction->nAnnihilation = 0;
fRunAction->nKaonPlus = 0;
fRunAction->nKaonMinus = 0;
fRunAction->nKaonZero = 0;
fRunAction->nKaonZeroL = 0;
fRunAction->nKaonZeroS = 0;
fRunAction->targetZ = -1;
fRunAction->targetA = -1;
fRunAction->targetPDG = -1;

// Process counters                                                                                                                                                                                                                  
fRunAction->nCompton = 0;
fRunAction->nPairProd = 0;
fRunAction->nIonisation = 0;
auto vertex = event->GetPrimaryVertex(0);
auto primary = vertex->GetPrimary(0);

primaryNuPDG = primary->GetPDGcode();
primaryPDG = primaryNuPDG;

}

void EventAction::AddStepInfo(int trackID, int parentID,
                              int PDG,
                              const G4ThreeVector& prePos,
                              const G4ThreeVector& postPos,
                              const G4ThreeVector& preMom,
                              const G4ThreeVector& postMom, double kinE,
                              double edep, double globalTime,
                              double stepLength,
                              const std::string& processName,const std::string& creatorprocessName,  G4ThreeVector birthPos,
                              G4double birthKE)
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
    s.birthPos = birthPos;
    s.birthKE = birthKE;

    steps.push_back(s);   // steps is a std::vector<StepInfo>                                                                                                                                                                        
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    if(!fRunAction) return;

    const double nucleonMass = 939.565 * CLHEP::MeV;

if(isCC && outgoingLeptonPDG != 0 && E > 0.0) {
    double Enu = E;
    G4ThreeVector pNu(px, py, pz);

    double Elep = outgoingLeptonE;
    G4ThreeVector pLep(outgoingLeptonPx, outgoingLeptonPy, outgoingLeptonPz);

    double qEnergy = Enu - Elep;
    G4ThreeVector qVec = pNu - pLep;

    q0 = qEnergy;
    Q2 = qVec.mag2() - qEnergy*qEnergy;

    double W2 = nucleonMass*nucleonMass + 2.0*nucleonMass*qEnergy - Q2;
    W = (W2 > 0.0) ? std::sqrt(W2) : 0.0;

    xBj = (2.0*nucleonMass*qEnergy > 0.0)
        ? Q2 / (2.0*nucleonMass*qEnergy)
        : 0.0;

    yBj = qEnergy / Enu;
}

fRunAction->eventID = eventID;
fRunAction->primaryPDG = primaryPDG;

fRunAction->nuInteractionProcess = nuInteractionProcess;

fRunAction->isCC = isCC;
fRunAction->isNC = isNC;

fRunAction->outgoingLeptonPDG = outgoingLeptonPDG;
fRunAction->outgoingLeptonE = outgoingLeptonE;
fRunAction->outgoingLeptonPx = outgoingLeptonPx;
fRunAction->outgoingLeptonPy = outgoingLeptonPy;
fRunAction->outgoingLeptonPz = outgoingLeptonPz;

fRunAction->q0 = q0;
fRunAction->Q2 = Q2;
fRunAction->W = W;
fRunAction->xBj = xBj;
fRunAction->yBj = yBj;

fRunAction->nSecondaries = nSecondaries;

    
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
    int nuPDG = primaryNuPDG;
int expectedLepton = 0;

if(std::abs(nuPDG) == 12) expectedLepton = (nuPDG > 0) ? 11 : -11;
if(std::abs(nuPDG) == 14) expectedLepton = (nuPDG > 0) ? 13 : -13;
if(std::abs(nuPDG) == 16) expectedLepton = (nuPDG > 0) ? 15 : -15;

bool isCC = false;

for(int pdg : finalStatePDG) {
    if(pdg == expectedLepton) {
        isCC = true;
        break;
    }
}


    interactionType = isCC ? "CC" : "NC";
    fRunAction->interactionType = interactionType;
    //fRunAction->secNames = secNames;
    fRunAction->secEnergies = secEnergies;
    fRunAction->secStartX = secStartX;
    fRunAction->secStartY = secStartY;
    fRunAction->secStartZ = secStartZ;
    fRunAction->secEndX = secEndX;
    fRunAction->secEndY = secEndY;
    fRunAction->secEndZ = secEndZ;

    //fRunAction->totalEdep = totalEdep_;                                                                                                                                                                                            
    fRunAction->nSteps = nSteps_;
    fRunAction->nSecondaries = nSecondaries;
  fRunAction->FillEvent(this);

    // Fill ONCE                                                                                                                                                                                                                     
    fRunAction->GetTree()->Fill();
}




  
