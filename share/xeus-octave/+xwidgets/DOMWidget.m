classdef DOMWidget < xwidgets.Widget
	methods
		function obj = DOMWidget()
			obj.layout = xwidgets.Layout;
		end
	end

	properties (Sync = true)
		_model_name = "DOMWidgetModel";
		_view_name = "DOMWidgetView";

		layout;
		_dom_classes = {};
	end
end
