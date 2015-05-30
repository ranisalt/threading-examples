#include <condition_variable>
#include <mutex>

class semaphore {
public:
	semaphore(int count = 0) : _count(count) {
	}

	void p() {
		std::unique_lock<std::mutex> lock(_mutex);
		while (_count == 0) _condition.wait(lock);
		--_count;
	}
	
	void v() {
		std::unique_lock<std::mutex> lock(_mutex);
		++_count;
		_condition.notify_one();
	}

private:
	int _count;
	std::condition_variable _condition;
	std::mutex _mutex;
};
