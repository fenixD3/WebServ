#!/usr/local/bin/python3
import requests

URL = "http://localhost:8080"
FILE_CONTENT = "test file"

RED = "\033[31m"
GREEN = "\033[32m"
RESET = "\033[39m"

def test_get_method(url = "", expected = ""):
	req = requests.get(URL + url)
	if req.status_code != 200:
		return f"Incorrect status: {req.status_code}"
	if req.text != expected:
		return f"Incorrect body: {req.text}"
	if int(req.headers['Content-Length']) != len(expected):
		target_len = len(expected)
		return f"Incorrect Content-Length: {req.headers['Content-Length']}, expected: {target_len}"
	return ""

def test_head_method(url = "", expected = ""):
	req = requests.head(URL + url)
	if req.status_code != 200:
		return f"Incorrect status: {req.status_code}"
	if req.text != "":
		return f"non-zero body: {req.text}"
	if int(req.headers['Content-Length']) != len(expected):
		target_len = len(expected)
		return f"Incorrect Content-Length: {req.headers['Content-Length']}, expected: {target_len}"
	return ""

def test_get_method_1():
	return test_get_method("", "Hello fim index.html")
def test_get_method_2():
	return test_get_method("/dir/page.html", "page in dir")
def test_get_method_3():
	return test_get_method("/dir/", "index in dir")
def test_get_method_4():
	return test_get_method("/dir", "index in dir")

def test_head_method_1():
	return test_get_method("", "Hello fim index.html")
def test_head_method_2():
	return test_head_method("/dir/page.html", "page in dir")
def test_head_method_3():
	return test_get_method("/dir/", "index in dir")
def test_head_method_4():
	return test_get_method("/dir", "index in dir")

def test_post_and_delete_method_1():
	return test_post_and_delete_method("/upload/")


def run_test(test_method):
	try:
		result = test_method()
	except BaseException as e:
		result = f"Error in test: {e}"
	if len(result):
		print(f"{test_method.__name__:30}: \t {RED}❌ {result}{RESET}")
	else:
		print(f"{test_method.__name__:30}: \t {GREEN}✅ OK!{RESET}")

	

def test_post_and_delete_method(url = "", expected = ""):
	# UPLOAD FILE
	req = requests.post(URL + url, FILE_CONTENT, headers={"Content-Disposition": "attachment; filename=\"test.test\""})
	if req.status_code != 201:
		return f"Incorrect status: {req.status_code}"
	
	# LOAD FILE
	req = requests.get(URL + url + "/test.test")
	if req.status_code != 200:
		return f"Incorrect status: {req.status_code}"
	if req.text != FILE_CONTENT:
		return f"Incorrect body: {req.text}"
	
	# DELETE FILE
	req = requests.delete(URL + url + "/test.test")
	if req.status_code != 201:
		return f"Incorrect status: {req.status_code}"
	

	return ""
	


if __name__ == "__main__":
	run_test(test_get_method_1)
	run_test(test_get_method_2)
	run_test(test_get_method_3)
	run_test(test_get_method_4)
	run_test(test_head_method_1)
	run_test(test_head_method_2)
	run_test(test_head_method_3)
	run_test(test_head_method_4)
	run_test(test_post_and_delete_method_1)




