#$ -wd /tmp 
#$ -j y
# Number of subtasks to start
#$ -t 1
#dir=/hera/cbm/users/vassilie/D0/nov12/tof_10m/urqmd/auau/25gev/centr
#dir=/hera/cbm/users/vassilie/sts_rich_tof/nov12/urqmd/auau/2gev/centr
dir=/hera/cbm/users/vassilie/mc/sts_rich_tof/urqmd/auau/25gev/mbias
#dir=/hera/cbm/users/vassilie/mc/nu/dec12/urqmd/auau/gev/centr
#dir=/hera/cbm/users/vassilie/mc/nov12/tof_10m/urqmd/auau/8gev/centr

#for i in {0..50..1}
#do
#  XXX=$(printf "%03d" "$i")
#  outfile=${dir}/analysis.auau.6gev.mbias.temp-all.${XXX}.root
#  infiles=${dir}/analysis.auau.6gev.mbias.${XXX}*.root
#  echo "Adding histograms ${XXX}: input=${infiles}, output=${outfile}"
#  hadd  -T -f -k ${outfile} ${infiles}
#done

outfile=${dir}/analysis.auau.25gev.mbias.all.root
infiles=${dir}/analysis.auau.25gev.mbias.temp-all.*.root
hadd  -T -f -k -v 1 ${outfile} ${infiles}

#rm -rf ${infiles} 
  