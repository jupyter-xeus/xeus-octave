##  -*- texinfo -*-
## @deftypefn  {} {} addpath (@var{dir1}, @dots{})
## @deftypefnx {} {} addpath (@var{dir1}, @dots{}, @var{option})
## Add named directories to the function search path.
##
## If @var{option} is @qcode{"-begin"} or 0 (the default), prepend the
## directory name to the current path.  If @var{option} is @qcode{"-end"}
## or 1, append the directory name to the current path.
## Directories added to the path must exist.
##
## In addition to accepting individual directory arguments, lists of
## directory names separated by @code{pathsep} are also accepted.  For example:
##
## @example
## addpath ("dir1:/dir2:~/dir3")
## @end example
##
## For each directory that is added, and that was not already in the path,
## @code{addpath} checks for the existence of a file named @file{PKG_ADD}
## (note lack of .m extension) and runs it if it exists.
##
## @seealso{path, rmpath, genpath, pathdef, savepath, pathsep}
## @end deftypefn

function r = addpath(varargin)
	r = builtin("addpath", varargin{:});

	# Any time we change the path we put back our patch path in the first place
	builtin("addpath", xoctave.OVERRIDE_PATH, "-begin");
end

##
## Copyright (C) 2020 Giulio Girardi.
##
## This file is part of xeus-octave.
##
## xeus-octave is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## xeus-octave is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
##
## along with xeus-octave.  If not, see <http://www.gnu.org/licenses/>.
##