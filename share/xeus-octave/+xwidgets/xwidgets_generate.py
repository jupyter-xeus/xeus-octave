import sys

from mako.template import Template
from ipywidgets import widgets

if __name__ == "__main__":
	widget_list = sorted(widgets.Widget._widget_types.items())

	if len(sys.argv) == 2:
		widget_template = Template(filename=sys.argv[1])

		for data, klass in widget_list:
			widget_name = data[2].removesuffix("Model")

			# Instanciate dummy widget
			if issubclass(klass, widgets.widget_link.Link):
				widget = klass((widgets.IntSlider(), 'value'), (widgets.IntSlider(), 'value'))
			elif issubclass(klass, (widgets.SelectionRangeSlider, widgets.SelectionSlider)):
				widget = klass(options=[1])
			else:
				widget = klass()

			traits = widget.traits(sync=True)
			traits.pop("_view_count")

			with open(f"{widget_name}.m", "w") as out:
				out.write(widget_template.render( # type: ignore
					widget_list=widget_list,
					widget_name=widget_name,
					widget=widget,
					traits=traits.items()
				))
	else:
		widget_files = [d[2].removesuffix('Model') + '.m' for d, _ in widget_list]
		print(';'.join(widget_files), end='')
