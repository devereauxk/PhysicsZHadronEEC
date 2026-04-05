#!/bin/bash

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
   echo "Source this file instead: source env.sh"
   exit 1
fi

export PYTHIA_LOCAL_BASE=/home/kdeverea/pythia
export PYTHIA8_BASE=${PYTHIA_LOCAL_BASE}/pythia8310
export HEPMC3_BASE=${PYTHIA_LOCAL_BASE}/hepmc3
export LHAPDF6_BASE=${PYTHIA_LOCAL_BASE}/lhapdf6
export MG5AMC_BASE=${PYTHIA_LOCAL_BASE}/MG5_aMC
export MG5AMC_PYTHON=/usr/bin/python3.11

export PATH=${PYTHIA_LOCAL_BASE}/bin:${PYTHIA8_BASE}/bin:${LHAPDF6_BASE}/bin:${PATH}
export LD_LIBRARY_PATH=${PYTHIA8_BASE}/lib:${HEPMC3_BASE}/lib64:${LHAPDF6_BASE}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}
export PYTHONPATH=${PYTHIA_LOCAL_BASE}/python${PYTHONPATH:+:${PYTHONPATH}}
export LHAPDF_DATA_PATH=${LHAPDF6_BASE}/share/LHAPDF
