import unittest
import requests

import utils as u

CONFIG = "tests/configs/full.conf"

class TestRedir(unittest.TestCase):
	pid, fd = 0, 0

	def_payload = {}
	def_headers = {
		'Host': 'redir_serv'
	}

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_redir_autoindex(self):
		url = "http://localhost:8080/"

		response = requests.request("GET", url, headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 404)

	def test_redir_index(self):
		url = "http://localhost:8080/index.html"
		
		response = requests.request("GET", url, headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 404)

	def test_redir_httpbin(self):
		url = "http://localhost:8080/httpbin/"
		target = "http://httpbin.org/"

		response = requests.request("GET", url, 
			headers=self.def_headers, data=self.def_payload)
		self.assertEqual(response.status_code, 200)

		req2 = requests.request("GET", target)
		self.assertEqual(response.text, req2.text)

		response = requests.request("GET", url, 
			headers=self.def_headers, data=self.def_payload, allow_redirects=False)

		self.assertEqual(response.status_code, 301)
		self.assertEqual(response.headers['Location'], target)

	def test_redir_google(self):
		url = "http://localhost:8080/google/"
		target = "http://google.com/"

		response = requests.request("GET", url, 
			headers=self.def_headers, data=self.def_payload, allow_redirects=False)

		self.assertEqual(response.status_code, 301)
		self.assertEqual(response.headers['Location'], target)

	def test_redir_stackoverflow(self):
		url = "http://localhost:8080/stackoverflow/"
		target = "http://stackoverflow.com/"

		response = requests.request("GET", url, 
			headers=self.def_headers, data=self.def_payload, allow_redirects=False)

		self.assertEqual(response.status_code, 301)
		self.assertEqual(response.headers['Location'], target)

	def test_redir_42(self):
		url = "http://localhost:8080/42/"
		target = "http://intra.42.fr/"

		response = requests.request("GET", url, headers=self.def_headers, data=self.def_payload)
		self.assertEqual(response.status_code, 200)

		response = requests.request("GET", url, 
			headers=self.def_headers, data=self.def_payload, allow_redirects=False)

		self.assertEqual(response.status_code, 301)
		self.assertEqual(response.headers['Location'], target)

class TestServerA(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_server_index(self):
		url = "http://localhost:8080"
		payload = {}
		headers = {
			'Host': 'webservA',
		}

		response = requests.request("GET", url, headers=headers, data=payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservA/index.html"))
		
	def test_server_no_host(self):
		url = "http://localhost:8080"
		payload = {}
		headers = {}

		response = requests.request("GET", url, headers=headers, data=payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservA/index.html"))

class TestServerB(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_server_index(self):
		url = "http://localhost:8080"
		payload= {}
		headers = {
			'Host': 'webservB',
		}

		response = requests.request("GET", url, headers=headers, data=payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservB/index.html"))

	def test_server_index_no_host(self):
		url = "http://localhost:8080"
		payload = {}
		headers = {}

		response = requests.request("GET", url, headers=headers, data=payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("webservA/index.html"))

if __name__ == '__main__':
	unittest.main()