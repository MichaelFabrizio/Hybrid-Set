#pragma once
#include <iostream>
#include <random>

#include <Graphics/QuadColorAlpha.h>
#include <Signals/Shader_Sign.h>
#include <Signals/HSet_Sign.h>
#include <Util/Event.h>
#include <Util/EventMapTypes.h>
#include <Util/BasicMotion.h>


template <std::size_t Max_Size>
class Animated_HSet
{
  typedef std::size_t Entity;
  typedef std::size_t Index;

  const static std::size_t Grid_Height_Mask = 31u;
  const static std::size_t Grid_Width_Mask = Max_Size - 32u;
    
  //Quad rendered dimensions
  constexpr static float Quad_SizeX = 0.04f;
  constexpr static float Quad_SizeY = 0.04f * 1.5f;
  constexpr static float Padding = 0.005f;
  constexpr static float Origin_X = -0.75f;
  constexpr static float Origin_Y = 0.48f;

  constexpr static float FrameStep = 0.005f;

  //Quad coloring & gradients
  constexpr static float Empty_R = 0.55f;
  constexpr static float Empty_G = 0.55f;
  constexpr static float Empty_B = 0.55f;
  
  constexpr static float LeftVal_R = 0.522f;
  constexpr static float LeftVal_G = 0.603f;
  constexpr static float LeftVal_B = 0.235f;

  constexpr static float Index_R = 0.662f;
  constexpr static float Index_G = 0.937f;
  constexpr static float Index_B = 0.992f;

  constexpr static float Masked_R = 0.7f;
  constexpr static float Masked_G = 0.7f;
  constexpr static float Masked_B = 0.0f;

  constexpr static float ColorAdjust = 0.01f;
  constexpr static float ColorAdjustY = 0.005f;

  const static std::size_t Starting_Level = 1u;

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

    if        (ID_Exists_On_Left(ID))   { if (S[ID] == ID) 
                                              { return ID; }
    }

    // if   (S[INDEX_0] == ID)        { return INDEX_0; }
    // if   (S[INDEX_1] == ID)        { return INDEX_1; }

    Index LAST = S[ID];
    Index INDEX_2 = (LAST != 0) ? LAST : 0;                 // Can be more expensive to find, check saved for last
    
