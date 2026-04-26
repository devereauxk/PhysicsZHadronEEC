#!/bin/bash

# Define common arguments
if [ "${SKIP_CLEAN:-0}" != "1" ]; then
   source clean.sh
fi
config_file=${CONFIG_FILE:-config.sh}
source "$config_file"
cut_parallelism=${CUT_PARALLELISM:-1}
if [ "$cut_parallelism" -lt 1 ]; then
   cut_parallelism=1
fi

for zpt_range in "${ZPT_RANGES[@]}"; do
   min_zpt=${zpt_range/_*/}
   max_zpt=${zpt_range/*_/}

   pids=()
   for pt_range in "${PT_RANGES[@]}"; do
      min_pt=${pt_range/_*/}
      max_pt=${pt_range/*_/}

      echo ./threader.sh "output/${1}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" "${12}" "${13}" "${14}" "${15}" "${16}" "${17}" "${18}" "${19}" "${20}" "${21}" "${22}" "${23}" "${24}" "${25}" "${26}" "${27}" "${28}" "${29}" "${30}" "${31}" "${32}" "${33}" "${34}" "${35}" "${36}" "${37}" "${38}" "${39}" "${40}" \
         --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt"

      ./threader.sh "output/${1}_ZPT${min_zpt}_${max_zpt}" "$pt_range" "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "${10}" "${11}" "${12}" "${13}" "${14}" "${15}" "${16}" "${17}" "${18}" "${19}" "${20}" "${21}" "${22}" "${23}" "${24}" "${25}" "${26}" "${27}" "${28}" "${29}" "${30}" "${31}" "${32}" "${33}" "${34}" "${35}" "${36}" "${37}" "${38}" "${39}" "${40}" \
         --MinTrackPT "$min_pt" --MaxTrackPT "$max_pt" \
         --MinZPT "$min_zpt" --MaxZPT "$max_zpt" &
      pids+=($!)

      while [ "${#pids[@]}" -ge "$cut_parallelism" ]; do
         wait -n
         alive=()
         for pid in "${pids[@]}"; do
            if kill -0 "$pid" 2>/dev/null; then
               alive+=("$pid")
            fi
         done
         pids=("${alive[@]}")
      done
   done

   wait

   # Combine results for the current HiBin and ZPT range
   hadd -f "plots/${1}_ZPT${min_zpt}_${max_zpt}-nosub.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/${1}_ZPT${min_zpt}_${max_zpt}-${pt_range}-nosub.root"
         done)
   hadd -f "plots/${1}_ZPT${min_zpt}_${max_zpt}-result.root" \
       $(for pt_range in "${PT_RANGES[@]}"; do
            echo "output/${1}_ZPT${min_zpt}_${max_zpt}-${pt_range}-result.root"
         done)
   
done
