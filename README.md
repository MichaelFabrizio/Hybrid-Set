# Hybrid Memory - A data solution for Web3 + Entity Component Systems

Author: Mike Fabrizio

Submission for the 2023 Algorand Global Hackathon

Warning - Use with caution. May contain index errors (still needs auditing).

![alt text][Banner]

[Banner]: https://github.com/MichaelFabrizio/Hybrid-Set/raw/main/References/Images/Hackathon_Banner.png "OpenGL rendering"


## Table of Contents

* [A Fast Iteration & Random Access Container](#A-Fast-Iteration-And-Random-Access-Container)
* [The Problem For Web3 Gaming](#The-Problem-For-Web3-Gaming)
* [Verifying Game Data (Memory)](#Verifying-Game-Data)
* [Introducing The HSet](#Introducing-The-Hset)
* [Highest Optimization Approach](#Highest-Optimization-Approach)
* [The Null Entity 0](#The-Null-Entity-0)
* [Compression](#Compression)
* [How It Works](#How-It-Works)
* [Demo](#Demo)
* [Roadmap](#Roadmap)
* [Collaboration](#Collaboration)


## A Fast Iteration And Random Access Container
-----------------------------------------------
The goal of my work is to empower Web3 Gaming. There is however, an inescable problem. Memory implementation really matters.

We can now provide a direct bridge from the game data layer to the Web 3 layer, so that we can precisely and quickly reward players for positive achievements in game.

When representing data there will always be trade offs in memory efficiency. This container is no different. However, the trade-offs are quite acceptable given the other benefits. 
It takes significant inspiration from the Sparse Set model which is very well documented. 


| ***Memory Access Features***               | *Sparse Set* | *Hybrid Set*    |
|--------------------------------------------|--------------|-----------------|
| Fastest N Element Iteration                | O(N)         | O(N)            |
| Very Fast Lookup                           | O(1)         | O(1 - Const)    |
| Contiguous                                 | **✓**        | **✓**           |
| Composition Architecture                   | **✓**        | **✓**           |
| Integers Lookup                            | **✓**        | **✓**           |
| Pageable memory                            | **✓**        | **✓**           |
| High Cache Locality                        | Usually      | **✓** `Always`  |
| Auditable Data Structure                   | Maybe        | **✓** `Always`  |
| Memory Compression                         | Maybe        | **✓** `Optional`|
| Pointer Stability                          | Maybe        |   ✘             |
| Self Ordering (Promotion algorithm)        | ✘            | **✓**           |


One thing I would like to add before continuing: 
I believe Sparse Sets are themselves very capable containers. They have O(1) Lookup, Add, Remove... Which is really great! To be clear, all memory layouts are a matter of compromise.


## The Problem For Web3 Gaming
------------------------------
Let's start with a definition for Web3 Gaming:

**Web3 Gaming** - A multiplayer environment where *players* are *rewarded* for their *participation*.


How can we reward participation? That is fundamentally the question we must answer. With today's games having near endless complexity,
we need a way to **verify** *exactly* what is happening in-game (Game State).

In other words, there *must* be a *Bridge of data* between the Blockchain Layer (verified, trustless), and the Game Data Layer* (**unverified, untrusted**).


![alt text][Bridge]

[Bridge]: https://github.com/MichaelFabrizio/Hybrid-Set/raw/main/References/Images/Vote_Committee.png "Bridge Of Data"


Unfortunately, there is a weakest link for our data-bridge. 
We need to **publish events** to the blockchain, except those **events** are **game events**, which are inherently **untrusted**.

#### Stopping Cheaters
----------------------
A malicious player could spoof any data they wanted. **This is a huge problem**.

* **Q**: How do we prevent a malicious hacker from FALSLY CLAIMING they achieved something.
* **Example**: The hacker CLAIMS they defeated a fire dragon. **Did they actually?** 
Are you about to give them a **potentially RARE** Web3 token or NFT, without verifying if it's true?

* **A**: We *need* to *verify* by a (honest) voting committee that what the player CLAIMS is true. 
We only give rewards *after* confirming they told the truth.


#### Verifying Game Data
------------------------

Imagine our goal is to build a modern Web3 game. How will that look *In Practice*?

Well, let's consider a modern FPS and the possibilities. We'd likely find:

- *Different weapons, armors, loadouts, perks*
- *Character levels, unique skins, crafting*
- *Competitive and unranked type gameplay*

All of which is represented *somewhere* in memory. 


Ok, yada yada, so what's the point?

Literally **every question** we need to ask about **what happened in our game**, is ultimately **stored some place in memory**.

Either our memory is extremely fast, *or the entire data-bridge is made slower*.


## Introducing The HSet
-----------------------
![alt text][HSet]

[HSet]: https://github.com/MichaelFabrizio/Hybrid-Set/raw/main/References/Images/HSet_Simplified.png "Hybrid Set"


The H-Set is a double array structure with these properties:

### The Top Array
-----------------
- Contains the data itself
- It is contiguous, therefore it has perfect cache locality and O(N) iteration for N elements
- Data is "unordered", (although it can achieve perfect ordering: see [Highest Optimization Approach](#Highest-Optimization-Approach))
- There is no pointer stability. Any pointers to data may be invalidated after any data change. There is one exception to pointer stability: see [The Null Entity 0](#The-Null-Entity-0))

### The Bottom Array
--------------------
- Contains the *indexes* to the data itself (used for lookup operations)
- Uses a sorting & promoting algorithm that I designed: see [Placement Method](###The-Placement-Method))
- Optimized to be as compact as possible
- Pageable memory


What does this looks like in practice?

![alt text][HSetFill]

[HSetFill]: https://github.com/MichaelFabrizio/Hybrid-Set/raw/main/References/Images/HSet_Filled.png "In Practice"


We can easily access all data, and this is reliably fast in all cases.

#### Example) Lookup:

An HSet stores the **character IDs** as **keys** themselves to find relevant data. For example:

Suppose we are representing many sets of armor:

```c++
HSet<ArmorType, 64> ArmorSet;        // Storage for 64 pieces of armor data (on the stack)

auto Player_Armor = ArmorSet.Get(0); // Suppose we know some ID = 0 representing our player character

Player_Armor.Calculate_Damages();    // Just for example :)
```

#### Example) Add:

```c++
// We assign a default armor to character ID = 2

Entity ID = 2;
ArmorSet.Add(ID)

// Alternatively a method to deploy a constructed piece of armor with arguments

Entity ID = 2;
ArmorSet.Add(ID, 12, 31) //Ex: Defense stat 12, Magic Defense stat 31
```

The private HSet.Place() Method is, necessarily, the most important logic with the HSet data structure. Indices must be placed *very specifically*, 
otherwise we've completely invalidated the (Top Array) data structure.

#### Example) Remove, Clear
Not yet implemented.

### Highest Optimization Approach
---------------------------------

![alt text][HSetBestCase]

[HSetBestCase]: https://github.com/MichaelFabrizio/Hybrid-Set/raw/main/References/Images/HSet_Best_Case.png "Best case scenario"

The fastest use-case is when all numbers are in sequence. This provides a direct link of **<Key, Value>** pairings for each number.

Furthermore, the data structure is self-assembling: *As it fills more completely, the indices find their natural ordering*.


In other words, the data structure is *fastest* under two easily satisfied conditions:
- When completely filled
- When adding numbers sequentially from 0 (so long as nothing is later removed from that set)


### The Null Entity 0
---------------------

Regarding the lack of pointer stability, there is one exception: ID = 0 yields **only stable pointer in the structure**.
Why is this relevant? This data could represent, for example, the armor on a player character, and it will always be accessible via pointer or the Get(0) method:

```
ArmorType * player_armor = Armor.Get(0); // Valid to store this pointer (for as long as the HSet lifetime is valid)

player_armor.Active_Magic_Spell(Protection);
```

Not yet implemented. 

### Compression
---------------

There is compression in the private demo version only.

## How It Works - Placement
---------------
Soon!

## Demo
-------

[![IMAGE ALT TEXT](http://img.youtube.com/vi/PRYaIJNHNfU/0.jpg)](http://www.youtube.com/watch?v=PRYaIJNHNfU "Hybrid Memory")

## Roadmap
----------

| ***Future Roadmap***                       | *Timeline*   |
|--------------------------------------------|--------------|
| 2D Rendering Engine Layer                  | 6 months     |
| Algorand Compatibility                     | 6 months     |
| Rewards System                             | 1-2 year     |
| Accurate Anti-Cheat Web3 Lobbies           | 1-2 years    |
| 3D Rendering Engine Layer                  | TBD          |

#### Decentralization
The working plan is to provide a "standardized" way for voting parties to reach consensus on Game State and Algorand State.

#### Collaboration
------------------

For information about Sparse Sets or ECS, I highly recommend the blogs of EnTT creator skypjack [here](https://skypjack.github.io/ "EnTT blog"),
as well as the writings of Stefan Reinalter at the [Molecule Engine's Blog](https://blog.molecular-matters.com "Molecule Engine blog").
Their work greatly aided my understanding of these design problems.

I'm happy to collaborate further. Let's make our Web3 dreams a reality. Contact me on twitter @Learn\_Anew.
