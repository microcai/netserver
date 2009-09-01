// netserver.cpp : ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ì¨Ó¦ï¿½Ã³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Úµã¡?
//

#include "stdafx.h"

#include "net.hpp"
#include "worker.hpp"
#include "jobqueue.hpp"

class MyWorker : public worker<message>
{
public:
	MyWorker(jobqueue<message>& _jobqueue, std::size_t _maxthreads = 4) :
	  worker<message>(_jobqueue, _maxthreads)
	  {}

	  virtual bool work(message& task)       // ÔÚÐ©Íê³ÉÊµ¼ÊÈÎÎñ.
	  {
		  session_ptr psession;
		  task.getsession(psession);
		  if (!psession) 
			  return false;

		  packMsgPtr MsgPtr = task.msg();

		  // ÓÃpsessionÍù¿Í»§¶Ë»Ø·¢Êý¾Ý;
		  switch (MsgPtr->MsgHead.type)
		  {
		  case MSG_USER_HEART:
			  // this->OnHeart(MsgPtr->heart);           // ´¦ÀíÐÄÌø.
			  break;
		  case MSG_USER_LOGON:
			  // this->OnLogon(MsgPtr->logon);           // ´¦ÀíµÇÂ½.
			  break;
		  default:
			  break;
		  }

		  return true;
	  }
};

class RunServer
{
public:
	RunServer(int port)
		: _jobqueueptr(new jobqueue<message>)
		, _workerptr(new MyWorker((*_jobqueueptr.get())))
		, _serverptr(new server(port, (*_jobqueueptr.get())))
	{
		_listenthreadptr.reset(new boost::thread(boost::bind(&RunServer::RunNetListen, this)));
		_workthreadptr.reset(new boost::thread(boost::bind(&RunServer::RunWorks, this)));
	}

	~RunServer() 
	{
		_workerptr->stop(); // Í£Ö¹.
		_serverptr->stop(); // Í£Ö¹.
		_jobqueueptr->notify_all(); // Í¨Öª.

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
	boost::shared_ptr<MyWorker> _workerptr;
	boost::shared_ptr<server> _serverptr;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			std::cerr << "usage: ./netserver <port>\n";
			return 1;
		}

		RunServer runServer1(atoi(argv[1]));                // ï¿½ï¿½Ò»ï¿½ï¿½server.
		RunServer runServer2(atoi(argv[1]) + 1);            // ï¿½Ú¶ï¿½ï¿½ï¿½server.

		// ï¿½È´ï¿½ï¿½Ë³ï¿½...
		std::string in;

		std::cout << "type 'exit' to exit.\nprompt # ";

		for (;;)
		{			
			char c = getchar();
			if (c == '\n' || c == EOF )
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

