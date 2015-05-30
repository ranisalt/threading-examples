#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>
#include "semaphore.h"

using ms = std::chrono::milliseconds;
using unique_lock = std::unique_lock<std::mutex>;
using resource_type = int;
const int buffer_size = 1;
const int iterations = 10;
const int min_wait = 200, max_wait = 500;
const int reader_count = 50;
const int writer_count = 50;

static semaphore empty(buffer_size), full, readers(1), writers(1);
static std::mutex output_mutex, resource_mutex;
static std::queue<resource_type> resource;

void reader(int id) {
	unique_lock output_lock(output_mutex);
	std::cout << "Leitor " << id << " criado." << std::endl;
	output_lock.unlock();

	static std::mutex count_mutex;
	std::random_device rd;
	std::uniform_int_distribution<int> delay_generator(min_wait, max_wait);

	for (auto i = 0; i < iterations; ++i) {
		// simulate slow non-critical region
		std::this_thread::sleep_for(ms(delay_generator(rd)));

		full.p();
		readers.p();

		output_lock.lock();
		std::cout << "Leitor " << id << " começa a ler." << std::endl;
		output_lock.unlock();

		{
			// lock the resource (queue)
			// this is a scoped lock: it unlocks automatically when scope ends
			unique_lock resource_lock(resource_mutex);

			// simulate slow critical region
			std::this_thread::sleep_for(ms(delay_generator(rd)));
			auto num = resource.front();
			resource.pop();

			output_lock.lock();
			std::cout << "Leitor " << id << " leu " << num << std::endl;
			output_lock.unlock();
		}

		output_lock.lock();
		std::cout << "Leitor " << id << " termina de ler." << std::endl;
		output_lock.unlock();

		readers.v();
		empty.v();
	}
}

void writer(int id) {
	unique_lock output_lock(output_mutex);
	std::cout << "Escritor " << id << " criado." << std::endl;
	output_lock.unlock();

	static std::mutex count_mutex;
	std::random_device rd;
	std::uniform_int_distribution<int> generator(0, 1023), delay_generator(min_wait, max_wait);

	for (auto i = 0; i < iterations; ++i) {
		std::this_thread::sleep_for(ms(delay_generator(rd)));

		empty.p();
		writers.p();
	
		output_lock.lock();
		std::cout << "Escritor " << id << " começa a escrever." << std::endl;
		output_lock.unlock();

		{
			unique_lock resource_lock(resource_mutex);
			std::this_thread::sleep_for(ms(delay_generator(rd)));
			auto num = generator(rd);
			resource.push(num);

			output_lock.lock();
			std::cout << "Escritor " << id << " escreveu " << num << std::endl;
			output_lock.unlock();
		}

		output_lock.lock();
		std::cout << "Escritor " << id << " termina de escrever." << std::endl;
		output_lock.unlock();

		writers.v();
		full.v();
	}
}

int main() {
	using std::thread;
	using std::vector;

	vector<thread> threads;
	threads.reserve(reader_count + writer_count);
	for (int i = 0; i < reader_count; ++i) {
		threads.emplace_back(reader, i);
	}
	for (int i = 0; i < writer_count; ++i) {
		threads.emplace_back(writer, i);
	}

	for (auto& t : threads) {
		t.join();
	}

	return 0;
}
