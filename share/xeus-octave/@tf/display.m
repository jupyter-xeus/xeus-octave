function display(sys)
	sysname = inputname (1);
	[inname, outname, tsam] = __lti_data__ (sys.lti);

	[inname, m] = __labels__ (inname, "u");
	[outname, p] = __labels__ (outname, "y");

	if (sys.inv && ! isct (sys))
		[num, den] = filtdata (sys);        # 'num' and 'den' are cells of real-valued vectors
	else
		num = sys.num;                      # 'num' and 'den' are cells of 'tfpoly' objects
		den = sys.den;
	endif

	out = [ "\\mathit{" sysname, "(", sys.tfvar, ")} = " ];
	showmatrix = m > 1 || p > 1;

	if (showmatrix)
		out = [ out " \\begin{bmatrix} " ];
	endif

	for nu = 1 : p
		for ny = 1 : m
			thenum = num{nu, ny};
			theden = den{nu, ny};

			tfp = isa (thenum, "tfpoly");

			if (thenum == tfpoly ([0]))
				def = "0";
			## elseif (den == tfpoly (1)) doesn't work because it
			## would mistakingly accept non-tfpoly denominators like [0, 1]
			elseif ((tfp && theden == 1) || (! tfp && isequal (theden, 1)))
				def = __latex_fix_sci_not__(tfpoly2str (thenum, sys.tfvar));
			else
				numstr = __latex_fix_sci_not__(tfpoly2str (thenum, sys.tfvar));
				denstr = __latex_fix_sci_not__(tfpoly2str (theden, sys.tfvar));

				def = [ "\\frac{" numstr "}{", denstr, "}" ];
			endif

			out = [ out, def ];

			if (showmatrix && ny != m)
				out = [ out " & " ];
			endif
		endfor

		if (showmatrix && nu != p)
			out = [ out " \\\\ " ];
		endif
	endfor

	if (showmatrix)
		out = [ out " \\end{bmatrix} " ];
	endif

	dout.("text/latex") = [ "$$ " out, " $$"];
	display_data(dout);

	display (sys.lti);  # display sampling time

	if (tsam == -2)
		disp ("Static gain.");
	elseif (tsam == 0)
		disp ("Continuous-time model.");
	else
		disp ("Discrete-time model.");
	endif

	disp("")
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
