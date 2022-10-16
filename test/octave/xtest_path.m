# Check that the xeus-octave override path is the first in the path list
assert(strcmp(strsplit(path(), pathsep()){2}, XOCTAVE_OVERRIDE_PATH))