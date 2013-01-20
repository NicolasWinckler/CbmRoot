export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build/
export SCRIPT=yes

export RADIATOR_NAME=

for N in 0 1 2 3 4; do

   if [ ${N} = "0" ] ; then
      export RADIATOR_NAME=Spectra_Be_run2310007_Rad_G20_Sus12.root
   elif [ ${N} = "1" ]; then
      export RADIATOR_NAME=Spectra_Be_run2310004_Rad_F_Sus18.root
   elif [ ${N} = "2" ]; then
      export RADIATOR_NAME=Spectra_Be_run2210010_Rad_G10_Sus18.root
   elif [ ${N} = "3" ]; then
      export RADIATOR_NAME=Spectra_Be_run2310014_Rad_B_Sus18.root
   elif [ ${N} = "4" ]; then
      export RADIATOR_NAME=Spectra_Be_run2310010_Rad_G20_Sus18.root
   elif [ ${N} = "5" ]; then
      export RADIATOR_NAME=.root
   elif [ ${N} = "6" ]; then
      export RADIATOR_NAME=.root
   elif [ ${N} = "7" ]; then
      export RADIATOR_NAME=.root
   fi

   xterm -hold -e ". ./sim.sh"&

done

export SCRIPT=no