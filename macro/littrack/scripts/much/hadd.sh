#$ -wd /tmp 
#$ -j y
# Number of subtasks to start
#$ -t 1
dir=/hera/cbm/users/andrey/mc/much/${geometry}/urqmd${particle}/auau/${energy}/${centrality}/

#for i in {0..50..1}
#do
#  XXX=$(printf "%03d" "$i")
#  outfile=${dir}/analysis.auau.6gev.mbias.temp-all.${XXX}.root
#  infiles=${dir}/analysis.auau.6gev.mbias.${XXX}*.root
#  echo "Adding histograms ${XXX}: input=${infiles}, output=${outfile}"
#  hadd  -T -f -k ${outfile} ${infiles}
#done

outfile=${dir}/qa.auau.${energy}.${centrality}.all.root
infiles=${dir}/qa.auau.${energy}.${centrality}.*.root
hadd -T -f -k ${outfile} ${infiles}

  