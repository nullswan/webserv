import cgi, cgitb              
  
form = cgi.FieldStorage()      
username = form.getvalue('username')    
password = form.getvalue('password')   
  
print ("Content-type:text/html\n")
print ("<html>")
print ("<head>")
print ("<title>CGI Python</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello, %s your password is %s.</h2>" 
       % (username, password))
  
print ("</body>")
print ("</html>")