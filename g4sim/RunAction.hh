#ifndef RUNACTION_H
#define RUNACTION_H

#include "G4UserRunAction.hh"
#include "TFile.h"
#include "TTree.h"

class RunAction : public G4UserRunAction {
public:
    RunAction();
    ~RunAction() override;

     void BeginOfRunAction(const G4Run*) override;
     void EndOfRunAction(const G4Run*) override;

      TTree* GetTree() { return fTree; }

    // Variables for ROOT branches (non-static)
    double E, x, y, z;
    double finalE, finalX, finalY, finalZ;
  Double_t px, py, pz;
  Double_t theta, phi, finalPhi,finalPhiDeg;
;
  Double_t totalEdep;
  Int_t nSteps;
  Int_t nSecondaries;
  double costh;  

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

double secFirstZ;
double secLastZ;
double secFirstX;
double secLastX;
double secFirstY;
double secLastY;

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
  
private:
  TFile* fFile;
  TTree* fTree;
};

#endif
