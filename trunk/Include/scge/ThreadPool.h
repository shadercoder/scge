#ifndef ThreadPool_h__
#define ThreadPool_h__

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

private:
	ThreadPool(const ThreadPool &);
	ThreadPool &operator=(const ThreadPool &);
};

#endif // ThreadPool_h__