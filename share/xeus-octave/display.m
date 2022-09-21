## -*- texinfo -*-
## @deftypefn {} {} display (@var{obj})
## Display the contents of the object @var{obj} prepended by its name.
##
## The Octave interpreter calls the @code{display} function whenever it needs
## to present a class on-screen.  Typically, this would be a statement which
## does not end in a semicolon to suppress output.  For example:
##
## @example
## myclass (@dots{})
## @end example
##
## Or:
##
## @example
## myobj = myclass (@dots{})
## @end example
##
## In general, user-defined classes should overload the @code{disp} method to
## avoid the default output:
##
## @example
## @group
## myobj = myclass (@dots{})
##   @result{} myobj =
##
##   <class myclass>
## @end group
## @end example
##
## When overloading the @code{display} method instead, one has to take care
## of properly displaying the object's name.  This can be done by using the
## @code{inputname} function.
##
## @seealso{disp, class, subsref, subsasgn}
## @end deftypefn

function display(varargin)
	if nargin < 1 || nargin > 2
		print_usage();
	endif

	value = varargin{1};
	name = "";
	tinfo = typeinfo(value);
	fmt = displayformat(tinfo);

	if nargin == 2
		if isa(varargin{2}, "char")
			name = varargin{2};
		else
			error("NAME must be a string");
		endif
	else
		name = inputname(1);
	endif

	switch tinfo
		case { "matrix", "bool matrix", "complex matrix", "range" }
			switch fmt
				case "latex"
					displaymatrixlatex(name, value, tinfo);
				case "html"
					displaymatrixhtml(name, value, tinfo);
				otherwise
					displaytext(name, value);
			end
		case { "scalar" "complex scalar" }
			switch fmt
				case "latex"
					displayscalarlatex(name, value);
				otherwise
					displaytext(name, value);
			end
		case { "scalar struct" "struct" }
			switch fmt
				case "json"
					try
						displayjson(name, value);
					catch
						displaytext(name, value);
					end
				otherwise
					displaytext(name, value);
			end
		otherwise
			displaytext(name, value);
	end
end

function displaymatrixhtml (name, value, fmt)
	out.("text/html") = __matrix_to_html__(name, value, fmt);
	out.("text/plain") = [name, " = ", disp(value)];
	display_data(out);
end

function displaymatrixlatex (name, value, fmt)
	out.("text/latex") = __matrix_to_latex__(name, value, fmt);
	out.("text/plain") = [name, " = ", disp(value)];
	display_data(out);
end

function displayscalarlatex (name, value)
	x = ["$$" name " = " ];
	x = [ x num2str(value) ];
	x = [ x "$$" ];

	out.("text/latex") = x;
	out.("text/plain") = [name, " = ", disp(value)];
	display_data(out);
end

function displayjson (name, value)
	metadata.("application/json").root = name;
	out.("application/json") = jsonencode(value);
	out.("text/plain") = [name, " = ", disp(value)];
	display_data(out);
end

function displaytext (name, value)
	out.("text/plain") = [name, " = ", disp(value)];
	display_data(out);
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
