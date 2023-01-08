classdef BoundedFloat < xwidgets.Float
	properties (Sync = true)
		max = 100;
		min = 0;
	end
end
