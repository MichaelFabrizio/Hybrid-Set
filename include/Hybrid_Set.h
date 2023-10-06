#pragma once
#include <iostream>
#include <tuple>

template <typename T, std::size_t N>
class Hybrid_Set
{
  typedef std::size_t Entity;
  typedef std::size_t Index;
  typedef std::tuple<Index, Index, Index> Location_Set;

  inline bool ID_Exists_Left_Zone(const Entity& EntityID) { 
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

    if        (ID_Exists_Left_Zone(ID)) { return ID;      }
    else if   (S[INDEX_0] == ID)        { return INDEX_0; }
    else if   (S[INDEX_1] == ID)        { return INDEX_1; }

    Index INDEX_2 = S[ID];                                  // Can be more expensive to find, check saved for last
    if (INDEX_2 != 0)                   { return INDEX_2; } // INDEX_2 = Last place to find
    else                                { return 0; }       // Final case: Not found
  }

  template <std::size_t Recursion_Depth = 0>
  void Compare_And_Promote(const Entity Placement, const Location_Set Locations)
  {
    constexpr auto R =             Recursion_Depth;
    const Index Bitmask_Square =   std::get<R>(Locations);
    const std::size_t Bitmask_ID = S[ Bitmask_Square ];
    
    if constexpr (R != 2)
    { 
      Entity Next_Placement;
      Location_Set Next_Placement_Locations;
      
      if (Bitmask_Square > (Count + 1u))        //Should be possible to remove this check for R=0, under some conditions
      { 
        S[Count + 1u] =             Placement;
        S[Placement] =              Count + 1u;
        Count++;
        return;
      }

      else if (Bitmask_ID < Placement)
      {
        S[Bitmask_ID] =             Bitmask_Square;
        Next_Placement =            Placement;
        Next_Placement_Locations =  Locations;
      }

      else
      {
        S[Bitmask_Square] =         Placement;
        S[Placement] =              Bitmask_Square;
        Next_Placement =            Bitmask_ID;
        Next_Placement_Locations =  Find_Locations(Next_Placement);
      }
      Compare_And_Promote<R+1>(Next_Placement, Next_Placement_Locations);
    }

    if constexpr (R == 2)
    {
      S[Count + 1u] = Placement;
      S[Placement] =  Count + 1u;
      Count++;
      return;
    }
  }

  inline void Internal_Update_Bitmasks()
  {
    // checks if the next empty square in our array has reached a new order of magnitude
    std::size_t Next_Level_Bool = ((Count + 1u) & Get_Next_Level) >> Level;

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
    Hybrid_Set() : S {0} { static_assert(128 <= N <= 256, "Array size restricted currently\n" ); }

    void Add(const Entity& ID) 
    { 
      Index Result = Find_Exact_Location(ID);

      if (Result != 0) { return; } //TODO: Will return T[Result]
      if (Count == N) { throw "[Add Function]: Error - Array bounds were allowed to overflow\n"; }

      auto ID_Locations = Find_Locations(ID);
      Compare_And_Promote<0>(ID, ID_Locations);
      Internal_Update_Bitmasks();

      //TEMPORARY LOGIC
       Result = Find_Exact_Location(ID);
       std::cout << "Result: " << Result << '\n';
       if (Result == 0) { throw "[Add Function]: Result shouldn't be zero anymore!\n"; }
      //END TEMP LOGIC

      //Will return T[Result]
      throw "[Add function]: not yet implemented\n";
    }

  private:
    // T Elements[N];                     //Contiguous array of objects
    std::size_t S[N];                     //Sparse array of entity IDs

    std::size_t Count = 0u;               //Count represents the number of entities
    std::size_t Next_Empty = Count + 1u;  //Count + 1u represents the next "empty" element in our set.
    
    std::size_t Level = 5u;                              
    std::size_t Get_Next_Level = 1u << Level;           
    std::size_t Get_Current_Level = 1u << (Level - 1u);
    std::size_t Level_Bitmask = Get_Next_Level - 1u;
    std::size_t Old_Bitmask = Get_Current_Level - 1u;
};
