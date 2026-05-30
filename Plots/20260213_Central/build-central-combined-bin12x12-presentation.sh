#!/bin/bash

set -euo pipefail

THISDIR=$(cd "$(dirname "$0")" && pwd)
PRESENTATION_DIR="$THISDIR/presentations"
mkdir -p "$PRESENTATION_DIR"

PP_TAG="${PP_TAG_OVERRIDE:-EEV5_ZV9_trkV28_nmix10_bin12x12}"
PPB_TAG="${PPB_TAG_OVERRIDE:-ZV9_trkV28_nmix10_bin12x12}"
PLOT_BASE="${PLOT_OUTPUT_BASE:-../plots/central_combined_bin12x12}"
TEX_FILE="$PRESENTATION_DIR/central_combined_bin12x12.tex"
PDF_FILE="$PRESENTATION_DIR/central_combined_bin12x12.pdf"

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
    local zpt=$1
    local trk=$2
    local title=$3
    cat >> "$TEX_FILE" <<TEXEOF
\begin{frame}[plain]{$title}
\resultpair{$PLOT_BASE/$PP_TAG/all_ZPT${zpt}_trkPT${trk}_${PPB_TAG}-DeltaEta-result.pdf}{$PLOT_BASE/$PP_TAG/all_ZPT${zpt}_trkPT${trk}_${PPB_TAG}-DeltaPhi-result.pdf}
\end{frame}
TEXEOF
}

add_slide "0_500" "0.5_15" '{$0 < p_{T}^{Z} < 500~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_slide "0_30" "0.5_2" '{$0 < p_{T}^{Z} < 30~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 2~\mathrm{GeV}$}'
add_slide "0_30" "2_4" '{$0 < p_{T}^{Z} < 30~\mathrm{GeV},\ 2 < p_{T}^{ch} < 4~\mathrm{GeV}$}'
add_slide "0_30" "4_15" '{$0 < p_{T}^{Z} < 30~\mathrm{GeV},\ 4 < p_{T}^{ch} < 15~\mathrm{GeV}$}'
add_slide "30_500" "0.5_2" '{$30 < p_{T}^{Z} < 500~\mathrm{GeV},\ 0.5 < p_{T}^{ch} < 2~\mathrm{GeV}$}'
add_slide "30_500" "2_4" '{$30 < p_{T}^{Z} < 500~\mathrm{GeV},\ 2 < p_{T}^{ch} < 4~\mathrm{GeV}$}'
add_slide "30_500" "4_15" '{$30 < p_{T}^{Z} < 500~\mathrm{GeV},\ 4 < p_{T}^{ch} < 15~\mathrm{GeV}$}'

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
