#pragma once
#include <iostream>

template <typename T, std::size_t N>
class Hybrid_Set
{
  typedef std::size_t Entity;
  typedef std::size_t Index;

  inline bool ID_Exists_On_Left(const Entity& EntityID) { 
    return (EntityID < Next_Empty) ? true : false;
  }

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
    std::cout << "ID: " << ID << '\n';
    std::cout << "Left Place" << '\n';
      const auto ID_To_Swap = S[ID];
      S[ID] = ID;
      
      bool ID_To_Swap_Direction = ID_Exists_On_Left(ID_To_Swap);
      Place_R0(ID_To_Swap, ID_To_Swap_Direction);
    }
    else if (ID == Next_Empty)
    { Place_R2(ID); }
    else
    {
      Place_R1(ID);
    }
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
      std::cout << "ID: " << ID << '\n';
      std::cout << "Masked place: " << Candidate_Index << '\n';
      S[Candidate_Index] = ID;
      Place_R1(Candidate_Owner); //Optional Feature: Call Place_R2 here instead (w/ extra checks)
    }
  }

  inline void Place_R2(const Entity ID)
  {
    
    std::cout << "ID: " << ID << '\n';
    const auto Next_Empty_ID_Pointer = S[Next_Empty];
    const auto Next_Empty_Bitmask_Pointer = Next_Empty & Old_Bitmask;
    const auto Next_Empty_Bitmask_Value = S[Next_Empty_Bitmask_Pointer];
    
    if (Next_Empty_ID_Pointer != 0)
    {
      S[Next_Empty] = ID;
      std::cout << "Endpoint Exact place: " << ID << '\n';
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
      std::cout << "Endpoint Bitmask-Swap and place: " << Next_Empty_Bitmask_Pointer << '\n';
    }
    else
    {
      std::cout << "Endpoint Index place: " << Next_Empty << '\n';
      S[Next_Empty] = ID;
      S[ID] = Next_Empty;
    }
    
    Count++;
    Next_Empty++;
    Internal_Update_Bitmasks();
  }

  inline void Swap_And_Remove_Last_Entity(const Entity ID)
  {
    std::cout << "Removing ID: " << ID << '\n';
    const auto Last_ID_Value = S[Count];

    S[ID] = Last_ID_Value;
    if (Last_ID_Value != Count)
    {
      S[Last_ID_Value] = ID;
    }
    S[Count] = 0u;

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
      Old_Bitmask = Level_Bitmask;
      Level--;
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
      bool direction = ID_Exists_On_Left(ID);

      if (ID == 0) { return; } //TODO: Return 0
      if (ID > 0xFFFF) { std::cout << "Underflow ID detected\n"; return; } //Extra checks for testing purposes; Also return 0
      if (ID >= N) { std::cout << "Overflow ID detected\n"; return; } //return 0
      if (Next_Empty == N) { throw "Array bounds overflow detected\n"; }
      
      Place_R0(ID, direction);

      //TEMPORARY LOGIC
       Index Result = Find_Exact_Location(ID);

       if (Result == 0) { throw "[Add Function]: Result shouldn't be zero anymore!\n"; }
       if (Result > (N-1)) { throw "[Add Function]: Critical index overflow error.\n"; }
      //END TEMP LOGIC

      //Will return T[Result]
    }

    void Remove(const Entity& ID) 
    { 
      if (ID_Exists_On_Left(ID) && (S[ID] == ID)) { Swap_And_Remove_Last_Entity(ID); }
      else
      {
        const auto Index = Find_Exact_Location(ID);
        if (Index == 0u) { std::cout << "ID not found\n"; return; }

        Swap_And_Remove_Last_Entity(Index);
      }
    }

    void Debug() 
    { 
      std::cout << "Index variables start at: " << Next_Empty << '\n';
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
    
    std::size_t Level = 2u;                              
    std::size_t Get_Next_Level = 1u << Level;           
    std::size_t Get_Current_Level = 1u << (Level - 1u);
    std::size_t Level_Bitmask = Get_Next_Level - 1u;
    std::size_t Old_Bitmask = Get_Current_Level - 1u;
};
