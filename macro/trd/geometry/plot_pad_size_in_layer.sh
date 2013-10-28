# plot only 1 pad size per module
#root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13g.digi.par\"\)
#
# plot all 3 pad sizes per module, all layers
#root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13g.digi.par\",3,0\)
#
# plot all 3 pad sizes per module, only 1st layer of each station
root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13g.digi.par\",3,1\)
