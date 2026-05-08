#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
PRESENTATION_DIR="$THISDIR/presentations"
mkdir -p "$PRESENTATION_DIR"

SHIFTED_PP_TAG="${SHIFTED_PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10_bin10x10shifted_20260506}"
SHIFTED_PPB_TAG="${SHIFTED_PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10_bin10x10shifted_20260506}"
SHIFTED_PLOT_BASE="${SHIFTED_PLOT_OUTPUT_BASE:-../plots/central_combined_bin10x10shifted_20260506}"
LEGACY_PP_TAG="${LEGACY_PP_TAG_OVERRIDE:-EEV5_ZV9_trkV27_nmix10_bin10x10_20260502}"
LEGACY_PPB_TAG="${LEGACY_PPB_TAG_OVERRIDE:-ZV9_trkV27_nmix10_bin10x10_20260502}"
LEGACY_PLOT_BASE="${LEGACY_PLOT_OUTPUT_BASE:-../plots/central_combined_bin10x10_20260502}"
TEX_FILE="$PRESENTATION_DIR/central_combined_bin10x10_20260502.tex"
PDF_FILE="$PRESENTATION_DIR/central_combined_bin10x10_20260502.pdf"
INTEGRAL_TEX_FILE="$PRESENTATION_DIR/central_combined_bin10x10_20260502_integrals.tex"

"$THISDIR/make-deltaphi-integral-table-bin10x10-20260502.sh" "$PRESENTATION_DIR"

cat > "$TEX_FILE" <<'TEXEOF'
\documentclass[aspectratio=169]{beamer}
\usepackage{graphicx}
\setbeamertemplate{navigation symbols}{}
\setbeamertemplate{footline}{}
\setbeamertemplate{headline}{}
\setbeamersize{text margin left=0.2cm,text margin right=0.2cm}
\newcommand{\resultpair}[2]{%
  \centering
  \includegraphics[width=0.49\textwidth,height=0.82\textheight,keepaspectratio]{#1}%
  \hfill
  \includegraphics[width=0.49\textwidth,height=0.82\textheight,keepaspectratio]{#2}%
}
\begin{document}
TEXEOF

add_slide() {
    local plot_base=$1
    local pp_tag=$2
    local ppb_tag=$3
    local zpt=$4
    local trk=$5
    local title=$6
    cat >> "$TEX_FILE" <<TEXEOF
\\begin{frame}[plain]{$title}
\\resultpair{$plot_base/$pp_tag/all_ZPT${zpt}_trkPT${trk}_${ppb_tag}-DeltaEta-result.pdf}{$plot_base/$pp_tag/all_ZPT${zpt}_trkPT${trk}_${ppb_tag}-DeltaPhi-result.pdf}
\\end{frame}
TEXEOF
}

add_note_slide() {
    local zpt=$1
    local trk=$2
    local title=$3
    add_slide "$SHIFTED_PLOT_BASE" "$SHIFTED_PP_TAG" "$SHIFTED_PPB_TAG" "$zpt" "$trk" "$title"
}

add_legacy_slide() {
    local zpt=$1
    local trk=$2
    local title=$3
    add_slide "$LEGACY_PLOT_BASE" "$LEGACY_PP_TAG" "$LEGACY_PPB_TAG" "$zpt" "$trk" "$title"
}

add_note_slide "0_500" "0.5_15" '{$0 < p_{T}^{Z} < 500~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_note_slide "0_30" "0.5_2" '{$0 < p_{T}^{Z} < 30~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 2~\mathrm{GeV}$}'
add_note_slide "0_30" "2_4" '{$0 < p_{T}^{Z} < 30~\mathrm{GeV},\ 2 < p_{T}^{ch} < 4~\mathrm{GeV}$}'
add_note_slide "0_30" "4_15" '{$0 < p_{T}^{Z} < 30~\mathrm{GeV},\ 4 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_note_slide "30_500" "0.5_2" '{$30 < p_{T}^{Z} < 500~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 2~\mathrm{GeV}$}'
add_note_slide "30_500" "2_4" '{$30 < p_{T}^{Z} < 500~\mathrm{GeV},\ 2 < p_{T}^{ch} < 4~\mathrm{GeV}$}'
add_note_slide "30_500" "4_15" '{$30 < p_{T}^{Z} < 500~\mathrm{GeV},\ 4 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_legacy_slide "5_350" "0.5_15" '{$5 < p_{T}^{Z} < 350~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_legacy_slide "20_40" "0.5_15" '{$20 < p_{T}^{Z} < 40~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_legacy_slide "5_30" "0.5_2" '{$5 < p_{T}^{Z} < 30~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 2~\mathrm{GeV}$}'
add_legacy_slide "5_30" "2_4" '{$5 < p_{T}^{Z} < 30~\mathrm{GeV},\ 2 < p_{T}^{ch} < 4~\mathrm{GeV}$}'
add_legacy_slide "5_30" "4_15" '{$5 < p_{T}^{Z} < 30~\mathrm{GeV},\ 4 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_legacy_slide "30_350" "0.5_2" '{$30 < p_{T}^{Z} < 350~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 2~\mathrm{GeV}$}'
add_legacy_slide "30_350" "2_4" '{$30 < p_{T}^{Z} < 350~\mathrm{GeV},\ 2 < p_{T}^{ch} < 4~\mathrm{GeV}$}'
add_legacy_slide "30_350" "4_15" '{$30 < p_{T}^{Z} < 350~\mathrm{GeV},\ 4 < p_{T}^{ch} < 15~\mathrm{GeV}$}'

if [ ! -f "$INTEGRAL_TEX_FILE" ]; then
    echo "Missing DeltaPhi integral audit slide at $INTEGRAL_TEX_FILE" >&2
    exit 1
fi
echo "\\input{$(basename "$INTEGRAL_TEX_FILE")}" >> "$TEX_FILE"
echo '\end{document}' >> "$TEX_FILE"

cd "$PRESENTATION_DIR"
if command -v pdflatex >/dev/null 2>&1; then
    pdflatex -interaction=nonstopmode -halt-on-error "$(basename "$TEX_FILE")" >/dev/null
elif command -v tectonic >/dev/null 2>&1; then
    tectonic "$(basename "$TEX_FILE")" >/dev/null
else
    echo "No TeX engine found (checked pdflatex and tectonic)." >&2
    exit 127
fi
rm -f "$(basename "$TEX_FILE" .tex)".aux "$(basename "$TEX_FILE" .tex)".log "$(basename "$TEX_FILE" .tex)".nav "$(basename "$TEX_FILE" .tex)".out "$(basename "$TEX_FILE" .tex)".snm "$(basename "$TEX_FILE" .tex)".toc

if [ ! -f "$PDF_FILE" ]; then
    echo "Failed to build $PDF_FILE" >&2
    exit 1
fi
