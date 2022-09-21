#############################################################################
# Copyright (c) 2022, Giulio Girardi
#
# Distributed under the terms of the GNU General Public License v3.
#
# The full license is in the file LICENSE, distributed with this software.
#############################################################################


import jupyter_kernel_test


class KernelTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xoctave"
    language_name = "Octave"
    code_hello_world = "disp('hello, world')"
    completion_samples = [
        {"text": "bessel", "matches": {"bessel", "besselh", "besseli","besselj","besselk","bessely"}},
        {"text": "facto", "matches": {"factor", "factorial"}},
        {"text": "nonsense_gsfkls", "matches": set()},
    ]
    complete_code_samples = [
        "1",
        "% comment",
        "function [m,s] = stat(x)\n"
        "    n = length(x);\n"
        "    m = sum(x)/n;\n"
        "    s = sqrt(sum((x-m).^2/n));\n"
        "end\n",
    ]
    code_inspect_sample = "plot"

    def test_stdout(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code=self.code_hello_world)
        self.assertEqual(output_msgs[0]["msg_type"], "stream")
        self.assertEqual(output_msgs[0]["content"]["name"], "stdout")
        self.assertEqual(output_msgs[0]["content"]["text"], "hello, world")
