<%!
from traitlets import (
    CaselessStrEnum,
    Unicode,
    Tuple,
    List,
    Bool,
    CFloat,
    Float,
    CInt,
    Int,
    Instance,
    Dict,
    Bytes,
    Any,
    Union,
)

from ipywidgets.widgets import Widget
from ipywidgets.widgets.trait_types import TypedTuple, CByteMemoryView, InstanceDict
%>

classdef ${widget_name} < __xwidget_internal__
	properties (Sync = true)
% for trait_name, trait in traits:
	% if trait.help:
		# ${trait.help}
	% endif
	% if trait.default() is None:
		${trait_name} = []; # null
	% elif type(trait) in (CaselessStrEnum, Unicode, CUnicode, Color, NumberFormat):
		${trait_name} = "${trait.default()}";
	% elif type(trait) in (CFloat, Float, CInt, Int):
		${trait_name} = ${trait.default()};
	% elif type(trait) is Bool:
		${trait_name} = ${str(trait.default()).lower()};
	% else:
		${trait_name} = []; # null
	% endif
% endfor
	endproperties

	methods
		function obj = ${widget_name}()
% for trait_name, trait in traits:
	% if type(trait) in (Instance, InstanceDict) and issubclass(trait.klass, Widget):
	<%
		for data, klass in widget_list:
			if klass == trait.klass:
				instance_name = data[2].removesuffix("Model")
				break
	%>
			obj.${trait_name} = xwidgets.${instance_name};
	% elif (type(trait) is TypedTuple and type(trait._trait) is Unicode) or (type(trait) is List and type(trait._trait) is Unicode):
		% if trait.default() is not None:
			obj.${trait_name} = {${','.join([f'"{v}"' for v in trait.default()])}};
		% endif
	% elif type(trait) is TypedTuple and type(trait._trait) is Instance and issubclass(trait._trait.klass, Widget):
			obj.${trait_name} = {};
	% elif type(trait) is CByteMemoryView:
			obj.${trait_name} = uint8([]);
	% endif
% endfor
		endfunction

% for trait_name, trait in traits:
		function set.${trait_name}(obj, value)
	% if not trait.allow_none:
			if isnull(value)
				error("input must not be null")
			end
	% endif
	% if type(trait) is CaselessStrEnum:
			mustBeMember(value, {${'"' + '","'.join(trait.values) + '"'}});
			if true
	% elif type(trait) is Unicode:
			if !ischar(value)
				error("input must be a string");
			else
	% elif type(trait) is CUnicode:
			if isnumeric(value)
				obj.${trait_name} = num2str(value);
			elseif !ischar(value)
				obj.${trait_name} = disp(value);
			else
	% elif type(trait) is Float:
			if !isreal(value) && !isscalar(value)
				error("input must be a real scalar");
			elseif isinteger(value)
				obj.${trait_name} = double(value);
			else
	% elif type(trait) is CFloat:
			if ischar(value)
				obj.${trait_name} = str2num(value);
			elseif !isreal(value) && !isscalar(value)
				error("input must be a real scalar");
			elseif isinteger(value)
				obj.${trait_name} = double(value);
			else
	% elif type(trait) is Int:
			if round(value) == value
				obj.${trait_name} = int64(value);
			elseif !isreal(value) && !isscalar(value) && !isinteger(value)
				error("input must be a real scalar integer");
			else
	% elif type(trait) is CInt:
			if !isreal(value) && !isscalar(value)
				error("input must be a real scalar");
			elseif isinteger(value)
				obj.${trait_name} = int64(value);
			else
	% elif type(trait) is Bool:
			if !islogical(value)
				error("input must be a logical value");
			else
	% elif type(trait) in (Instance, InstanceDict) and issubclass(trait.klass, Widget):
	<%
		for data, klass in widget_list:
			if klass == trait.klass:
				instance_name = data[2].removesuffix("Model")
				break
	%>
			if !isa(value, "xwidgets.${instance_name}")
				error("input must be instance of xwidgets.${instance_name}");
			else
	% elif type(trait) is TypedTuple and type(trait._trait) is Unicode:
			if !iscellstr(value)
				error ("input must be an array of strings");
			else
	% elif type(trait) is TypedTuple and type(trait._trait) is Instance and issubclass(trait._trait.klass, Widget):
			if !iscell(value) || !all(cellfun(@(c) isa(c, "__xwidget_internal__"), value))
				error ("input must be an array of widgets");
			else
	% elif type(trait) is CByteMemoryView:
			if !strcmp(typeinfo(value), "uint8 matrix")
				obj.${trait_name} = uint8(value);
			else
	% else:
			if true
				warning("Property of type ${type(trait)} is not validated");
	% endif
				obj.${trait_name} = value;
			end
		endfunction

% endfor
% for trait_name, trait in traits:
	% if type(trait) in (Int, CInt, Float, CFloat):
		function value = get.${trait_name}(obj)
			if isnull(obj.${trait_name})
				value = [];
			else
		% if type(trait) in (Int, CInt):
				value = int64(obj.${trait_name});
		% elif type(trait) in (Float, CFloat):
				value = double(obj.${trait_name});
		% endif
			end
		endfunction

	% endif
% endfor
	endmethods
endclassdef
