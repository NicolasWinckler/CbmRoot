
dir=/hera/cbm/users/andrey/mc/dielectron/dec12/8gev/

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

create_dir_particles ${dir}/0.7field/mvd/
create_dir_particles ${dir}/0.7field/nomvd/
create_dir_particles ${dir}/1.0field/mvd/
create_dir_particles ${dir}/1.0field/nomvd/

