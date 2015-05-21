#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

const uint8_t philosophers = 5;
const uint8_t iterations = 10;

std::mutex output;
std::vector<std::mutex> forks(philosophers);

void philosopher(unsigned int id) {
	using std::chrono::milliseconds;
	using std::lock_guard;
	using std::mutex;
	
	{
		lock_guard<mutex> lock{output};
		std::cout << "Filósofo " << id << " criado." << std::endl;
	}

	auto left_fork = id, right_fork = (id + 1) % philosophers;

	// the secret: a philosopher always gets its lowered-index forks (assuming they are ordered someway).
	// the last philosophers, at forks (n, 0) will grab 0 (locked) then 0. the n-1 philosopher will then
	// be able to grab fork n, preventing deadlock.
	if (left_fork > right_fork)
		std::swap(left_fork, right_fork);

	for (auto i = 1; i <= iterations; ++i) {
		// sleep_for is noexcept
		std::this_thread::sleep_for(milliseconds(100));

		{
			lock_guard<mutex> left(forks[left_fork]), right(forks[right_fork]);

			{
				lock_guard<mutex> lock{output};
				std::cout << "Filósofo " << id << " começa a comer pela " << i << "ª vez." << std::endl;
			}

			std::this_thread::sleep_for(milliseconds(200));

			{
				lock_guard<mutex> lock{output};
				std::cout << "Filósofo " << id << " termina de comer pela " << i << "ª vez." << std::endl;
			}
		}
	}
}

int main() {
	std::cout << "Criando filósofos." << std::endl;

	std::vector<std::thread> phil;
	for (auto i = 0; i < philosophers; ++i) {
		phil.emplace_back(philosopher, i);
	}

	for (auto& p : phil) {
		p.join();
	}

	return 0;
}
