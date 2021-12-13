import unittest
import requests

import utils as u

CONFIG = "tests/configs/vhosts.conf"

class TestVhosts(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_server_a_autoindex(self):
		url = "http://localhost:8000/"
		payload = {}
		headers = {}

		response = requests.request("GET", url, headers=headers, data=payload)
		self.assertEqual(response.status_code, 200)

	def test_server_b_autoindex(self):
		url = "http://localhost:8000/"
		payload = {}
		headers = {
			'Host': 'webservB',
		}

		response = requests.request("GET", url, headers=headers, data=payload)
		self.assertEqual(response.status_code, 200)

	def test_server_a(self):
		url = "http://localhost:8000/index.html"

		payload = {}
		headers = {
			'Host': 'webservA',
		}

		response = requests.request("GET", url, headers=headers, data=payload)
		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservA/index.html"))

		headers = {}

		response = requests.request("GET", url, headers=headers, data=payload)
		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservA/index.html"))

	def test_server_b(self):
		url = "http://localhost:8000/index.html"

		payload = {}
		headers = {
			'Host': 'webservB',
		}

		response = requests.request("GET", url, headers=headers, data=payload)
		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservB/index.html"))

if __name__ == '__main__':
	unittest.main()