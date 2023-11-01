#pragma once
#include <iostream>

template <typename T, std::size_t Max_Size>
class Hybrid_Set
{
  typedef std::size_t Entity;
  typedef std::size_t Index;

  const static std::size_t Starting_Level = 2u;

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
    Index INDEX_0 = ID & Old_Bitmask;
    Index INDEX_1 = ID & Level_Bitmask;

    if        (ID_Exists_On_Left(ID) && S[ID] == ID)   { return ID; }
    else if   (S[INDEX_0] == ID)        { return INDEX_0; }
    else if   (S[INDEX_1] == ID)        { return INDEX_1; }

    Index LAST = S[ID];
    Index INDEX_2 = (LAST != 0) ? LAST : 0;                 // Can be more expensive to find, check saved for last
    
    if (S[INDEX_2] == ID)               { return INDEX_2; } // INDEX_2 = Last place to find
    else                                { return 0; }       // Final case: Not found
  }

  inline void Place_R0(const Entity ID, const bool Direction)
  {
    if (Direction) { 
    // std::cout << "ID: " << ID << '\n';
    // std::cout << "Left Place" << '\n';
      const auto ID_To_Swap = S[ID];
      S[ID] = ID;
      
      bool ID_To_Swap_Direction = ID_Exists_On_Left(ID_To_Swap);
      Place_R0(ID_To_Swap, ID_To_Swap_Direction);
    }
    else if (ID == Next_Empty)
    { Place_R2(ID); }

    else
    { Place_R1(ID); }
  }

  inline void Place_R1(const Entity ID)
  {
    const auto Candidate_Index = ID & Level_Bitmask;

    if (Candidate_Index >= Next_Empty)
    {
      Place_R2(ID);
      return;
    }

    const auto Candidate_Owner = S[Candidate_Index];
    if (Candidate_Owner < ID)
    {
      Place_R2(ID);
    }
    else
    {
      // std::cout << "ID: " << ID << '\n';
      // std::cout << "Masked place: " << Candidate_Index << '\n';
      S[Candidate_Index] = ID;
      S[ID] = 0;
      Place_R1(Candidate_Owner); //Optional Feature: Call Place_R2 here instead (w/ extra checks)
    }
  }

  inline void Place_R2(const Entity ID)
  {
    
    // std::cout << "ID: " << ID << '\n';
    const auto Next_Empty_ID_Pointer = S[Next_Empty];
    const auto Next_Empty_Bitmask_Pointer = Next_Empty & Old_Bitmask;
    const auto Next_Empty_Bitmask_Value = S[Next_Empty_Bitmask_Pointer];
    
    if (Next_Empty_ID_Pointer != 0)
    {
      S[Next_Empty] = Next_Empty;
      S[Next_Empty_ID_Pointer] = ID;
      // std::cout << "Endpoint Exact place: " << ID << '\n';
      Count++;
      Next_Empty++;
      Internal_Update_Bitmasks();
      return;
    }
    else if (Next_Empty_Bitmask_Value == Next_Empty)
    {
      S[Next_Empty] = Next_Empty;
      S[Next_Empty_Bitmask_Pointer] = ID;
      S[ID] = Next_Empty_Bitmask_Pointer;
      // std::cout << "Endpoint Bitmask-Swap and place: " << Next_Empty_Bitmask_Pointer << '\n';
    }
    else
    {
      // std::cout << "Endpoint Index place: " << Next_Empty << '\n';
      S[Next_Empty] = ID;
      S[ID] = Next_Empty;
    }
    
    Count++;
    Next_Empty++;
    Internal_Update_Bitmasks();
  }

  inline void Swap_And_Remove_Last_Entity(const Entity ID, const Index ID_Index)
  {
    // std::cout << "Removing ID: " << ID << '\n';
    // std::cout << "ID Location: " << ID_Index << '\n';

    const auto Last_ID_Value = S[Count];

    S[ID_Index] = Last_ID_Value;
    if (Last_ID_Value != Count)
    {
      S[Last_ID_Value] = ID_Index;
      S[ID] = 0u;
      S[Count] = 0u;
    }
    else { S[Count] = ID_Index; }

    Count--;
    Next_Empty--;
    Downgrade_Bitmasks();
  }

  inline void Internal_Update_Bitmasks()
  {
    std::size_t Next_Level_Reached = (Next_Empty & Get_Next_Level) >> Level;

    if (Next_Level_Reached > 1u) { throw "next_level_bool weird output"; }

    else if (Next_Level_Reached)     {
      Old_Bitmask = Level_Bitmask;
      Level++;
      Get_Next_Level = 1u << Level;
      Get_Current_Level = 1u << (Level - 1u);
      Level_Bitmask = Get_Next_Level - 1u;
    }
  }

  inline void Downgrade_Bitmasks()
  {
    std::size_t Previous_Level_NOT_Reached = (Count & Get_Current_Level) >> (Level - 1u);
    if (Previous_Level_NOT_Reached > 1u) { throw "previous_level_bool weird output"; }

    else if (!Previous_Level_NOT_Reached)     {
      Level_Bitmask = Old_Bitmask;
      Level--;
      Get_Next_Level = 1u << Level;
      Get_Current_Level = 1u << (Level - 1u);
    }
  }

  public:
    Hybrid_Set() : S {0} { static_assert(128 <= Max_Size <= 256, "Array size restricted currently\n" ); } //For some reason static assert isn't working
   ~Hybrid_Set() {};
    Hybrid_Set(const Hybrid_Set& H) = delete;
    Hybrid_Set& operator=(const Hybrid_Set&  H) = delete;
    Hybrid_Set& operator=(const Hybrid_Set&& H) = delete;

    void Add(const Entity& ID) 
    { 
      bool direction = ID_Exists_On_Left(ID);

      if (ID == 0u) { return; } //TODO: Return 0
      if (S[ID] == ID) { return; }
      if (ID > 0xFFFF) { std::cout << "Underflow ID detected\n"; return; } //Extra checks for testing purposes; Also return 0
      if (ID >= Max_Size) { std::cout << "Overflow ID detected\n"; return; } //return 0
      if (Next_Empty >= Max_Size) { return; }
      
      Place_R0(ID, direction);

      //TEMPORARY LOGIC
       Index Result = Find_Exact_Location(ID);

       if (Result == 0u) { throw "[Add Function]: Result shouldn't be zero anymore!\n"; }
       if (Result > (Max_Size-1u)) { throw "[Add Function]: Critical index overflow error.\n"; }
      //END TEMP LOGIC

      //Will return Result
    }

    void Remove(const Entity& ID) 
    { 
      if (ID == 0u) { return; }
      auto Possible_Index = ID;

      if (ID_Exists_On_Left(ID) && (S[ID] == ID)) { Swap_And_Remove_Last_Entity(ID, Possible_Index); } //Should be refactored
      else
      {
        Possible_Index = Find_Exact_Location(ID); //Implicitly an exact match at this point (or the Null Entity 0)
        if (Possible_Index == 0u) { std::cout << "ID " << ID << " not found\n"; return; }

        Swap_And_Remove_Last_Entity(ID, Possible_Index);
      }
    }

    Entity Find(const Entity ID) const { return Find_Exact_Location(ID); }

    void Clear()
    {
      //RESET EVERYTHING
      for (Index i = 0u; i < Max_Size; i++) { S[i] = 0u; }
      //for (auto ID : T) { T[ID] = __; //Nullptr? Basic first instance? }
      //RESET THESE TOO
      Count = 0u;
      Next_Empty = Count + 1u;
      Level = Starting_Level;
      Get_Next_Level = 1u << Level;           
      Get_Current_Level = 1u << (Level - 1u);
      Level_Bitmask = Get_Next_Level - 1u;
      Old_Bitmask = Get_Current_Level - 1u;
    }

    std::size_t GetCount()  { return Count; }
    std::size_t GetNext()   { return Next_Empty; }
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
    //T Elements[Max_Size];               //Contiguous array of objects
    std::size_t S[Max_Size];              //Sparse array of entity IDs

    std::size_t Count = 0u;               //Count represents the number of entities
    std::size_t Next_Empty = Count + 1u;  //Count + 1u represents the next "empty" element in our set.
    
    std::size_t Level = Starting_Level;                              
    std::size_t Get_Next_Level = 1u << Level;           
    std::size_t Get_Current_Level = 1u << (Level - 1u);
    std::size_t Level_Bitmask = Get_Next_Level - 1u;
    std::size_t Old_Bitmask = Get_Current_Level - 1u;
};
