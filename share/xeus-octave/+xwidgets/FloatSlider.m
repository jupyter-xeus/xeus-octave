classdef FloatSlider < xwidgets.BoundedFloat
	methods
		function obj = FloatSlider()
			obj.style = xwidgets.SliderStyle;
		end
	end

	properties
		_view_name = "FloatSliderView";
		_model_name = "FloatSliderModel";

		step = 0.1;
		orientation = "horizontal";
		readout = true;
		readout_format = ".2f";
		continuous_update = true;
		disabled = false;
		style;
	end
end