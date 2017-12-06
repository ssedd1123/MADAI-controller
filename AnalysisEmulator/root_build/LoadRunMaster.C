void LoadRunMaster()
{

gROOT->ProcessLine("gInterpreter->AddIncludePath(\"../include/\")");
gROOT->ProcessLine("gInterpreter->AddIncludePath(\"../src/\")");

gROOT->ProcessLine(".L ../src/TraceAnalyzer.cxx");
gROOT->ProcessLine(".L ../src/ExpResultSelector.cxx");
gROOT->ProcessLine(".L ../src/CSVReader.cxx");
gROOT->ProcessLine(".L ../src/DataSelector.cxx");
gROOT->ProcessLine(".L ../src/EmulatorInterface.cxx");
gROOT->ProcessLine(".L ../src/EmulatorOutputReader.cxx");
gROOT->ProcessLine(".L ../src/ProcessPipe.cxx");
gROOT->ProcessLine(".L ../src/RunInputReader.cxx");
gROOT->ProcessLine(".L ../src/SettingSetter.cxx");
gROOT->ProcessLine(".L ../src/RunMaster.cxx");
gROOT->ProcessLine(".L ../src/DataCreator.cxx");

gROOT->ProcessLine(".L TestModelExact/TestModelExact.h");

}
