classdef DescriptionWidget < xwidgets.DOMWidget & xwidgets.CoreWidget
	properties
		_model_name = "DescriptionModel";

		description = "";
		description_tooltip = "";
		style;
	end
end
