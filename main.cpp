#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
struct ChronoTimer
{
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;
	ChronoTimer()
	{
		start = std::chrono::high_resolution_clock::now();
	}
	~ChronoTimer()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		float ms = duration.count() * 1000.0f;
		std::cout << "Timer took " << ms << "ms" << '\n';
	}
};
void WithoutLookUp() {
	long long Sum = 0;
	ChronoTimer timer;
	for (int j = 0; j < 2147483647; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			switch (i)
			{
			case 1:
				Sum += 1;
				break;
			case 2:
				Sum += 2;
				break;
			case 3:
				Sum += 3;
				break;
			case 4:
				Sum += 4;
				break;
			case 5:
				Sum += 5;
				break;
			case 6:
				Sum += 6;
				break;
			case 7:
				Sum += 7;
				break;
			case 8:
				Sum += 8;
				break;
			case 9:
				Sum += 9;
				break;
			case 10:
				Sum += 10;
				break;
			default:
				break;
			}
		}
	}
	std::cout << "Switch statement\n";
	std::cout << "Sum:" << Sum << "\n";
}
void LookUp() {
	int numbers[11]{ 0,1,2,3,4,5,6,7,8,9,10 };
	long long Sum = 0;
	ChronoTimer timer;
	for (int j = 0; j < 2147483647; j++)
	{
		for (int i = 0; i < 10; i++)
		{
			Sum += numbers[i];
		}
	}
	std::cout << "Lookup Table\n";
	std::cout << "Sum:" << Sum << "\n";
}
int main(void) {
	WithoutLookUp();
	LookUp();
	return 0;
}