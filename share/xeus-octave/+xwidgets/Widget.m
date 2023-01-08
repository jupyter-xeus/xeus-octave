classdef Widget < __xwidget_internal__
	methods
		function obj = Widget()
			obj = obj@__xwidget_internal__();
		end
	end

	properties
		_model_name = "WidgetModel";
		_view_name = "WidgetView";
		_model_module = "@jupyter-widgets/base";
		_view_module = "@jupyter-widgets/base";
		_model_module_version = "1.2.0";
		_view_module_version = "1.2.0";
	end
end