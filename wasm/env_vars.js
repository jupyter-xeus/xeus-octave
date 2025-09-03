Module["onRuntimeInitialized"] = () => {
    console.log("Octave is ready!");
}

Module["preRun"] = () => {
    ENV["OCTAVE_HOME"] = "/";
    ENV["OCTAVE_PATH"] = "/share/xeus-octave::/share/octave/site/m:/share/octave/site/m/startup:/share/octave/10.2.0/m:/share/octave/10.2.0/m/audio:/share/octave/10.2.0/m/deprecated:/share/octave/10.2.0/m/elfun:/share/octave/10.2.0/m/general:/share/octave/10.2.0/m/geometry:/share/octave/10.2.0/m/gui:/share/octave/10.2.0/m/help:/share/octave/10.2.0/m/image:/share/octave/10.2.0/m/io:/share/octave/10.2.0/m/legacy:/share/octave/10.2.0/m/linear-algebra:/share/octave/10.2.0/m/miscellaneous:/share/octave/10.2.0/m/ode:/share/octave/10.2.0/m/optimization:/share/octave/10.2.0/m/path:/share/octave/10.2.0/m/pkg:/share/octave/10.2.0/m/plot:/share/octave/10.2.0/m/plot/appearance:/share/octave/10.2.0/m/plot/draw:/share/octave/10.2.0/m/plot/util:/share/octave/10.2.0/m/polynomial:/share/octave/10.2.0/m/prefs:/share/octave/10.2.0/m/profiler:/share/octave/10.2.0/m/set:/share/octave/10.2.0/m/signal:/share/octave/10.2.0/m/sparse:/share/octave/10.2.0/m/specfun:/share/octave/10.2.0/m/special-matrix:/share/octave/10.2.0/m/startup:/share/octave/10.2.0/m/statistics:/share/octave/10.2.0/m/strings:/share/octave/10.2.0/m/testfun:/share/octave/10.2.0/m/time:/share/octave/10.2.0/m/web:/share/octave/10.2.0/data";
    ENV["OCTAVE_BUILT_IN_DOCSTRINGS_FILE"] = "/share/octave/10.2.0/etc/built-in-docstrings";
    ENV["OCTAVE_TEXI_MACROS_FILE"] = "/share/octave/10.2.0/etc/macros.texi";
};
