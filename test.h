#pragma once
#include <iostream>
namespace test
{
	class Test
	{
	public:
		
        void* operator new(size_t size, int n) {
            std::cout << "custom placement new called, size = " << size << "|n = " << n << std::endl;
            return ::operator new(size);
        }
      
        // Since the above "placement delete" hides the global one, expose it
        void operator delete(void* ptr) {
            std::cout << "golbal delete called1" << std::endl;
            return ::operator delete(ptr);
        }

		


        int a;
        int b;
	};

}
