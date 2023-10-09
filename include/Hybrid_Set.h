#pragma once
#include <iostream>
#include <tuple>

template <typename T, std::size_t N>
class Hybrid_Set
{
  typedef std::size_t Entity;
  typedef std::size_t Index;
  typedef std::tuple<Index, Index, Index> Location_Set;

  inline bool ID_Exists_On_Left(const Entity& EntityID) { 
    return (EntityID < Next_Empty) ? true : false;
  }

  inline Location_Set Find_Locations(const Entity& EntityID)
  {
    Location_Set locations;
    
    locations = { EntityID & Old_Bitmask,
                  EntityID & Level_Bitmask,
                  S[EntityID]
    };

    return locations;
  }

  inline Index Find_Exact_Location(const Entity& ID)
  {
    Index INDEX_0 = ID & Old_Bitmask;
    Index INDEX_1 = ID & Level_Bitmask;

    if        (ID_Exists_On_Left(ID))   { return ID;      }
    else if   (S[INDEX_0] == ID)        { return INDEX_0; }
    else if   (S[INDEX_1] == ID)        { return INDEX_1; }

    Index LAST = S[ID];
    Index INDEX_2 = (LAST != 0) ? LAST : 0;                 // Can be more expensive to find, check saved for last
    
    if (S[INDEX_2] == ID)               { return INDEX_2; } // INDEX_2 = Last place to find
    else                                { return 0; }       // Final case: Not found
  }

  inline Entity Place_R1(const Entity& ID)
  {
    const auto Candidate_Index = ID & Level_Bitmask;

    if (Candidate_Index < Next_Empty)
    { 
      const auto Previous_ID = S[Candidate_Index];
      if (Previous_ID < ID) { return ID; }
      
      else {
        S[Candidate_Index] = ID;
        auto Next_ID = Place_R1(Previous_ID);
        return Next_ID; }
    }
    return ID;
  }

  inline void Place_R2(const Entity& ID)
  {
    S[Next_Empty] = ID;
    S[ID] = Next_Empty;
    Count++;
    Next_Empty++;
    Internal_Update_Bitmasks(); // Must be called after Count++ and Next_Empty++. A given Set, H, needs updated leading bit & associative masks internally.
  }

  template <std::size_t R = 0>
  inline void Place(const Entity& ID)
  {
    if constexpr (R == 0) {
      if (ID_Exists_On_Left(ID)) {        // Add a limit on this recursion branch. TODO: Refactor into a Conditional_Place_Lefthanded method.
        const auto Previous_ID = S[ID];
        S[ID] = ID;
        Place<R>(Previous_ID); }
      
      else { Place<R+1>(ID); } }

    if constexpr(R == 1) { const auto Next_ID = Place_R1(ID); Place<R+1>(Next_ID); }
    if constexpr(R == 2) { Place_R2(ID); }
  }

  inline void Internal_Update_Bitmasks()
  {
    // checks if the next empty square in our array has reached a new order of magnitude
    std::size_t Next_Level_Bool = ((Next_Empty) & Get_Next_Level) >> Level;

    //temporary: ensure output is always 1u or 0u, (output must always be 1u or 0u)
    if (Next_Level_Bool > 1u) { throw "next_level_bool weird output"; }

    //updates bitmask and level data
    else if (Next_Level_Bool)     {
      Old_Bitmask = Level_Bitmask;
      Level++;
      Get_Next_Level = 1u << Level;
      Get_Current_Level = 1u << (Level - 1u);
      Level_Bitmask = Get_Next_Level - 1u;
    }
  }

  public:
    Hybrid_Set() : S {0} { static_assert(128 <= N <= 256, "Array size restricted currently\n" ); } //For some reason static assert isn't working
   ~Hybrid_Set() {};
    Hybrid_Set(const Hybrid_Set& H) = delete;
    Hybrid_Set& operator=(const Hybrid_Set&  H) = delete;
    Hybrid_Set& operator=(const Hybrid_Set&& H) = delete;

    void Add(const Entity& ID) 
    { 
      if (ID == 0) { return; } //TODO: Return 0
      if (ID > 0xFFFF) { std::cout << "Underflow ID detected\n"; return; } //Extra checks for testing purposes; Also return 0
      if (ID >= N) { std::cout << "Overflow ID detected\n"; return; } //return 0
      if (Next_Empty == N) { throw "[Add Function]: Error - Array bounds were allowed to overflow\n"; }
      Place<0>(ID);

      //TEMPORARY LOGIC
       Index Result = Find_Exact_Location(ID);
       if (Result == 0) { throw "[Add Function]: Result shouldn't be zero anymore!\n"; }
       if (Result > (N-1)) { throw "[Add Function]: Critical index overflow error.}
      //END TEMP LOGIC

      //Will return T[Result]
    }

    void Remove(const Entity& ID) //Could it make sense to be passing ID by value for these?
    {
      const auto Last_ID = S[Count];
      if (ID_Exists_On_Left(ID) && S[ID] == ID) //Found ID to remove in contiguous section (Left side)
      {
        if (Last_ID != Count)           //Case 1: S[Count] is indexed (Defined as Exists_On_Right() = True)
        { 
          S[Count] = 0;
          S[ID] = Last_ID;
          S[Last_ID] = ID;
        }
        else //Last_ID == Count        //Case 2: S[Count] = Count. Only occurs on an exact match, and requires different remove logic.
        {
          S[Count] = ID;
          S[ID] = Last_ID;
        }

        Count--;
        Next_Empty--;
      }
      else
      {
        const auto ID_To_Remove_Index = Find_Exact_Location(ID);
        throw "Not yet implemented\n";
      }
    }

    void Debug() 
    { 
      for (std::size_t i=0; i!=N; i++)
      {
        std::cout << "Index: " << i << " Element: " << S[i] << '\n';
      }
    }

  private:
    // T Elements[N];                     //Contiguous array of objects
    std::size_t S[N];                     //Sparse array of entity IDs

    std::size_t Count = 0u;               //Count represents the number of entities
    std::size_t Next_Empty = Count + 1u;  //Count + 1u represents the next "empty" element in our set.
    
    std::size_t Level = 4u;                              
    std::size_t Get_Next_Level = 1u << Level;           
    std::size_t Get_Current_Level = 1u << (Level - 1u);
    std::size_t Level_Bitmask = Get_Next_Level - 1u;
    std::size_t Old_Bitmask = Get_Current_Level - 1u;
};
