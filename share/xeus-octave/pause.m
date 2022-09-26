##  -*- texinfo -*-
## @deftypefn  {} {} pause ()
## @deftypefnx {} {} pause (@var{n})
## @deftypefnx {} {@var{old_state} =} pause ("on")
## @deftypefnx {} {@var{old_state} =} pause ("off")
## @deftypefnx {} {@var{old_state} =} pause ("query")
## Suspend the execution of the program or change the state of the pause function.
##
## If invoked without an input arguments then the program is suspended until a
## character is typed.  If argument @var{n} is a positive real value, it indicates
## the number of seconds the program shall be suspended, for example:
##
## @example
## @group
## tic; pause (0.05); toc
##      @print{} Elapsed time is 0.05039 seconds.
## @end group
## @end example
##
## The following example prints a message and then waits 5 seconds before
## clearing the screen.
##
## @example
## @group
## disp ("wait please...");
## pause (5);
## clc;
## @end group
## @end example
##
## If invoked with a string argument @qcode{"on"}, @qcode{"off"}, or
## @qcode{"query"}, the state of the pause function is changed or queried.  When
## the state is @qcode{"off"}, the pause function returns immediately.  The
## optional return value contains the previous state of the pause function.  In
## the following example pause is disabled locally:
##
## @example
## @group
## old_state = pause ("off");
## tic; pause (0.05); toc
##      @print{} Elapsed time is 3.00407e-05 seconds.
## pause (old_state);
## @end group
## @end example
##
## While the program is suspended Octave still handles figures painting and
## graphics callbacks execution.
##
## @seealso{kbhit}
## @end deftypefn

function r = pause(varargin)
	if (nargin == 0)
		warning("Pause cannot be called without an argument in notebooks")
	else
		builtin("pause", varargin{:})
	end
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