    if (S[INDEX_2] == ID)               { return INDEX_2; } // INDEX_2 = Last place to find
    else                                { std::cout << "ID: " << ID << '\n'; return 0; }       // Final case: Not found
  }

  template <std::size_t R = 0>
  void Place(const Entity ID)
  {
    if constexpr (R == 0)   {
      if (ID < Next_Empty)  {
        auto Swap_ID = S[ID];
        if (Swap_ID <= ID) { return; }
        if (Swap_ID > Next_Empty)    {
          // std::cout << "R0 Leftwise Placement\n";
          S[ID] = ID;
          Elements[ID].UpdateColorData(ID, false, LeftVal_R, LeftVal_G, LeftVal_B);
          
          trajectory.SetSource(ID);
          Place<3>(Swap_ID); return;
        }
        else {
          S[ID] = ID;
          Elements[ID].UpdateColorData(ID, false, LeftVal_R, LeftVal_G, LeftVal_B);
          trajectory.SetSource(ID);
          Place<2>(Swap_ID); return; 
        }
      }
      
      if (ID == Next_Empty) { Place<1>(ID); return; }
      if (ID > Next_Empty) { Place<4>(ID); return; }

      return;
    }

    if constexpr (R == 1)
    {
      // std::cout << "R1 --Unknown Exact-- Placement\n";
      if (S[Next_Empty] != 0) { return; }
      else {
        S[Next_Empty] = ID; Elements[ID].UpdateColorData(ID, false, LeftVal_R, LeftVal_G, LeftVal_B);
        Count++;
        Next_Empty++;
        return;
      }
    }

    if constexpr (R == 2)
    {
      // std::cout << "R2 --Known Exact-- Placement\n";
      Elements[ID].UpdateColorData(ID, false, LeftVal_R, LeftVal_G, LeftVal_B);
      S[Next_Empty] = ID;
      trajectory.Next_Motion(Next_Empty, LeftVal_R, LeftVal_G, LeftVal_B);
      Count++;
      Next_Empty++;
      return;
    }

    if constexpr (R == 3)
    {
      // std::cout << "R3 --Known Greater Than-- Placement\n";
      const auto Next_Empty_Pointer = S[Next_Empty];
      if (Next_Empty_Pointer != 0)
      {
        S[Next_Empty_Pointer] = ID;
        S[Next_Empty] = Next_Empty;
        S[ID] = Next_Empty_Pointer;

        Elements[Next_Empty].UpdateColorData(Next_Empty, false, LeftVal_R, LeftVal_G, LeftVal_B);

        Elements[ID].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        Elements[Next_Empty_Pointer].UpdateColorData(ID, true, Index_R, Index_G, Index_B);

        trajectory.SetSource(Next_Empty_Pointer);
        trajectory.Next_Motion(Next_Empty, Index_R, Index_G, Index_B);
        
        Count++;
        Next_Empty++;
        return;
      }
      else
      {
        S[Next_Empty] = ID;
        S[ID] = Next_Empty;
        
        Elements[Next_Empty].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        Elements[ID].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        trajectory.Next_Motion(Next_Empty, Index_R, Index_G, Index_B);
        
        Count++;
        Next_Empty++;
        return;
      }
    }

    if constexpr (R == 4)
    {
      // std::cout << "R4 --Unknown Greater Than-- Placement\n";
      if (S[ID] != 0) { return; }
      
      const auto Next_Empty_Pointer = S[Next_Empty];
      if (Next_Empty_Pointer != 0)
      {
        S[Next_Empty_Pointer] = ID;
        S[Next_Empty] = Next_Empty;
        S[ID] = Next_Empty_Pointer;
        
        Elements[Next_Empty_Pointer].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        Elements[Next_Empty].UpdateColorData(Next_Empty, false, LeftVal_R, LeftVal_G, LeftVal_B);
        Elements[ID].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        
        trajectory.SetSource(Next_Empty_Pointer);
        trajectory.Next_Motion(Next_Empty, Index_R, Index_G, Index_B);
        
        Count++;
        Next_Empty++;
        return;
      }
      else
      {
        Elements[Next_Empty].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        Elements[ID].UpdateColorData(ID, true, Index_R, Index_G, Index_B);
        S[Next_Empty] = ID;
        S[ID] = Next_Empty;
        trajectory.Next_Motion(ID, Index_R, Index_G, Index_B);
        Count++;
        Next_Empty++;
        return;
      }
    }
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

  inline void Upgrade_Bitmasks()
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

  inline void Downgrade_Bitmasks();
  void Receive_ImGUI_Payload(HSet_Payload* payload) { _payload = payload; _payload->Array_Start = & S[0]; _payload->Array_Count = Max_Size - 1u; }

  void Add_Range(Index start, Index end);

  public:
    Animated_HSet(HSet_IMGUI_Sink& subscription) : S {0}, dist(0, 100), Rangedist(1, Max_Size - 1u), MotionQuad(0.0f, 0.0f, Quad_SizeX, Quad_SizeY, LeftVal_R, LeftVal_G, LeftVal_B, 1.0f )
    { 
      subscription.Bind<Animated_HSet, &Animated_HSet::Receive_ImGUI_Payload>(this);

      float X_Index, Y_Index, Next_X_Location, Next_Y_Location;
      for (Index i = 0; i < Max_Size; i++) {
        X_Index = (float)((Grid_Width_Mask & i) >> 5);
        Y_Index = (float)(Grid_Height_Mask & i);
        Next_X_Location =  X_Index * (Quad_SizeX + Padding) + Origin_X;
        Next_Y_Location = - Y_Index * (Quad_SizeY + Padding) + Origin_Y;

      ::new (&Elements[i]) QuadColorAlpha(  Next_X_Location, Next_Y_Location, Quad_SizeX, Quad_SizeY,
                                Empty_R, Empty_G, Empty_B, 1.0f); 
      }

      MotionQuad.hidden = false;

      std::cout << "Index R: " << Index_R << '\n';
      std::cout << "Index G: " << Index_G << '\n';
      std::cout << "Index B: " << Index_B << '\n';
    }

    ~Animated_HSet() {};
    Animated_HSet(const Animated_HSet& H) = delete;
    Animated_HSet& operator=(const Animated_HSet&  H) = delete;
    Animated_HSet& operator=(const Animated_HSet&& H) = delete;

    void TestAdd(const Entity ID)
    {
      // std::cout << "Next_Empty" << Next_Empty << '\n';
      if (ID <= 0 || ID >= Max_Size)  { return; }
      if (Next_Empty >= Max_Size)     { return; }

      // std::cout << "Add: " << ID << '\n';
      Place<0>(ID);

      Index Result = Find_Exact_Location(ID);
      // if (Result == 0u) { std::cout << "[Add Function]: Result shouldn't be zero anymore! ID: " << '\n'; throw ID; }
      if (Result >= Max_Size) { throw "[Add Function]: Critical index overflow error.\n"; }
    }

    void Add() 
    { 
        // if (_payload->Add_On){
        //   TestAdd(Next_Empty);
        // _payload->Add_On = false; }
        // 
        if (_payload->Single_Add_On){
          TestAdd(_payload->SpecificID);
        _payload->Single_Add_On = false; }
    }

    void AddRange()
    {   
      const auto qty = _payload->Range;

      auto Previous_Empty = Next_Empty;
      const auto Randomness = _payload->Randomness;
      if (_payload->Activate_Range) {
      for (int i = 1; i < qty;)
      {
        if (Next_Empty >= Max_Size) { std::cout << "Returning\n"; _payload->Activate_Range = false; return; }
        int random = dist(rd);
        int counter = 0;
        if (random < Randomness)
        {
          random = Rangedist(rd);
          TestAdd(random);
        }
        else { TestAdd(Next_Empty);}
        if (Previous_Empty != Next_Empty) { Previous_Empty = Next_Empty; i++; }
      }
    }_payload->Activate_Range = false;

    }

    void Remove(const Entity& ID) 
    { 
      throw 0;
      // if (ID == 0u) { return; }
      // auto Possible_Index = ID;
      //
      // if (ID_Exists_On_Left(ID) && (S[ID] == ID)) { Swap_And_Remove_Last_Entity(ID, Possible_Index); } //Should be refactored
      // else
      // {
      //   Possible_Index = Find_Exact_Location(ID); //Implicitly an exact match at this point (or the Null Entity 0)
      //   // if (Possible_Index == 0u) { std::cout << "ID " << ID << " not found\n"; return; }
      //
      //   Swap_And_Remove_Last_Entity(ID, Possible_Index);
      // }
      // Elements[ID].~T();
    }

    Entity Find(const Entity ID) { return Find_Exact_Location(ID); }
    
    void Clear()
    {
      for (Index i = 0u; i < Max_Size; i++) 
      { 
        S[i] = 0u; 
        Elements[i].UpdateColorData(Empty_R, Empty_G, Empty_B);
      }
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

    void Update(Event<void(Shader), 1>& changeShaderSignal)
    {
      // if (trajectory.Active) 
      // {
      //   MotionQuad.hidden = false;
      //   trajectory.Process();
      //   MotionQuad.UpdateLocationData(trajectory.GetX0(), trajectory.GetY0());
      // }
      // else {
        // MotionQuad.hidden = true; }
      
      Add();
      AddRange();
      
      changeShaderSignal.Invoke(Shader::QUADCOLORALPHA);

      for (std::size_t i=0u; i!=Max_Size; i++)
      {
        glBindVertexArray(Elements[i].VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      }
      // if (MotionQuad.hidden) { return; }
      // else {
      //   glBindVertexArray(MotionQuad.VAO);
      //   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      // }
    }

  private:
    QuadColorAlpha Elements[Max_Size];               //Contiguous array of objects
    std::size_t S[Max_Size];              //Sparse array of entity IDs

    std::size_t Count = 0u;               //Count represents the number of entities
    std::size_t Next_Empty = Count + 1u;  //Count + 1u represents the next "empty" element in our set.
    
    std::size_t Level = Starting_Level;                              
    std::size_t Get_Next_Level = 1u << Level;           
    std::size_t Get_Current_Level = 1u << (Level - 1u);
    std::size_t Level_Bitmask = Get_Next_Level - 1u;
    std::size_t Old_Bitmask = Get_Current_Level - 1u;

    HSet_Payload* _payload;
    QuadColorAlpha MotionQuad;
    MotionTrajectory trajectory;

    std::random_device rd;
    std::uniform_int_distribution<int> dist;
    std::uniform_int_distribution<int> Rangedist;
};
