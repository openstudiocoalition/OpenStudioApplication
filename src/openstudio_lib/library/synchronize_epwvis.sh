#!/usr/bin/env bash
# Synchronize epwvis assets into the OpenStudio library folder.
# Run this script whenever the epwvis source is updated.

EPWVIS_SOURCE_DIR="/Users/julien/Software/Others/epwvis"

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

mkdir -p "$SCRIPT_DIR/js/epwvis"
mkdir -p "$SCRIPT_DIR/css/epwvis"

# JavaScript libraries
cp "$EPWVIS_SOURCE_DIR/lib/js/jQuery/jquery-2.2.4.min.js"       "$SCRIPT_DIR/js/epwvis/jquery.min.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/bootstrap/bootstrap.min.js"        "$SCRIPT_DIR/js/epwvis/bootstrap.min.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/d3/d3.min.js"                      "$SCRIPT_DIR/js/epwvis/d3.min.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/crossfilter/crossfilter.min.js"    "$SCRIPT_DIR/js/epwvis/crossfilter.min.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/psychrolib.js"                     "$SCRIPT_DIR/js/epwvis/psychrolib.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/epw2json.js"                       "$SCRIPT_DIR/js/epwvis/epw2json.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/epwCharts.js"                      "$SCRIPT_DIR/js/epwvis/epwCharts.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/solarPV.js"                        "$SCRIPT_DIR/js/epwvis/solarPV.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/save-svg-as-png/saveSvgAsPng.js"  "$SCRIPT_DIR/js/epwvis/saveSvgAsPng.js"
cp "$EPWVIS_SOURCE_DIR/lib/js/jszip/jszip.min.js"               "$SCRIPT_DIR/js/epwvis/jszip.min.js"

# CSS
cp "$EPWVIS_SOURCE_DIR/lib/css/bootstrap/bootstrap.min.css"      "$SCRIPT_DIR/css/epwvis/bootstrap.min.css"
cp "$EPWVIS_SOURCE_DIR/lib/css/epwvis.css"                       "$SCRIPT_DIR/css/epwvis/epwvis.css"

echo "epwvis assets synchronized successfully."
