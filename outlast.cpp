#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

unsigned int current_channel = 0, watching = 0;
const uint8_t channels = 4, crazies = 30;
const uint8_t runs = 10;

std::mutex output, queue, remote;

void crazed(unsigned int id, unsigned int fav_channel) {
	using std::chrono::milliseconds;
	using std::lock_guard;
	using std::mutex;
	using std::unique_lock;

	{
		lock_guard<mutex> lock{output};
		std::cout << "Louco " << id << " com canal favorito " << fav_channel
			<< " criado." << std::endl;
	}

	for (auto i = 0; i < runs; ++i) {
		std::this_thread::sleep_for(milliseconds(100));

		{
			lock_guard<mutex> sofa{queue};
			if (current_channel == fav_channel) {
				++watching;
			} else {
				// wait until remote is released
				
				current_channel = fav_channel;
			}
		}

		{
			lock_guard<mutex> lock{output};
			std::cout << "Louco " << id << " começa a assistir o canal "
				<< fav_channel << std::endl;
		}

		std::this_thread::sleep_for(milliseconds(100));

		{
			lock_guard<mutex> lock{output};
			std::cout << "Louco " << id << " termina de assistir o canal "
				<< fav_channel << std::endl;
		}

		{
			lock_guard<mutex> sofa{queue};
			--watching;

			if (watching == 0) {
				// release remote
			}
		}
	}
}

int main() {
	using std::thread;
	using std::vector;

	vector<thread> craz;
	for (auto i = 0; i < crazies; ++i) {
		craz.emplace_back(crazed, i, i % channels);
	}

	for (auto& c : craz) {
		c.join();
	}

	return 0;
}
