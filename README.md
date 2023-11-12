# Hybrid Memory - A solution to Web3 Gaming architecture

Author: Mike Fabrizio

Submission for the 2023 Algorand Global Hackathon

Warning - Experimental - May contain index errors. Use with caution. 

## Table of Contents

* [A Fast Iteration & Random Access Container](#A-Fast-Iteration-&-Random-Access-Container)
* [The Problem For Web3 Gaming](#The-Problem-For-Web3-Gaming)
* [Verifying Game Data (Memory)](#Verifying-Game-Data)
* [Introducing The HSet](#Introducing-The-Hset)
* [How It Works](#How-It-Works)
* [Demo](#Demo)
* [Roadmap](#Roadmap)


## A Fast Iteration & Random Access Container

The goal of my work is to empower Web3 Gaming. There is however, an inescable problem. Memory implementation really matters.

We can now provide a direct bridge from the game data layer to the Web 3 layer, so that we can precisely and quickly reward players for positive achievements in game.

When representing data there will always be trade offs in memory efficiency. This container is no different. However, the trade-offs are quite acceptable given the other benefits. 
It takes significant inspiration from the Sparse Set model which is very well documented. 

For information about Sparse Sets or ECS, I highly recommend the blogs of EnTT creator skypjack [here](https://skypjack.github.io/ "EnTT blog"),
as well as the writings of Stefan Reinalter at [blog.molecular-matters.com](blog.molecular-matters.com "Molecule Engine blog").
Their work greatly aided my understanding of these design problems.


| ***Memory Access Features***               | *Sparse Set* | *Hybrid Set*   |
|--------------------------------------------|--------------|----------------|
| Fastest N Element Iteration                | O(N)         | O(N)           |
| Very Fast Lookup                           | O(1)         | O(1 - Const)   |
| Contiguous                                 | **✓**        | **✓**          |
| Composition Architecture                   | **✓**        | **✓**          |
| Integers Lookup                            | **✓**        | **✓**          |
| Pageable memory                            | **✓**        | **✓**          |
| High Cache Locality                        | Usually      | **✓** Always   |
| Auditable Data Structure                   | Maybe        | **✓** Always   |
| Memory Compression                         | Maybe        | **✓** Optional |
| Pointer Stability                          | Maybe        |   ✘            |
| Self Ordering (Promotion algorithm)        | ✘            | **✓**          |


One thing I would like to add before continuing: 
I believe Sparse Sets are themselves very capable containers. They have O(1) Lookup, Add, Remove... Which is really great! To be clear, all memory layouts are a matter of compromise.


## The Problem For Web3 Gaming

Let's start with a definition for Web3 Gaming:

**Web3 Gaming** - A multiplayer environment where *players* are *rewarded* for their *participation*.


How can we reward participation? That is fundamentally the question we must answer. With today's games having near endless complexity,
we need a way to **verify** *exactly* what is happening in-game (Game State).

In other words, there *must* be a *Bridge of data* between the Blockchain Layer (verified, trustless), and the Game Data Layer* (**unverified, untrusted**).







Unfortunately, there is a weakest link for our data-bridge. 
We need to **publish events** to the blockchain, except those **events** are **game events**, which are inherently **untrusted**.


A malicious player could spoof any data they wanted. **This is a huge problem**.

**Q**: How do we prevent a malicious hacker from FALSLY CLAIMING they achieved something.
**Example**: The hacker CLAIMS they defeated a fire dragon. **Did they actually?** Are you about to give them a **potentially RARE** Web3 token or NFT, without verifying if it's true?

**A**: We *need* to *verify* by a (honest) voting committee that what the player CLAIMS is true. We only give rewards *after* confirming they told the truth.


## Verifying Game Data

Imagine our goal is to build a modern Web3 game. How will that look *In Practice*?

Well, let's consider a modern FPS and the possibilities. We'd likely find:

-*Different weapons, armors, loadouts, perks*
-*Character levels, unique skins, crafting*
-*Competitive and unranked type gameplay*

All of which is represented *somewhere* in memory. 


Ok, yada yada, so what's the point?

Literally **every question** we need to ask about **what happened in our game**, is ultimately **stored some place in memory**.

Either our memory is extremely fast, *or the entire data-bridge is made slower*.


## Introducing The HSet

![alt text][HSet]

[HSet]: https://github.com/MichaelFabrizio/Hybrid-Set/raw/main/References/Images/HSet_Simplified.png "Hybrid Set"

## How It Works
Soon!

## Demo
Soon!

## Roadmap

| ***Future Roadmap***                       | *Timeline*   |
|--------------------------------------------|--------------|
| 2D Rendering Engine Layer                  | 6 months     |
| Algorand Compatibility                     | 6 months     |
| Rewards System                             | 1-2 year     |
| Accurate Anti-Cheat Web3 Lobbies           | 1-2 years    |
| 3D Rendering Engine Layer                  | TBD          |

