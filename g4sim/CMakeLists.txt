cmake_minimum_required(VERSION 3.15)
project(g4sim)

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
    RunAction.cc
    EventAction.cc
    SteppingAction.cc
    main.cc
)
#set(HEADERS
#  MyPhysicsList.hh
#  )
  

# --------------------
# Executable
# --------------------
add_executable(g4sim ${SOURCES})
add_compile_options(-Wno-shadow)
target_compile_options(g4sim PRIVATE -Wno-shadow)

# Link libraries
target_link_libraries(g4sim ${ROOT_LIBRARIES} ${Geant4_LIBRARIES})
