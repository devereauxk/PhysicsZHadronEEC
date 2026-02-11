#!/bin/bash

INPUT=/home/kdeverea/Jewel/jewel-2.4.0/eventfiles/pp.hepmc
OUTPUT=output/pp.root

./ExecuteHepMC --Input $INPUT --Output $OUTPUT 
