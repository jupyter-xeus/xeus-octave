function display(x)
      name = private_disp_name(x, inputname (1));
      out.("text/latex") = [ "$$" name, "=", latex(x) ,"$$"];
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
