# Remarks:
# 1. Absorber and station positions are defined relative to MuchCave Zin
#
# 2. When defining distances between absorbers and stations keep in mind 
#    the following technological restrictions:
#    a) distance from layer center to absorber >= 5.5 cm 
#    b) distance between layer centers         >=   7 cm 
#
# 3. Absorber materials currently implemented:
#    I - MUCHiron
#    L - MUCHlead
#    W - MUCHwolfram
#    C - MUCHcarbon
#
# 4. There is a possibility to switch off detailed module design 
#    (Use module design = 0). One large module per layer is created 
#    in this case (at the front side of each layer).

# General information
MuchCave Zin position [cm]  :     105
Acceptance tangent min      :     0.1
Acceptance tangent max      :     0.5
Number of absorbers         :       6
Number of stations          :       6
Use module design (0/1)     :       1

# Absorber specification
Absorber Zin position [cm]  :       0      38      76     114     162     215
Absorber thickness [cm]     :      20      20      20      30      35     100
Absorber material           :       I       I       I       I       I       I

# Station specification
Station Zceneter [cm]       :      29      67     105     151     206   327.5
Number of layers            :       2       2       2       2       2       3
Detector type               :       1       1       1       1       1       1
Distance between layers [cm]:       7       7       7       7       7       7
Support thickness [cm]      :     1.5     1.5     1.5     1.5     1.5     1.5
Sigma X minimum [cm]        :    0.04    0.04    0.04    0.04    0.04    0.04
Sigma Y minimum [cm]        :    0.04    0.04    0.04    0.04    0.04    0.04
Sigma X maximum [cm]        :    0.32    0.32    0.32    0.32    0.32    0.32
Sigma Y maximum [cm]        :    0.32    0.32    0.32    0.32    0.32    0.32
Maximum occupancy           :    0.05    0.05    0.05    0.05    0.05    0.05

# GEM module specification (type 1)
Active volume lx [cm]       :      30
Active volume ly [cm]       :      30
Active volume lz [cm]       :     0.3
Spacer lx [cm]              :     0.5
Spacer ly [cm]              :       5
Overlap along y axis [cm]   :       2
