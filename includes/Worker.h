
#include "VirtualServer.h"
#include "Response.h"
#include "Request.h"


class Worker
{
private:
	HttpResponse HttpGet(const HttpRequest& request, const Location& location);

public:
	Worker();
	~Worker();

	HttpResponse ProcessRequest(const HttpRequest& request, const Location& location);

};


