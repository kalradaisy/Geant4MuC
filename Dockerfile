 FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    wget \
    git \
    gdb \
    libxerces-c-dev \
    libexpat1-dev \
    libgl1-mesa-dev \
    libxmu-dev \
    libxi-dev \
    libmotif-dev \
    emacs \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt

RUN wget https://root.cern.ch/download/root_v6.28.12.Linux-ubuntu22-x86_64-gcc11.4.tar.gz && \
    tar -xzf root_v6.28.12.Linux-ubuntu22-x86_64-gcc11.4.tar.gz && \
    rm root_v6.28.12.Linux-ubuntu22-x86_64-gcc11.4.tar.gz
    
# Download Geant4
RUN wget https://gitlab.cern.ch/geant4/geant4/-/archive/v11.4.1/geant4-v11.4.1.tar.gz \
 && tar -xzf geant4-v11.4.1.tar.gz

# Build Geant4
RUN mkdir geant4-build && cd geant4-build && \
    cmake ../geant4-v11.4.1 \
      -DCMAKE_BUILD_TYPE=Debug \
      -DGEANT4_USE_NEUTRINO=ON \
      -DGEANT4_USE_G4NDL=ON \
      -DGEANT4_USE_RADIOACTIVE_DECAY=ON \
      -DGEANT4_BUILD_MULTITHREADED=ON \
      -DGEANT4_USE_GDML=ON \
      -DGEANT4_USE_OPENGL_X11=OFF \
      -DGEANT4_USE_QT=OFF \
      -DGEANT4_USE_UISESSION=ON \
      -DGEANT4_INSTALL_DATA=ON \
      -DCMAKE_INSTALL_PREFIX=/opt/geant4-install \
 && make -j4 && make install

# Setup environment
ENV GEANT4_DIR=/opt/geant4-install
ENV PATH=/opt/geant4-install/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/geant4-install/lib:$LD_LIBRARY_PATH

WORKDIR /workspace

