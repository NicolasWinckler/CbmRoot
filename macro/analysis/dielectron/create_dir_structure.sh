#!/bin/sh

export DIR=/lustre/cbm/user/ebelolap/aug11/sep12/25gev/100field/mvd3

function create_dir() {
    mkdir -p ${DIR}/${1}/log
}

create_dir rho0
create_dir omega
create_dir omegadalitz
create_dir phi

