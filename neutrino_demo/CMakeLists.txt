cmake_minimum_required(VERSION 3.15)
project(neutrino_demo)

# --------------------
# Find ROOT
# --------------------
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist)
include_directories(${ROOT_INCLUDE_DIRS})

# --------------------
# Find Geant4
# --------------------
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# --------------------
# Sources
# --------------------
set(SOURCES
    DetectorConstruction.cc
    DetectorMessenger.cc
    PrimaryGenerator.cc
    PrimaryGeneratorMessenger.cc
    MyNeutrinoPhysics.cc
    MyPhysicsList.cc
    RunAction.cc
    EventAction.cc
    SteppingAction.cc
    TrackingAction.cc
    main.cc
    Biasing.cc
    BiasingMessenger.cc
    ActionInitialization.cc
)
#set(HEADERS
#  MyPhysicsList.hh
#  )
  

# --------------------
# Executable
# --------------------
add_executable(neutrino ${SOURCES})
add_compile_options(-Wno-shadow)
target_compile_options(neutrino PRIVATE -Wno-shadow)

# Link libraries
target_link_libraries(neutrino ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})
