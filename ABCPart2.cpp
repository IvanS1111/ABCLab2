<<<<<<< HEAD
﻿#include<iostream>
#include<queue>
#include <Windows.h>
#include <mutex>
#include<functional>
using namespace std;

class Queue
{
private:
	queue<uint8_t> q;
	int fixed_size = 100;
	mutex mtx;
public:
	void set_fixed_size(int value)
	{
		fixed_size = value;
	}
	int size()
	{
		return q.size();
	}
	int front()
	{
		return q.front();
	}
	void push(uint8_t value)
	{
		while (true)
		{
			mtx.lock();
			if (q.size() < fixed_size)
			{
				q.push(value);
				mtx.unlock();
				break;
			}
			mtx.unlock();
		}
	}

	bool pop(uint8_t& value)
	{
		mtx.lock();
		if (!q.empty())
		{
			value = q.front();
			q.pop();
			mtx.unlock();
			return true;
		}
		else
		{
			mtx.unlock();
			Sleep(1);
		}

		mtx.lock();
		if (!q.empty())
		{
			value = q.front();
			q.pop();
			mtx.unlock();
			return true;
		}
		else
		{
			mtx.unlock();
			return false;
		}

	}

};
class Queue2
{
private:
	queue<uint8_t> q;
	int fixed_size;
	mutex mtx;
	condition_variable cv;
public:
	void setSize(int value)
	{
		fixed_size = value;
	}
	int size()
	{
		return q.size();
	}
	void push(uint8_t value)
	{
		unique_lock<mutex> ul(mtx);
		cv.wait(ul, [this] {return q.size() < fixed_size; });

		q.push(value);
	}
	bool pop(uint8_t& value)
	{
		mtx.lock();
		if (!q.empty())
		{
			value = q.front();
			q.pop();
			cv.notify_one();
			mtx.unlock();
			return true;
		}
		else
		{
			cv.notify_one();
			mtx.unlock();
			Sleep(1);
		}

		mtx.lock();
		if (!q.empty())
		{
			value = q.front();
			q.pop();
			cv.notify_one();
			mtx.unlock();
			return true;
		}
		else
		{
			cv.notify_one();
			mtx.unlock();
			return false;
		}

	}
};
template<typename T>
class ProducerConsumer
{
protected:
	int TasksNum = 0;
	atomic_int ProducerNum = 0;
	Queue res;
	T q;
public:
	void setTaskNum(int value)
	{
		TasksNum = value;
	}
	void setSize(int value)
	{
		q.setSize(value);
	}
	void setProducerNum(int value)
	{
		ProducerNum = value;
	}
	int getProducerNum()
	{
		return ProducerNum;
	}
	void printAns()
	{
		uint8_t ans;
		int size = res.size();
		for (int i = 0; i < size; i++)
		{
			res.pop(ans);
			cout << int(ans) << " ";
		}
		cout << endl;
	}
	void threadProducer()
	{
		for (int i = 0; i < TasksNum; i++)
			q.push(1);
		ProducerNum.fetch_sub(1);
	}
	void threadConsumer()
	{
		uint8_t counter = 0;
		uint8_t value = 0;
		while (true)
		{
			if (q.pop(value))
				counter += value;
			else
				if (ProducerNum == 0)
				{
					res.push(counter);
					break;
				}
		}
	}

	function<void()> getthreadProducer = bind(&ProducerConsumer::threadProducer, this);
	function<void()> getthreadConsumer = bind(&ProducerConsumer::threadConsumer, this);
};
class ProducerConsumer2 : public ProducerConsumer<Queue2>
{
};
class ProducerConsumer1 : public ProducerConsumer<Queue>
{
};

class ThreadsCreator
{
public:
	void getThreads(int NumThreads, vector<thread>& threadList, function<void()> threadFunction)
	{
		for (int i = 0; i < NumThreads; i++)
			threadList.push_back(thread(threadFunction));
	}
	void joinThreads(vector<thread>& threads)
	{
		for (int i = 0; i < threads.size(); i++)
			threads[i].join();
	}
};

int main()
{

	/*ProducerConsumer1 pc;
	pc.setTaskNum(12);
	pc.setProducerNum(4);

	ThreadsCreator creator;
	vector<thread> threadsListProducer, threadsListConsumer;

	unsigned int start = clock();

	creator.getThreads(4, threadsListProducer, pc.getthreadProducer);
	creator.getThreads(4, threadsListConsumer, pc.getthreadConsumer);

	creator.joinThreads(threadsListProducer);
	creator.joinThreads(threadsListConsumer);

	unsigned int finish = clock() - start;
	cout << double(finish) / CLK_TCK << endl;

	pc.printAns();*/

	ProducerConsumer2 pc;

	pc.setTaskNum(1024);
	pc.setProducerNum(32);
	pc.setSize(1);

	ThreadsCreator creator;
	vector<thread> threadsListProducer, threadsListConsumer;

	unsigned int start = clock();

	creator.getThreads(pc.getProducerNum(), threadsListProducer, pc.getthreadProducer);
	creator.getThreads(8, threadsListConsumer, pc.getthreadConsumer);

	creator.joinThreads(threadsListProducer);
	creator.joinThreads(threadsListConsumer);

	unsigned int finish = clock() - start;
	cout << double(finish) / CLK_TCK << endl;

	pc.printAns(); 

	return 0;
=======
﻿#include <iostream>
#include <queue>
#include<mutex>
#include<Windows.h>
#include<chrono>
#include<functional>
using namespace std;

class ThreadsCreator
{
public:
	void getThreads(int NumThreads, vector<thread>& threadList, function<void()> threadFunction)
	{
		for (int i = 0; i < NumThreads; i++)
			threadList.push_back(thread(threadFunction));
	}
	void joinThreads(vector<thread>& threads)
	{
		for (int i = 0; i < threads.size(); i++)
			threads[i].detach();
	}
};
class ProducerConsumer
{
private:
	bool fixed_size = false;
	int size = 0, TaskNum = 0;
	mutex mtx;
	queue <uint8_t> q;
public:

	void setFixedSize(bool value)
	{
		fixed_size = value;
	}

	void setSize(int value)
	{
		size = value;
	}

	void threadProducer()
	{
		if (q.size() <= size || fixed_size == false)
		{
			while (q.size() < size)

				for (int i = 0; i < TaskNum; i++)
					q.push(1);
		}
		else
		{
			for (int i = 0; i < TaskNum; i++)
				q.push(1);
		}
	}
	function<void()> getthreadProducer = bind(&ProducerConsumer::threadProducer, this);

	void threadConsumer()
	{
		int counter = 0;
		while (q.size() == 0)
			Sleep(1);
		mtx.lock();
		counter += q.front();
		q.pop();
		mtx.unlock();
	}
	function<void()> getthreadConsumer = bind(&ProducerConsumer::threadConsumer, this);

};
int main()
{
	ProducerConsumer PC;
	ThreadsCreator tc;
	vector<thread> threadsListProducer, threadsListConsumer;
	tc.getThreads(4, threadsListProducer, PC.getthreadProducer);
	tc.getThreads(4, threadsListConsumer, PC.getthreadConsumer);
	tc.joinThreads(threadsListProducer);
	tc.joinThreads(threadsListConsumer);

    return 0;
>>>>>>> bc646fe8b27f78676e705a625337bbd54ebdf310
}