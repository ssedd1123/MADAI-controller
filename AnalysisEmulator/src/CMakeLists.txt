include_directories(${AnalysisEmulator_SOURCE_DIR}/include)
link_directories(${AnalysisEmulator}/src)
add_executable(AnalysisEmulator test.cxx EmulatorOutputReader.cxx EmulatorInterface.cxx DataSelector.cxx RunInputReader.cxx RunMaster.cxx CSVReader.cxx SettingSetter.cxx ProcessPipe.cxx ExpResultSelector.cxx TraceAnalyzer.cxx)
target_compile_features(AnalysisEmulator PRIVATE cxx_range_for)
