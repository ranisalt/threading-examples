#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

const uint8_t processes = 10;
const uint8_t runs = 10;

std::mutex critical, output;

void process(unsigned int id) {
	using std::chrono::milliseconds;
	using std::lock_guard;
	using std::mutex;
	using std::unique_lock;

	{
		lock_guard<mutex> lock{output};
		std::cout << "Processo " << id << " criado." << std::endl;
	}

	for (auto i = 0; i < runs; ++i) {
		std::this_thread::sleep_for(milliseconds(100));

		unique_lock<mutex> sem{critical};
		lock_guard<mutex> lock{output};
		std::cout << "Processo " << id << " entrou na região crítica." << std::endl;

		std::this_thread::sleep_for(milliseconds(100));

		std::cout << "Processo " << id << " saiu da região crítica." << std::endl;
	}
}

int main() {
	using std::thread;
	using std::vector;

	vector<thread> proc;
	for (auto i = 0; i < processes; ++i) {
		proc.emplace_back(process, i);
	}

	for (auto& p : proc) {
		p.join();
	}

	return 0;
}
