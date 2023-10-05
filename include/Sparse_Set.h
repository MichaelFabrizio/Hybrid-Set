#pragma once
#include <iostream>
#include <tuple>

template <typename T, std::size_t N>
class Sparse_Set
{
  typedef std::size_t Entity;
  typedef std::size_t Index;
  typedef std::tuple<Index, Index, Index> Location_Set;

  inline Location_Set Internal_Find_Locations(const Entity& EntityID)
  {
    Location_Set locations;
    
    locations = { EntityID & Old_Bitmask,
                  EntityID & Level_Bitmask,
                  S[EntityID]
    };

    return locations;
  }

  inline Index Confirm_Entity_Exists(const Entity& ID, const Location_Set& locations)
  {
    auto Entity_0 = S[ std::get<0>(locations) ]; //   The entity which exists at Index = (EntityID & Old_Bitmask)
    auto Entity_1 = S[ std::get<1>(locations) ]; //   The entity which exists at Index = (EntityID & New_Bitmask)
    auto Entity_2 = S[ std::get<2>(locations) ]; //   This works differently, it returns S[S[EntityID]], which outputs Index=0 if the entity was not found,
                                                 //   Note: Index 0 is always a null entity and cannot be assigned.
                                                 //   ...Otherwise, Returns the exact square where the entity is found.

    if (Entity_0 == ID)  { return std::get<0>(locations); }
    if (Entity_1 == ID)  { return std::get<1>(locations); }
    if (Entity_2 != 0)   { return std::get<2>(locations); }
    
    else { return 0; }
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
        Next_Placement_Locations =  Internal_Find_Locations(Next_Placement);
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
    Sparse_Set() : S {0} {}

    void Add(const Entity& ID) 
    { 
      auto ID_Locations = Internal_Find_Locations(ID);
      Index Result = Confirm_Entity_Exists(ID, ID_Locations);

      if (Result != 0) { } //TODO: Will return T[Result]
      if (Count == N) { throw "[Add Function]: Error - Array bounds were allowed to overflow\n"; }

      Compare_And_Promote<0>(ID, ID_Locations);
      Internal_Update_Bitmasks();

      //TEMPORARY LOGIC
      ID_Locations = Internal_Find_Locations(ID);
      Result = Confirm_Entity_Exists(ID, ID_Locations); //Redundant extra checks -> To be phased out
      std::cout << "Result: " << Result << '\n';
      //END TEMP LOGIC

      //Will return T[Result]
      throw "[Add function]: not yet implemented\n";
    }

  private:
    // T Elements[N];        //Contiguous array of objectstd::get<0>(ID_Locations)s
    std::size_t S[N];        //Sparse array of entity IDs

    std::size_t Count = 0u;  //Count represents the number of entities
                             //ALSO: Count + 1u represents the next "empty" element in our set.
    
    std::size_t Level = 5u;                              
    std::size_t Get_Next_Level = 1u << Level;           
    std::size_t Get_Current_Level = 1u << (Level - 1u);
    std::size_t Level_Bitmask = Get_Next_Level - 1u;
    std::size_t Old_Bitmask = Get_Current_Level - 1u;
};
