# Hybrid Memory - A solution to Web3 Gaming architecture

Author: Mike Fabrizio

Submission for the 2023 Algorand Global Hackathon

Warning - Experimental - May contain indexing errors. Use with caution.

## Table of Contents

* [A Fast Iteration & Random Access Container](#A-Fast-Iteration-&-Random-Access-Container)

## A Fast Iteration & Random Access Container

The goal of my work is to empower Web3 Gaming. There is however, an inescable problem. Memory implementation really matters.

We can now provide a direct bridge from the game data layer to the Web 3 layer, so that we can precisely and quickly reward players for positive achievements in game.

When representing data there will always be trade offs in memory efficiency. This container is no different. However, the trade-offs are quite acceptable given the other benefits. 
It takes significant inspiration from the Sparse Set model documented here and here.

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


| ***Future Roadmap***                       | *Timeline*   |
|--------------------------------------------|--------------|
| 2D Rendering Engine Layer                  | 6 months     |
| Algorand Compatibility                     | 6 months     |
| Rewards System                             | 1-2 year     |
| Accurate Anti-Cheat Web3 Lobbies           | 1-2 years    |
| 3D Rendering Engine Layer                  | TBD          |

