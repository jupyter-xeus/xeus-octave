classdef xfigure < ximage
	properties
		h = 0;
		% Override default ximage width and height properties
		Width;
		Height;
	endproperties

	% Prevent access to some ximage properties
	properties (Access = private)
		Format;
		Value;
	endproperties

	methods
		function x = xfigure(varargin)
			x.h = figure("__plot_stream__", x.__pointer__, varargin{:});
		endfunction

		function set.Width(obj, w)
			p = get(obj.h, "position");
			p(3) = w;
			set(obj.h, "position", p)
		endfunction

		function set.Height(obj, h)
			p = get(obj.h, "position");
			p(4) = h;
			set(obj.h, "position", p)
		endfunction

		function w = get.Width(obj)
			p = get(obj.h, "position");
			w = p(3);
		endfunction

		function h = get.Height(obj)
			p = get(obj.h, "position");
			h = p(4);
		endfunction
	endmethods
endclassdef