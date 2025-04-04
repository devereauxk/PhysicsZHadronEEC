#include <iostream>

void PrintUsage()
{
    cout << "Usage: program [options]" << endl;
    cout << "Options:" << endl;
    cout << "  --help                Print this help message" << endl;
    cout << "  --OutputBase=BASE     Set the output base name (default: Plot)" << endl;
    cout << "  --DataFiles=FILES     Set the data files (default: pp.root, test.root)" << endl;
    cout << "  --SkipSystematics     Skip systematics calculation (default: false)" << endl;
    cout << "  --SystematicFiles=FILES Set the systematic files" << endl;
    cout << "  --CurveLabels=LABELS  Set the curve labels (default: pp, PbPb 0-30%)" << endl;
    cout << "  --ToPlot=PLOT         Set the plot name (default: DeltaPhi)" << endl;
    cout << "  --Tags=TAGS           Set the tags (default: Result1_2, Result2_4, Result4_10)" << endl;
    cout << "  --SecondTags=TAGS     Set the second tags" << endl;
    cout << "  --Labels=LABELS       Set the labels" << endl;
    cout << "  --ExtraInfo=INFO      Set the extra information" << endl;
    cout << "  --XMin=MIN            Set the X axis minimum (default: 0)" << endl;
    cout << "  --XMax=MAX            Set the X axis maximum (default: PI)" << endl;
    cout << "  --YMin=MIN            Set the Y axis minimum (default: -5)" << endl;
    cout << "  --YMax=MAX            Set the Y axis maximum (default: 5)" << endl;
    cout << "  --RMin=MIN            Set the R axis minimum (default: -5)" << endl;
    cout << "  --RMax=MAX            Set the R axis maximum (default: 5)" << endl;
    cout << "  --XAxisLabel=LABEL    Set the X axis label (default: |#Delta#phi_{trk,Z}|)" << endl;
    cout << "  --YAxisLabel=LABEL    Set the Y axis label (default: <#DeltaN_{ch}>/event)" << endl;
    cout << "  --RAxisLabel=LABEL    Set the R axis label (default: Difference to pp)" << endl;
    cout << "  --LegendLeft=VALUE    Set the legend left position (default: 0.08)" << endl;
    cout << "  --LegendBottom=VALUE  Set the legend bottom position (default: 0.60)" << endl;
}
