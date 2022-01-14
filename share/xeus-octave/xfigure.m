classdef xfigure < ximage
	properties
		h = 0;
	endproperties

	methods
		function x = xfigure(varargin)
			x.h = figure("__plot_stream__", x.__pointer__, varargin{:});
		endfunction
	endmethods
endclassdef