#include <iostream>
#include <conio.h>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <windows.h>
#include <random>

using namespace std;

volatile int rtime = 2000;
volatile long long result = 0;

HANDLE closeEvent;
mutex m;

int intRand(const int min, const int max) 
{
	random_device rd;
	mt19937 generator(rd());
	uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

void closeThread()
{
	m.lock();
	cout << "to exit press any key\n";
	m.unlock();
	if (cin.get() != 0)
	{
		SetEvent(closeEvent);
	}
}

void t(int id, const map<string, int>& prices, map<string, int>& cnt, vector<string>& expences)
{
	//srand(time(NULL)); (rand() % (rtime + 1)), rand() % expences.size()
	while (true)
	{
		this_thread::sleep_for(chrono::milliseconds(intRand(0, rtime)));

		if (closeEvent && WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		m.lock();
		string rkey = expences[intRand(0, expences.size() - 1)];
		cout << "thread" << id << " appealed to " << rkey << '\n';
		result += prices.at(rkey);
		cnt[rkey]++;
		m.unlock();
	}
}

int main(int argc, char* argv[])
{
	if (argc == 1 || (argc > 1 && !atoi(argv[0])))
	{
		cout << "there no rand time parameter or it is incorrect, will be used 2000 ms\n";
	}
	else
	{
		rtime = atoi(argv[0]);
	}

	closeEvent = CreateEvent(NULL, true, false, NULL);
	if (!closeEvent)
	{
		cout << "Warning!!! close event is not created, programm will be executed endlessly\n";
	}

	thread close(closeThread);
	srand(time(NULL));

	vector<string> expences = { "food", "entertainment", "transport", "zhek" };
	map<string, int> prices;
	map<string, int> cnt;
	for (auto p : expences)
	{
		cnt.insert({ p, 0 });
		prices.insert({ p, rand() % 100});
	}

	vector<thread> vecThread(4);
	int id = 0;
	for (auto& i : vecThread)
	{
		i = thread(t, id++, ref(prices), ref(cnt), ref(expences));
	}

	while (true)
	{
		this_thread::sleep_for(chrono::seconds(10));
		
		if (closeEvent && WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0)
		{
			break;
		}

		m.lock();
		cout << "count of appeals: \n";
		for (auto p : cnt)
		{
			cout << p.first << ' ' << p.second << '\n';
		}

		cout << "all costs: " << result << '\n';
		m.unlock();
	}

	for (auto& i : vecThread)
	{
		i.join();
	}

	close.join();

	if (closeEvent)
		CloseHandle(closeEvent);
}
