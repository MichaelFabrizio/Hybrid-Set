#include <iostream>
#include <Hybrid_Set.h>

struct Test_Component { int value = 5; };

int main()
{
  try{
    Hybrid_Set<Test_Component, 64> pool;

    for (std::size_t i = 63; i > 0;)
    {
      pool.Add(i);
      i--;
    }

    for (std::size_t i = 0; i < 40; i++)
    {
      pool.Remove(i);
    }
    pool.Add(25);
    pool.Add(1);
    pool.Add(2);
    pool.Add(3);
    pool.Add(4);
    pool.Add(5);
    pool.Add(10);
    pool.Add(12);
    // pool.Clear();
    pool.Debug();
  }
  catch (const char* Error)
  {
    std::cout << "[Error]: " << Error << '\n';
    exit(-1);
  }
  return 0;
}
