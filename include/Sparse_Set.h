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

    Entity Add(const Entity& ID) 
    { 
      auto ID_Locations = Internal_Find_Locations(ID);
      Index Result = Confirm_Entity_Exists(ID, ID_Locations);

      if (Result != 0) { return ID; }

      else { throw "[Add function]: not yet implemented"; }
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
