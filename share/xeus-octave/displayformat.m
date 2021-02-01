function r = displayformat(typ, fmt)
    persistent settings;

    if (isempty(settings))
		settings.("matrix") = "html";
		settings.("bool matrix") = "html";
		settings.("complex matrix") = "html";
		settings.("range") = "html";
		settings.("scalar") = "text";
		settings.("complex scalar") = "text";
		settings.("struct") = "json";
		settings.("scalar struct") = "json";
    end

    if (nargin == 0)
        r = settings;
    elseif (nargin == 1)
        if (ischar(typ))
			if (strcmp(typ, "latex"))
				settings.("matrix") = "latex";
				settings.("bool matrix") = "latex";
				settings.("complex matrix") = "latex";
				settings.("range") = "latex";
				settings.("scalar") = "latex";
				settings.("complex scalar") = "latex";
                settings.("struct") = "json";
                settings.("scalar struct") = "json";
			elseif (strcmp(typ, "html"))
				settings.("matrix") = "html";
				settings.("bool matrix") = "html";
				settings.("complex matrix") = "html";
				settings.("range") = "html";
				settings.("scalar") = "text";
				settings.("complex scalar") = "text";
                settings.("struct") = "json";
                settings.("scalar struct") = "json";
            elseif (isfield(settings, typ))
                r = settings.(typ);
                return;
            else
                r = "";
            end
        else
            error("TYPE must be a string");
        end
    elseif (nargin == 2)
        if (ischar(typ) && ischar(fmt))
            settings.(typ) = fmt;
        else
            error("Parameters must be strings")
        end
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
