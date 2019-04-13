# DB_VECTOR
Database related programms and tests for NEC AURORA TSUBASA Vector engine.

veo.c contains a selection operator that runs on the Vector Host Xeon(VH) and offloads a Selection calculation to the Vector Engine (VE) using the VEOFFLOAD API.

use "make veo" to build.
./veo to run

vh.c contains a selection operator that runs on the VE and offloads a calculation to the Vector Host Xeon (VH) using the VH-Call API
use: "make vh" to build
./vh to run
