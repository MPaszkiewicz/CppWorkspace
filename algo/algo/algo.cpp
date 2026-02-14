// algo.cpp : Defines the entry point for the application.
//

#include "algo.h"

#include "vector/Vector.h"
#include "vector/search.h"

#include <vector>

int main()
{
	std::cout << "Hello CMake." << std::endl;

    const std::vector<int> data{ 1, 2, 4, 5, 5, 6 };
	vector<int> v;
	for (int i = 0; i < data.size(); ++i)
	{
		v.push_back(data[i]);
	}

    for (int i = 0; i < 8; ++i)
    {
        // Search for first element x such that i ≤ x
        auto lower = lower_bound<int>(v.data(), 0, v.size(), i);



        std::cout << i << " ≤ ";
        lower != v.size()
            ? std::cout << v[lower] << " at index " << lower
            : std::cout << "not found";
        std::cout << '\n';
    }

	return 0;
}
