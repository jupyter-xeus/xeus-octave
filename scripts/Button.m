%[[[cog	from generator import * ]]]
%[[[end]]]
%[[[cog classdef() ]]]
%[[[end]]]

	properties (Sync = true)
		%[[[cog properties() ]]]
		%[[[end]]]
	end

	methods (Sync = true)

		%[[[cog constructor() ]]]
		%[[[end]]]
			%[[[cog initializers() ]]]
			%[[[end]]]
		endfunction

		function set._dom_classes(obj, value)
			if ! iscellstr(value)
				error ("input must be a cellstr");
			end
		endfunction

		function set.button_style(obj, value)
			mustBeMember(value, { "primary","success","info","warning","danger","" });
		endfunction

		function set.layout(obj, value)
			if ! isa(value, "xwidgets.Layout")
				error ("input must be instance of Layout");
			end
		endfunction

		function set.style(obj, value)
			if ! isa(value, "xwidgets.ButtonStyle")
				error ("input must be instance of ButtonStyle");
			end
		endfunction
	end

end
