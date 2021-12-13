import unittest
import requests

import utils as u

CONFIG = "tests/configs/default.conf"

class TestDefault(unittest.TestCase):
	pid, fd = 0, 0

	@classmethod
	def setUpClass(cls):
		cls.pid, cls.fd = u.start_server(CONFIG)

	@classmethod
	def tearDownClass(cls):
		if cls.fd and cls.pid:
			u.stop_server(cls.pid, cls.fd)

	def test_autoindex(self):
		r = requests.get("http://localhost:8000/html/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/</h3>", r.text)

	# def test_root_index(self):
	# def test_root_404(self):
	# def test_404(self):
	# def test_ping(self):
	def test_ping_autoindex(self):
		r = requests.get("http://localhost:8000/ping/")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("ping/index.html"))
	# def test_ping_method(self):
	# def test_redirect(self):

	def test_html_autoindex(self):
		r = requests.get("http://localhost:8000/html/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/</h3>", r.text)

	# def test_html_index(self):
	# def test_html_404_page(self):
	# def test_html_404_error(self):
	def test_cgi_autoindex(self):
		r = requests.get("http://localhost:8000/html/cgi/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/cgi/</h3>", r.text)
	# def test_cgi_login_form(self):
	def test_cgi_php_autoindex(self):
		r = requests.get("http://localhost:8000/cgi/php/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/cgi/php/</h3>", r.text)

	# def test_cgi_php_UA(self):
	# def test_cgi_php_login(self):
	# def test_cgi_php_webserv_session(self):
	# def test_cgi_php_webserv_session_user(self):

	def test_cgi_python_autoindex(self):
		r = requests.get("http://localhost:8000/html/cgi/python/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/cgi/python/</h3>", r.text)

	# def test_cgi_python_index(self):
	# def test_cgi_python_serv_error(self):
	# def test_uploads_autoindex(self):
	# def test_file_1_upload(self):
	# def test_file_2_delete(self):
	# def test_multiple_file_1_upload(self):
	# def test_multiple_file_2_delete(self):

if __name__ == '__main__':
	unittest.main()