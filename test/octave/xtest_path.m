# Check that the xeus-octave override path is the first in the path list
assert(strcmp(strsplit(path(), pathsep()){2}, XEUS_OCTAVE_OVERRIDE_PATH))
