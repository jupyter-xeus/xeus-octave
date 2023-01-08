classdef DescriptionWidget < xwidgets.DOMWidget & xwidgets.CoreWidget
	properties (Sync = true)
		_model_name = "DescriptionModel";

		description = "";
		description_tooltip = "";
		style;
	end
end
