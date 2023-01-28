#############################################################################
# Copyright (c) 2022, Giulio Girardi
#
# Distributed under the terms of the GNU General Public License v3.
#
# The full license is in the file LICENSE, distributed with this software.
#############################################################################

import platform

import jupyter_kernel_test
import os
from pathlib import Path

class KernelTests(jupyter_kernel_test.KernelTests):

    kernel_name = "xoctave"
    language_name = "Octave"
    code_hello_world = "disp('hello, world')"
    code_stderr = "fprintf(2,'this is stderr')"
    code_generate_error = "garble"
    completion_samples = [
        {
            "text": "bessel",
            "matches": {
                "bessel",
                "besselh",
                "besseli",
                "besselj",
                "besselk",
                "bessely",
            },
        },
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
    code_display_data = [
        {'code': 'x = [0 1 2]', 'mime': 'text/html'},
    ]

    def test_pager(self):
        """Reimplementation of KernelTests.code_page_something.

        The test assume the help would be in "text/plain" but the XOctave help is in HTML.
        """
        self.flush_channels()
        reply, output_msgs = self.execute_helper("disp?")
        self.assertEqual(reply["content"]["status"], "ok")
        payloads = reply["content"]["payload"]
        self.assertEqual(len(payloads), 1)
        self.assertEqual(payloads[0]["source"], "page")
        mimebundle = payloads[0]["data"]
        self.assertIn("text/html", mimebundle)
        self.assertIn("disp", mimebundle["text/html"])

    def test_stdout(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code=self.code_hello_world)
        self.assertEqual(output_msgs[0]["msg_type"], "stream")
        self.assertEqual(output_msgs[0]["content"]["name"], "stdout")
        self.assertEqual(output_msgs[0]["content"]["text"], "hello, world")

    def test_stderr(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="does_not_exist()")
        self.assertEqual(output_msgs[0]["msg_type"], "error")
        self.assertIn("ename", output_msgs[0]["content"])
        self.assertIn("evalue", output_msgs[0]["content"])
        self.assertIn("traceback", output_msgs[0]["content"])
        self.assertIn("does_not_exist", "\n".join(output_msgs[0]["content"]["traceback"]))

    def test_plot_notebook(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="graphics_toolkit notebook; plot([])")

        content0 = output_msgs[0]["content"]
        self.assertEqual(output_msgs[0]["msg_type"], "display_data")

        content1 = output_msgs[1]["content"]
        self.assertEqual(output_msgs[1]["msg_type"], "update_display_data")
        self.assertTrue(len(content1["data"]["image/png"]) > 0)
        self.assertTrue(content1["metadata"]["image/png"]["height"] > 0)
        self.assertTrue(content1["metadata"]["image/png"]["width"] > 0)

        self.assertEqual(content0["transient"]["display_id"], content1["transient"]["display_id"])

    def test_plot_plotly(self):
        # On MacOS with conda-forge Octave, graphic commands show a FreeType warning
        # the first time
        if platform.system() == "Darwin":
            self.flush_channels()
            self.execute_helper(code="graphics_toolkit plotly; plot([])")

        self.flush_channels()
        reply, output_msgs = self.execute_helper(code="graphics_toolkit plotly; plot([])")

        content0 = output_msgs[0]["content"]
        self.assertEqual(output_msgs[0]["msg_type"], "display_data")

        content1 = output_msgs[1]["content"]
        self.assertEqual(output_msgs[1]["msg_type"], "update_display_data")
        app1 = content1["data"]["application/vnd.plotly.v1+json"]
        self.assertTrue(len(app1) > 0)
        self.assertTrue(len(app1["layout"]) > 0)
        self.assertTrue(app1["layout"]["height"] > 0)
        self.assertTrue(app1["layout"]["width"] > 0)

        self.assertEqual(content0["transient"]["display_id"], content1["transient"]["display_id"])

    def test_issue_68(self):
        """
        This tests that parsing of code with multiple errors is actually stopped
        on the first error
        """
        codes = [
            "*+3",
            "[][1]",
            "undefined_symbol_1\nundefined_symbol_2"
        ]

        for code in codes:
            self.flush_channels()
            reply, output_msgs = self.execute_helper(code=code)

            # Only the first error message should be present
            self.assertTrue(len(output_msgs) == 1)
            self.assertEqual(output_msgs[0]["msg_type"], "error")

    def test_octave_scripts(self):
        directory = Path(__file__).parent / 'octave'

        for file in directory.iterdir():
            if file.name.endswith(".m"):
                self.flush_channels()
                with file.open() as script:
                    code = script.read()
                    reply, output_msgs = self.execute_helper(code)

                    assert reply['content']['status'] == 'ok', code
