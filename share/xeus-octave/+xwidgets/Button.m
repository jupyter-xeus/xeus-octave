classdef Button < xwidgets.DOMWidget & xwidgets.CoreWidget
	methods
		function obj = Button()
			obj.style = xwidgets.ButtonStyle;
		end
	end

	properties
		_model_name = "ButtonModel";
		_view_name = "ButtonView";

		description = "";
		tooltip = "";
		disabled = false;
		icon = "";
		button_style = "";
		style;
	end
end