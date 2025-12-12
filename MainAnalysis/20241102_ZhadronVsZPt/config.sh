PT_RANGES=("0.5_500") # ("0.5_1" "1_2" "2_4") # ("0.5_10")
ZPT_RANGES=("40_500") # ("0_10" "10_20" "20_40") # ("0_350")
UseLeadingTrk=0
UseTrackWeight=1
UseEventWeight=0 
UseResidualWeight=0
yBoost=0
EmbedEPOS=1
TAG="track_nominal_V16"

# nominal = track + event, (if gen) EPOS
# residual = nominal + residual

# track_nominal = track, (if gen) EPOS
# track_residual = track_nominal + residual