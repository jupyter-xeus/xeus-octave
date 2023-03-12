classdef interact < xvbox
    methods
        function obj = interact(_fhandle, varargin)
            if nargin < 2
                print_usage();
            end

            if ! is_function_handle(_fhandle)
                error("FHANDLE must be a function handle");
            end

            obj.fhandle = _fhandle;
            obj.create_widgets(varargin{:});
            obj.register_observers();

            obj.Children{end+1} = xoutput;

            obj.on_update();
        end
    end

    methods (Access = private)
        function create_widgets(obj, varargin)
            for i = 1:numel(varargin)
                param = varargin{i};

                if typeinfo(param) != "cell" || numel(param) < 2
                    error("PARAMS must be cell arrays with at least 2 elements");
                end

                description = param{1};

                if numel(param) == 2
                    arg = param{2};

                    switch typeinfo(arg)
                        case "scalar"
                            w = xslider;
                            w.Description = description;
                            w.Value = arg;
                            obj.Children{end+1} = w;
                    end
                elseif numel(param) == 3
                    min = param{2};
                    max = param{3};

                    switch typeinfo(min)
                        case "scalar"
                            w = xslider;
                            w.Description = description;
                            w.Min = min;
                            w.Max = max;
                            obj.Children{end+1} = w;
                    end
                elseif numel(param) == 4
                    min = param{2};
                    max = param{3};
                    arg = param{4};

                    switch typeinfo(arg)
                        case "scalar"
                            w = xslider;
                            w.Description = description;
                            w.Min = min;
                            w.Max = max;
                            w.Value = arg;
                            obj.Children{end+1} = w;
                    end
                end
            end
        end

        function register_observers(obj)
            for i = 1:numel(obj.Children)
                obj.Children{i}.observe_Value(@(w) obj.on_update());
            end
        end

        function on_update(obj)
            % Prepare arguments to pass to the interactive function
            args = cellfun(@(w) w.Value, obj.Children(1:end-1), "UniformOutput", false);
            % Clear current figure
            set (groot, "currentfigure", []);
            % Get current figures
            figures_before = findobj("type", "figure");
            % Save current graphics toolkit
            tksv = graphics_toolkit();
            % Change graphics toolkit
            graphics_toolkit("__interact");
            % Set output capture
            obj.Children{end}.capture;
            % Clear output
            clear_output(true);
            % Call function
            obj.fhandle(args{:})
            % Get current figures
            figures_after = findobj("type", "figure");
            % Get new figures
            figures_new = setdiff(figures_after, figures_before);
            % Draw any figure
            drawnow;
            % Release output
            obj.Children{end}.release;
            % Close figures
            delete(figures_new);
            % Restore graphics toolkit
            graphics_toolkit(tksv);
        end
    end

    properties (SetAccess = private, GetAccess = public)
        fhandle
    end
end
