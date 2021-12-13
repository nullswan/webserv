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

	def test_root_index(self):
		r = requests.get("http://localhost:8000/index.html")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("index.html"))

	def test_root_404_page(self):
		r = requests.get("http://localhost:8000/404.html")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("404.html"))

	def test_404_error(self):
		r = requests.get("http://localhost:8000/html/not_found")
		self.assertEqual(r.status_code, 404)
		self.assertIn("Not Found", r.text)

	def test_ping(self):
		r = requests.get("http://localhost:8000/ping/index.html")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("ping/index.html"))

	def test_ping_autoindex(self):
		r = requests.get("http://localhost:8000/ping/")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("ping/index.html"))

	def test_ping_method(self):
		url = "http://localhost:8000/ping/file"
		payload = u.get_random_string(100)
		headers = {
			'Content-Type': 'text/plain'
		}
		response = requests.request("POST", url,
			headers=headers, data=payload)

		self.assertEqual(response.status_code, 405)
		self.assertIn("Method Not Allowed", response.text)

	def test_redirect(self):
		r = requests.request("GET", 
			"http://localhost:8000/google/", allow_redirects=False)
		self.assertEqual(r.status_code, 301)
		self.assertEqual(len(r.text), 0)
		self.assertEqual(r.headers['location'], "http://google.com")

	def test_html_autoindex(self):
		r = requests.get("http://localhost:8000/html/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/</h3>", r.text)

	def test_html_index(self):
		r = requests.get("http://localhost:8000/html/index.html")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("index.html"))

	def test_html_404_page(self):
		r = requests.get("http://localhost:8000/html/404.html")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("404.html"))

	def test_html_404_error(self):
		r = requests.get("http://localhost:8000/html/not_found")
		self.assertEqual(r.status_code, 404)
		self.assertIn("Not Found", r.text)

	def test_cgi_autoindex(self):
		r = requests.get("http://localhost:8000/html/cgi/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/cgi/</h3>", r.text)

	def test_cgi_login_form(self):
		r = requests.get("http://localhost:8000/cgi/php/login_form.html")
		self.assertEqual(r.status_code, 200)
		self.assertEqual(r.text, u.get_html_file("cgi/php/login_form.html"))

	def test_cgi_php_autoindex(self):
		r = requests.get("http://localhost:8000/cgi/php/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/cgi/php/</h3>", r.text)

	def test_cgi_php_UA(self):
		r = requests.get("http://localhost:8000/cgi/php/user_agent.php")
		self.assertEqual(r.status_code, 200)
		self.assertIn("python-requests", r.text)
	
		headers = {
			'User-Agent': 'so much custom ua wow'
		}
		r = requests.get("http://localhost:8000/cgi/php/user_agent.php", headers=headers)

		self.assertEqual(r.status_code, 200)
		self.assertIn("so much custom ua wow", r.text)

	def test_cgi_php_login(self):
		url = "http://localhost:8000/cgi/php/login.php"
		payload = {
			'user': 'admin',
			'pwd': 'pass'	
		}
		response = requests.request("POST", url, data=payload)
		self.assertEqual(response.status_code, 200)
		self.assertIn(payload['user'], response.text)
		self.assertIn('Logged-in', response.text)
		self.assertIn('POST', response.text)

	def test_cgi_php_webserv_session(self):
		url = "http://localhost:8000/cgi/php/webserv_session.php"
		r = requests.get(url)

		self.assertEqual(r.status_code, 200)
		self.assertIn('WEBSERV_SESSION_ID', r.headers['Set-Cookie'])

		sid = r.headers['Set-Cookie'].split('=')[1].split(';')[0]
		headers = {
			'Cookie': 'WEBSERV_SESSION_ID=' + sid
		}
		r = requests.get(url, headers=headers)

		self.assertEqual(r.status_code, 200)
		self.assertIn(sid, r.text)

	def test_cgi_php_webserv_session_user(self):
		url = "http://localhost:8000/cgi/php/webserv_session.php"
		r = requests.get(url)

		self.assertEqual(r.status_code, 200)
		self.assertIn('WEBSERV_SESSION_ID', r.headers['Set-Cookie'])

		sid = r.headers['Set-Cookie'].split('=')[1].split(';')[0]
		headers = {
			'Cookie': 'WEBSERV_SESSION_ID=' + sid
		}
		r = requests.get(url+"?WEBSERV_USER=admin", headers=headers)

		self.assertEqual(r.status_code, 200)
		self.assertIn("WEBSERV_USER = admin", r.text)
		self.assertIn(sid, r.text)

	def test_cgi_python_autoindex(self):
		r = requests.get("http://localhost:8000/html/cgi/python/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/cgi/python/</h3>", r.text)

	def test_cgi_python_index(self):
		r = requests.get("http://localhost:8000/cgi/python/index.py")
		self.assertEqual(r.status_code, 200)
		self.assertIn("First CGI Program", r.text)

	def test_cgi_python_serv_error(self):
		r = requests.get("http://localhost:8000/cgi/python/serv_error.py")
		self.assertEqual(r.status_code, 500)

	def test_uploads_autoindex(self):
		r = requests.get("http://localhost:8000/uploads/")
		self.assertEqual(r.status_code, 200)
		self.assertIn("<h3>Index of tests/www/html/uploads/</h3>", r.text)

	# def test_multiple_file_1_upload(self):
	# def test_multiple_file_2_delete(self):

if __name__ == '__main__':
	unittest.main()