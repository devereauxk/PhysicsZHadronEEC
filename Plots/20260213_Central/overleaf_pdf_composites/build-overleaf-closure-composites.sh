#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
OVERLEAF_BASE="../../../../OverleafZHadronInPPb/figures/analysis/closure"
PP_TAG="EEV5_ZV9_trkV27_nmix10"
PA_TAG="ZV9_trkV27_nmix10"
TRACK_PT="0.5_15"
ZPTS=("0_10" "10_20" "20_40" "40_500")

if ! command -v tectonic >/dev/null 2>&1; then
    echo "tectonic is required but was not found in PATH." >&2
    exit 1
fi

build_grid_pdf() {
    local output_pdf=$1
    local top_left=$2
    local top_mid=$3
    local top_right=$4
    local bottom_left=$5
    local bottom_mid=$6
    local bottom_right=$7
    local stem="${output_pdf%.pdf}"
    local tex_file="$THISDIR/${stem}.tex"

    cat > "$tex_file" <<EOF
\documentclass{article}
\usepackage{graphicx}
\usepackage[paperwidth=16in,paperheight=11.2in,margin=0in]{geometry}
\pagestyle{empty}
\setlength{\parindent}{0pt}
\setlength{\tabcolsep}{0pt}
\setlength{\fboxsep}{0pt}
\newcommand{\scanpanel}[1]{%
  \includegraphics[width=0.329\paperwidth,trim=12 12 12 10,clip]{#1}%
}
\newcommand{\scanrow}[3]{%
  \noindent\makebox[\paperwidth][c]{%
    \scanpanel{#1}\hspace{-0.25em}%
    \scanpanel{#2}\hspace{-0.25em}%
    \scanpanel{#3}%
  }%
}
\begin{document}
\scanrow{$top_left}{$top_mid}{$top_right}
\vspace{-0.8em}
\scanrow{$bottom_left}{$bottom_mid}{$bottom_right}
\end{document}
EOF

    tectonic --outdir "$THISDIR" "$tex_file" >/dev/null
    rm -f "$THISDIR/${stem}.aux" "$THISDIR/${stem}.log"

    if [ ! -f "$THISDIR/$output_pdf" ]; then
        echo "Failed to build $output_pdf" >&2
        exit 1
    fi
}

closure_path() {
    local system=$1
    local zpt=$2
    local observable=$3
    local suffix=$4
    local tag="$PA_TAG"
    if [ "$system" = "pp" ]; then
        tag="$PP_TAG"
    fi
    printf '%s/%s_ZPT%s_trkPT%s_%s-closure-%s-%s.pdf' \
        "$OVERLEAF_BASE" "$system" "$zpt" "$TRACK_PT" "$tag" "$observable" "$suffix"
}

for zpt in "${ZPTS[@]}"; do
    build_grid_pdf \
        "overleaf_closure_presub_ZPT${zpt}_deltaphi_2x3.pdf" \
        "$(closure_path pp  "$zpt" DeltaPhi all)" \
        "$(closure_path pPb "$zpt" DeltaPhi all)" \
        "$(closure_path PbP "$zpt" DeltaPhi all)" \
        "$(closure_path pp  "$zpt" DeltaPhi bkg)" \
        "$(closure_path pPb "$zpt" DeltaPhi bkg)" \
        "$(closure_path PbP "$zpt" DeltaPhi bkg)"

    build_grid_pdf \
        "overleaf_closure_presub_ZPT${zpt}_deltaeta_2x3.pdf" \
        "$(closure_path pp  "$zpt" DeltaEta all)" \
        "$(closure_path pPb "$zpt" DeltaEta all)" \
        "$(closure_path PbP "$zpt" DeltaEta all)" \
        "$(closure_path pp  "$zpt" DeltaEta bkg)" \
        "$(closure_path pPb "$zpt" DeltaEta bkg)" \
        "$(closure_path PbP "$zpt" DeltaEta bkg)"

    build_grid_pdf \
        "overleaf_closure_postsub_ZPT${zpt}_2x3.pdf" \
        "$(closure_path pp  "$zpt" DeltaPhi result)" \
        "$(closure_path pPb "$zpt" DeltaPhi result)" \
        "$(closure_path PbP "$zpt" DeltaPhi result)" \
        "$(closure_path pp  "$zpt" DeltaEta result)" \
        "$(closure_path pPb "$zpt" DeltaEta result)" \
        "$(closure_path PbP "$zpt" DeltaEta result)"
done
