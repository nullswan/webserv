import unittest
import requests

import utils as u

CONFIG = "tests/configs/error_page.conf"

class TestConfigErrorPage(unittest.TestCase):
	pid, fd = 0, 0
	
	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.pid and cls.fd:
			u.stop_server(cls.pid, cls.fd)

	def test_error_page_404(self):
		r = requests.get("http://localhost:8000/not_found")
		self.assertEqual(r.status_code, 404)
		self.assertEqual(r.text, u.get_html_file("custom_error.html"))

if __name__ == '__main__':
	unittest.main()