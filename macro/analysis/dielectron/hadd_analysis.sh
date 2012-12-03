dir=/hera/cbm/users/andrey/mc/dielectron/nov12/8gev/0.7field/mvd/omegaepem/

outfile=${dir}/reco.auau.8gev.centr.all.root
infiles=${dir}/reco.auau.8gev.centr.*.root

hadd  -T -f ${outfile} ${infiles}
