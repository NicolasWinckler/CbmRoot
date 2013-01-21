
dir=/hera/cbm/users/slebedev/mc/dielectron/dec12/25gev/mirrors/

function create_dir() {
    mkdir -p ${1}/log
    mkdir -p ${1}/results/all
}

function create_dir_particles() {
    create_dir ${1}/rho0
    create_dir ${1}/omegaepem
    create_dir ${1}/omegadalitz
    create_dir ${1}/phi
    mkdir -p ${1}/results
}

create_dir_particles ${dir}/2mm/1.0field/nomvd/
create_dir_particles ${dir}/5mm/1.0field/nomvd/
create_dir_particles ${dir}/6mm/1.0field/nomvd/
create_dir_particles ${dir}/10mm/1.0field/nomvd/
#create_dir_particles ${dir}/0.7field/mvd4/
#create_dir_particles ${dir}/1.0field/mvd4/
#create_dir_particles ${dir}/0.5field/mvd/
#create_dir_particles ${dir}/0.5field/mvd4/

