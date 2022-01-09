#############################################################################
# Copyright (c) 2022, Giulio Girardi
#
# Distributed under the terms of the GNU General Public License v3.
#
# The full license is in the file LICENSE, distributed with this software.
#############################################################################

import unittest
import jupyter_kernel_test


class KernelTests(jupyter_kernel_test.KernelTests):
    kernel_name = "xoctave"
    language_name = "octave"
    code_hello_world = "disp('hello, world')"
    code_stderr = "fprintf(2,'this is stderr')"
    code_generate_error = "garble"
    completion_samples = [{
        "text": "graphics_",
        "matches": ["graphics_toolkit"]
    }]
    code_display_data = [
        {'code': 'x = [0 1 2]', 'mime': 'text/html'},
    ]

    def test_plot(self):
        self.flush_channels()
        reply, output_msgs = self.execute_helper('sombrero')

        self.assertEqual(reply['content']['status'], 'ok')

        self.assertGreaterEqual(len(output_msgs), 1)
        found = False
        for msg in output_msgs:
            if msg['msg_type'] == 'update_display_data':
                found = True
            else:
                continue
            self.assertIn('image/png', msg['content']['data'])
        assert found, 'plot not found'


if __name__ == "__main__":
    unittest.main()
