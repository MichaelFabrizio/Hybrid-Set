#include <iostream>
#include <Hybrid_Set.h>

struct Test_Component { int value = 5; };

int main()
{
  try{
    Hybrid_Set<Test_Component, 64> pool;
    pool.Add(5);
  }
  catch (const char* Error)
  {
    std::cout << "[Error]: " << Error << '\n';
    exit(-1);
  }
  return 0;
}
