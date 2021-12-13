import unittest
import requests

import utils as u

CONFIG = "tests/configs/body_limit.conf"

class TestConfigBodyLimitServerA(unittest.TestCase):
	pid = 0
	fd = 0

	url = "http://localhost:8000/"
	upload_url = url + "uploads/"
	file_url = url + "uploads/file_a"
	def_payload = {}
	def_headers = {}

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_entrypoint(self):
		response = requests.request("GET", self.url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("index.html"))

	def test_autoindex(self):
		response = requests.request("GET", self.upload_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)

	def test_1_upload_text_plain(self):
		payload = u.get_random_string(5)
		headers = {
			'Content-Type': 'text/plain'
		}
		response = requests.request("POST", self.file_url,
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, payload)

	def test_2_reupload_text_plain(self):
		payload = u.get_random_string(5)
		headers = {
			'Content-Type': 'text/plain'
		}
		response = requests.request("POST", self.file_url,
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 409)

	def test_3_delete_text_plain(self):
		response = requests.request("DELETE", self.file_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 404)

	def test_upload_at_limit(self):
		payload = u.get_random_string(10)
		headers = {
			'Content-Type': 'text/plain'
		}
		response = requests.request("POST", self.file_url + "limit",
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url + "limit",
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, payload)

		response = requests.request("DELETE", self.file_url + "limit",
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url + "limit",
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 404)

	def test_upload_over_limit(self):
		payload = u.get_random_string(100)
		headers = {
			'Content-Type': 'text/plain'
		}
		response = requests.request("POST", self.file_url,
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 413)

		response = requests.request("GET", self.file_url,
			headers=self.def_headers, data=self.def_payload)
		
		self.assertEqual(response.status_code, 404)

class TestConfigBodyLimitB(unittest.TestCase):
	pid = 0
	fd = 0

	url = "http://localhost:8000/"
	upload_url = "http://localhost:8000/uploads/"
	file_url = "http://localhost:8000/uploads/file_b"
	def_payload = {}
	def_headers = {
		'Host': 'webserv2'
	}

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_entrypoint(self):
		response = requests.request("GET", self.url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, u.get_html_file("index2.html"))

	def test_autoindex(self):
		response = requests.request("GET", self.upload_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)

	def test_1_upload_text_plain_config_body_limit_b(self):
		payload = u.get_random_string(50000)
		headers = {
			'Content-Type': 'text/plain'
		}
		headers['Host'] = self.def_headers['Host']
		response = requests.request("POST", self.file_url,
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, payload)

	def test_2_reupload_text_plain(self):
		payload = u.get_random_string(50000)
		headers = {
			'Content-Type': 'text/plain'
		}
		headers['Host'] = self.def_headers['Host']

		response = requests.request("POST", self.file_url,
			headers=headers, data=payload)
		self.assertEqual(response.status_code, 409)

	def test_3_delete_text_plain(self):
		response = requests.request("DELETE", self.file_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url,
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 404)

	def test_upload_at_limit(self):
		payload = u.get_random_string(100000)
		headers = {
			'Content-Type': 'text/plain'
		}
		headers['Host'] = self.def_headers['Host']
		response = requests.request("POST", self.file_url + "limit",
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url + "limit",
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 200)
		self.assertEqual(response.text, payload)

		response = requests.request("DELETE", self.file_url + "limit",
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 204)

		response = requests.request("GET", self.file_url + "limit",
			headers=self.def_headers, data=self.def_payload)

		self.assertEqual(response.status_code, 404)

	def test_upload_over_limit(self):
		payload = u.get_random_string(110000)
		headers = {
			'Content-Type': 'text/plain'
		}
		headers['Host'] = self.def_headers['Host']
		response = requests.request("POST", self.file_url,
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 413)

		response = requests.request("GET", self.file_url,
			headers=self.def_headers, data=self.def_payload)
		
		self.assertEqual(response.status_code, 404)

if __name__ == '__main__':
	unittest.main()