# G4TargetPractice

A general-purpose Geant4 simulation tool that lets you run particle transport through any detector geometry described in GDML format, driven entirely by a Geant4 macro file. No local Geant4 installation required — just Docker.

## Quick Start with Docker

Pre-built images are available at:
**`ghcr.io/lawrenceleejr/g4targetpractice`**

These images include Geant4 (with GDML and ROOT support) and the compiled `g4sim` executable. They are built, tested, and deployed automatically via GitHub Actions.

### What you need

1. **Docker** (or Podman) installed and running
2. A **GDML file** describing your detector geometry
3. A **Geant4 macro file** (`.mac`) configuring your particle gun and run settings

### Running a simulation

Place your GDML file and macro file in a local directory (e.g. `myrun/`), then run:

```bash
docker run --rm -it \
  -v $PWD/myrun/:/run/ \
  -w /run/ \
  ghcr.io/lawrenceleejr/g4targetpractice:main \
  run.mac
```

This mounts your local `myrun/` directory as `/run/` inside the container, sets it as the working directory, and runs the simulation using your `run.mac` macro. Output files (e.g. `output.root`) are written back to your local `myrun/` directory.

### Example macro (`run.mac`)

```
# Load your GDML geometry
/detector/readGDML my_detector.gdml

# Configure the particle gun
/gun/particle e-
/gun/energy 10 GeV
/gun/position 0 0 -50 cm
/gun/direction 0 0 1

# Initialize and run
/run/initialize
/run/printProgress 100
/run/beamOn 1000
```

The `/detector/readGDML` command takes a path relative to the working directory inside the container (i.e. relative to where you mounted your files). Geant4 standard `/gun/` commands are used to configure the particle gun — any particle in the Geant4 particle table is supported.

### Supported particles

Any particle available in the Geant4 particle table can be used, including:
`e-`, `e+`, `gamma`, `proton`, `neutron`, `mu-`, `mu+`, `pi+`, `pi-`, `pi0`, `kaon+`, `nu_mu`, and many more.

### Output

Each run produces an `output.root` file in the working directory containing a ROOT TTree (`tree`) with per-event kinematic and hit information for the primary particle and secondaries.

---

## Repository Structure

```
G4TargetPractice/
├── g4sim/                   # Geant4 simulation source code
│   ├── CMakeLists.txt
│   ├── main.cc
│   ├── DetectorConstruction.cc/hh   # GDML-based geometry loader
│   ├── PrimaryGenerator.cc/hh       # Particle gun with macro control
│   ├── RunAction.cc/hh              # ROOT output file/tree
│   ├── EventAction.cc/hh            # Per-event data collection
│   ├── SteppingAction.cc/hh         # Per-step data collection
│   ├── output.gdml                  # Example detector geometry (GDML)
│   ├── simple_det.gdml              # Minimal example GDML
│   └── run_ci.mac                   # Macro used for CI testing
├── run/                     # Example user run directory
│   ├── run.mac              # Example macro file
│   └── MAIA_260211.gdml     # Example GDML geometry
├── .github/workflows/
│   └── build-test-deploy.yml  # CI: build, test, push Docker image
├── Dockerfile               # Alternative standalone Dockerfile
├── .gitignore
└── README.md
```

---

## Building from Source

If you need to build locally (requires Geant4 ≥ 11 with GDML support and ROOT):

```bash
mkdir build
cd build
cmake ../g4sim/
cmake --build .
```

Then run from the repository root:

```bash
./build/g4sim g4sim/run_ci.mac
```

Or with your own macro:

```bash
./build/g4sim /path/to/your/run.mac
```

## Docker Images

Images are built automatically on every push and are available at:

```
ghcr.io/lawrenceleejr/g4targetpractice:<tag>
```

| Tag | Description |
|-----|-------------|
| `main` | Latest stable build from the `main` branch |
| `latest` | Alias for the default branch |
| `<branch>-<sha>` | Per-commit image |

The images are based on `ghcr.io/lobis/root-geant4-garfield` which provides a pre-built Geant4 + ROOT environment.

## Physics

The simulation uses the `FTFP_BERT` reference physics list by default, which covers hadronic and electromagnetic physics appropriate for most HEP detector studies. The physics list can be extended via the macro or by modifying the source.

