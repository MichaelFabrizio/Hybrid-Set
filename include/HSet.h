#pragma once
#include <iostream>

template <std::size_t Max_Size>
class HSet
{
  typedef std::size_t Entity;
  typedef std::size_t Index;

  inline bool ID_Exists_On_Left(const Entity& EntityID) { 
    return (EntityID < Next_Empty) ? true : false;
  }

  // Add optional feature: If entity is found at Index_0 (old bitmask location),
  // Index it right away! Use an overloaded FN of Find_Exact_Location().
  // The overloaded FN should look up Index_2 first, then Index_1, then Index_0.
  // This will "maybe" produce faster repeat Look Ups (needs benchmarking)
  // TODO: Test the speed for different sized containers

  inline Index Find_Exact_Location(const Entity& ID)
  {
    if        (ID_Exists_On_Left(ID))   { if (S[ID] == ID) 
                                              { return ID; }
    }
    Index LAST = S[ID];
    Index INDEX_2 = (LAST != 0) ? LAST : 0;                 // Can be more expensive to find, check saved for last
    
    if (S[INDEX_2] == ID)               { return INDEX_2; } // INDEX_2 = Last place to find
    else                                {  return 0; }       // Final case: Not found
  }

  template <std::size_t R = 0>
  void Place(const Entity ID)
  {
    if constexpr (R == 0)   {
      if (ID < Next_Empty)  {
        auto Swap_ID = S[ID];
        if (Swap_ID <= ID) { return; }
        if (Swap_ID > Next_Empty)    {
          std::cout << "R0 --Compact-- Placement\n";
          S[ID] = ID;
          Place<3>(Swap_ID); return;
        }
        else {
          S[ID] = ID;
          Place<2>(Swap_ID); return; 
        }
      }
      
      if (ID == Next_Empty) { Place<1>(ID); return; }
      if (ID > Next_Empty) { Place<4>(ID); return; }

      return;
    }

    if constexpr (R == 1)
    {
      std::cout << "R1 --Unknown Exact-- Placement\n";
      if (S[Next_Empty] != 0) { return; }
      else {
        S[Next_Empty] = ID;
        Count++;
        Next_Empty++;
        return;
      }
    }

    if constexpr (R == 2)
    {
      std::cout << "R2 --Known Exact-- Placement\n";
      S[Next_Empty] = ID;
      Count++;
      Next_Empty++;
      return;
    }

    if constexpr (R == 3)
    {
      std::cout << "R3 --Known Greater Than-- Placement\n";
      const auto Next_Empty_Pointer = S[Next_Empty];
      if (Next_Empty_Pointer != 0)
      {
        S[Next_Empty_Pointer] = ID;
        S[Next_Empty] = Next_Empty;
        S[ID] = Next_Empty_Pointer;
        
        Count++;
        Next_Empty++;
        return;
      }
      else
      {
        S[Next_Empty] = ID;
        S[ID] = Next_Empty;
        
        Count++;
        Next_Empty++;
        return;
      }
    }

    if constexpr (R == 4)
    {
      std::cout << "R4 --Unknown Greater Than-- Placement\n";
      if (S[ID] != 0) { return; }
      
      const auto Next_Empty_Pointer = S[Next_Empty];
      if (Next_Empty_Pointer != 0)
      {
        S[Next_Empty_Pointer] = ID;
        S[Next_Empty] = Next_Empty;
        S[ID] = Next_Empty_Pointer;
        
        Count++;
        Next_Empty++;
        return;
      }
      else
      {
        S[Next_Empty] = ID;
        S[ID] = Next_Empty;
        Count++;
        Next_Empty++;
        return;
      }
    }
  }

  public:
    HSet() : S {0} {}

    ~HSet() {}
    HSet(const HSet& H) = delete;
    HSet& operator=(const HSet&  H) = delete;
    HSet& operator=(const HSet&& H) = delete;

    void Add(const Entity ID)
    {
      if (ID <= 0 || ID >= Max_Size)  { return; }
      if (Next_Empty >= Max_Size)     { return; }

      Place<0>(ID);

      Index Result = Find_Exact_Location(ID);
      if (Result >= Max_Size) { throw "[Add Function]: Critical index overflow error.\n"; }
    }

    void Remove(const Entity& ID) 
    { 
      throw "not yet implemented\n";
    }

    Index Find(const Entity ID) { return Find_Exact_Location(ID); }
    
    void Clear()
    {
      for (Index i = 0u; i < Max_Size; i++) 
      { 
        S[i] = 0u; 
      }
      Count = 0u;
      Next_Empty = Count + 1u;
    }

    std::size_t GetCount()  { return Count; }
    std::size_t GetNext()   { return Next_Empty; }
    std::size_t GetMaxSize()   { return Max_Size; }
    std::size_t GetIndexOwner(Index index) { return S[index]; }

    void Debug() 
    { 
      std::cout << "Index variables start at: " << Next_Empty << '\n';
      for (std::size_t i=0u; i!=Max_Size; i++)
      {
        std::cout << "Index: " << i << " Element: " << S[i] << '\n';
      }
    }

  private:
    std::size_t S[Max_Size];              //Sparse array of entity IDs

    std::size_t Count = 0u;               //Count represents the number of entities
    std::size_t Next_Empty = Count + 1u;  //Count + 1u represents the next "empty" element in our set.
};
