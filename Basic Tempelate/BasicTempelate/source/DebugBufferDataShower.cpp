#include <iostream>
using namespace std;
union Data{
	int32_t data;
	float f;
};
int main2() {
	while (true)
	{
		Data data;
		data.data = 0x3f800000;
		cout << data.f << endl;
	}
	return 0;
}