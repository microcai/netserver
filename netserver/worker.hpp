/*
*
* Copyright (C) 2009 jack.wgm, microcai.
* For conditions of distribution and use, see copyright notice 
* in (http://code.google.com/p/netsever/source/browse/trunk/COPYING)
*
* Author: jack.wgm
* Email:  jack.wgm@gmail.com
*/

#ifndef WORKER_H__
#define WORKER_H__

#if _MSC_VER
#pragma once
#endif

#include "jobqueue.hpp"

template <typename Job>
class worker
{
public:
	worker(jobqueue<Job>& _jobqueue, std::size_t _maxthreads = 4);
	~worker(void);

public:
	void run();
	void stop();
	virtual bool work(Job& task) = 0;       // 派生类需要重载此虚函数,以完成工作.

protected:
	void workloop();                    // 工作循环.

private:
	std::vector<boost::shared_ptr<boost::thread> > threads_;
	boost::mutex mutex_;
	std::size_t maxthreads_;
	jobqueue<Job>& jobqueue_;
	bool exitthread;
}; 

template <typename Job>
worker<Job>::worker(jobqueue<Job>& _jobqueue, std::size_t _maxthreads/* = 4*/) :
jobqueue_(_jobqueue),
maxthreads_(_maxthreads),
exitthread(false)
{
}

template <typename Job>
worker<Job>::~worker(void)
{
}

template <typename Job>
void worker<Job>::run()
{
	try
	{
		for (std::size_t i=0; i<maxthreads_; ++i) {
			boost::shared_ptr<boost::thread> _thread(new boost::thread(
				boost::bind(&worker::workloop, this)));
			threads_.push_back(_thread);
		}

		for (std::size_t i=0; i<maxthreads_; ++i) {
			threads_[i]->join();
		}
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR INFO:" << e.what() << std::endl;
	}
}

template <typename Job>
void worker<Job>::stop()
{
	exitthread = true;
	jobqueue_.notify_all();
}

template <typename Job>
void worker<Job>::workloop()               // 所有工作在些完成.
{
	do 
	{
		Job task_ = jobqueue_.getjob();

		if (work(task_))
			continue;
		else
			break;

	} while (!exitthread);
}

#endif // WORKER_H__