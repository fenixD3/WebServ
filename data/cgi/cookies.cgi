#!/usr/local/bin/python3

import sys
import os
def run():
	# data = sys.stdin.buffer.read()
	data = open(0).read()
	visited = 0
	if os.environ.get('HTTP_Cookie'):
		for cookie in os.environ['HTTP_Cookie'].split(';'):
			key, value = cookie.split('=');
			if key == "Visited":
				visited = int(value) + 1
	print("Status: 200 OK", end="\r\n")
	print("Server: Webserv.cgi", end="\r\n")
	print("Content-Type: text/html", end="\r\n")
	print("Set-Cookie: Expires = Tuesday, 31-Dec-2027 23:12:40 GMT;", end="\r\n")
	print(f"Set-Cookie: Visited = {visited};", end="\r\n")
	
	print("\r\n")
	print("<html>")
	print("<head>")
	print("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">")
	print("<title>Hello World</title>")
	print("</head>")
	print("<body>")
	
	print("<h3>")
	print("Hi! I am CGI Cookie script\n")
	print("</h3>")
	print(f"You visited our site {visited} times")
	
	print("</body>")
	print("</html>")

if __name__ == "__main__":
	run()


