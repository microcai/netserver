// netserver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "net.hpp"
#include "worker.hpp"
#include "jobqueue.hpp"

class RunServer
{
public:
	RunServer(int port)
	: _jobqueueptr(new jobqueue<message>)
	, _workerptr(new worker<message>((*_jobqueueptr.get())))
	, _serverptr(new server(port, (*_jobqueueptr.get())))
	{
 		_listenthreadptr.reset(new boost::thread(boost::bind(&RunServer::RunNetListen, this)));
 		_workthreadptr.reset(new boost::thread(boost::bind(&RunServer::RunWorks, this)));
	}
	
	~RunServer()
	{
		_workerptr->stop(); // 停止工作.
		_serverptr->stop(); // 停止服务.
		_jobqueueptr->notify_all(); // 通知队列取消等待任务.

		_listenthreadptr->join();
		_workthreadptr->join();
	}

	void RunNetListen()
	{
		if (_serverptr)
			_serverptr->run();
	}

	void RunWorks()
	{
		if (_workerptr)
			_workerptr->run();
	}

private:
	boost::shared_ptr<boost::thread> _listenthreadptr;
	boost::shared_ptr<boost::thread> _workthreadptr;
	boost::shared_ptr<jobqueue<message> > _jobqueueptr;	
	boost::shared_ptr<worker<message> > _workerptr;	
	boost::shared_ptr<server> _serverptr;
};

int main(int argc, char* argv[])
{
	try
	{
		RunServer runServer1(atoi(argv[1]));                // 第一个server.
		RunServer runServer2(atoi(argv[1]) + 1);            // 第二个server.

		// 等待退出...
		std::string in;

		std::cout << "输入\'exit\'退出.\nprompt # ";
		for (;;)
		{
			char c = getchar();
			if (c == '\n')
			{
				if (in == "exit")
					break;
				else if(in != "")
					std::cout << "Bad command ! \n";


				std::cout << "prompt # ";
				in = "";				
			}
			else
			{
				in += c;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}

