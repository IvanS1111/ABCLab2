#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include<atomic>
#include <functional>
#include <ctime>
#include<Windows.h>
using namespace std;

class ThreadSafe
{
protected:
	atomic_int atomic_index = 0;
	int index = 0;
	int NumTasks = 0;
	int NumThreads = 0;
	vector<int> v;
	mutex mtx;

public:
	int getNumThreads()
	{
		return NumThreads;
	}

	void printVector()
	{
		for (int i = 0; i < v.size(); i++)
			cout << v[i] << " ";
		cout << endl;
	}

	void init(int numTasks, int numThreads)
	{
		NumTasks = numTasks;
		NumThreads = numThreads;

		for (int i = 0; i < NumTasks; i++)
			v.push_back(0);
	}
protected:
	bool addArray(int indexbuf) 
	{
		if (indexbuf < NumTasks)
		{
			v[indexbuf]++;
			Sleep(1);
			return true;
		}
		else
			return false;
	}
	void threadFunction(int param)
	{
		while (true)
		{
			int indexbuf;
			if (param == 0)
			{
				mtx.lock();
				indexbuf = index;
				index++;
				mtx.unlock();
			}
			else
			{
				indexbuf = atomic_index.fetch_add(1);
			}
			if (addArray(indexbuf) == false)
				break;
		}
	}
};
class ThreadSafeMutex : public ThreadSafe
{
public:
	function<void()> getthreadFunction = bind(&ThreadSafeMutex::threadFunction, this, 0);

};

class ThreadSafeAtomic : public ThreadSafe
{
public:
	function<void()> getthreadFunction = bind(&ThreadSafeAtomic::threadFunction, this, 1);
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
	//***************Mutex****************
	ThreadSafeMutex myThreadMutex;
	myThreadMutex.init(1024, 32);

	ThreadsCreator creator;

	unsigned int start = clock();

	vector<thread> threadListMutex; 
	creator.getThreads(myThreadMutex.getNumThreads(), threadListMutex, myThreadMutex.getthreadFunction);
	creator.joinThreads(threadListMutex);

	unsigned int finish = clock() - start;
	cout << double(finish)/CLK_TCK << endl;

	myThreadMutex.printVector();

	//***************Atomic****************

	/*ThreadSafeAtomic myThreadAtomic;
	myThreadAtomic.init(1024, 32);

	ThreadsCreator creator;

	unsigned int start = clock();

	vector<thread> threadListAtomic;
	creator.getThreads(myThreadAtomic.getNumThreads(), threadListAtomic, myThreadAtomic.getthreadFunction);
	creator.joinThreads(threadListAtomic);

	unsigned int finish = clock() - start;
	cout << double(finish) / CLK_TCK << endl;

	myThreadAtomic.printVector();*/

	return 0;
}
