#include <iostream>
#include <Hybrid_Set.h>

struct Test_Component { int value = 5; };

int main()
{
  try{
    Hybrid_Set<Test_Component, 64> pool;

    for (std::size_t i = 63; i != 0; i--)
    {
      std::cout << "i: " << i << " ";
      pool.Add(i);
    }
    pool.Debug();
  }
  catch (const char* Error)
  {
    std::cout << "[Error]: " << Error << '\n';
    exit(-1);
  }
  return 0;
}
