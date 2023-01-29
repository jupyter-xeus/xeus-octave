#
# Copyright (C) 2023 Giulio Girardi.
#
# This file is part of xeus-octave.
#
# xeus-octave is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# xeus-octave is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
#
# along with xeus-octave.  If not, see <http://www.gnu.org/licenses/>.
#

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
					doc=klass.__doc__,
					traits=traits.items()
				))
	else:
		widget_files = [d[2].removesuffix('Model') + '.m' for d, _ in widget_list]
		print(';'.join(widget_files), end='')
