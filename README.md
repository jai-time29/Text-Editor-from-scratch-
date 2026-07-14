I. Project: Text-Editor-from-scratchI.
Executive Summary
Text-Editor-from-scratch is a high-performance, raw-terminal-based smart text editor built from the ground up in C++.
By shifting away from high-level abstractions like vectors of strings, this project explores low-level systems programming and 
fundamental computer science paradigms.The editor features a matrix of Gap Buffers for $O(1)$ inline text mutations, 
a decoupled Command Pattern Undo/Redo Engine, linear-time Knuth-Morris-Pratt (KMP) text searching, and a predictive language engine driven by
Tries and Bigram Frequency Maps.

II. Core System Architecture
The application relies on a strictly decoupled, production-grade layered architecture.
The core data structures, undo layers, and AI components have zero direct awareness of the physical hardware keyboard.
Instead, a custom processing layer converts live hardware events into abstract operational commands before they propagate down into the document graph.


[ Raw Hardware Keyboard ]
                                 │
                                 ▼
                     [ POSIX Raw InputHandler ]
                                 │
                                 ▼
                    [ Decoupled Command Layer ]
                                 │
                                 ▼
                       [ Core Editor Engine ]
                                 │
         ┌───────────────────────┼───────────────────────┐
         ▼                       ▼                       ▼
   [ Document ]           [ Undo Manager ]       [ AI Panel Console ]
         │                 (Command Stack)       (Trie / Bigram Maps)
         ▼
 ┌───────────────┐
 │ Line 0: [Gap] │
 ├───────────────┤
 │ Line 1: [Gap] │
 └───────────────┘

III. ========================================================================================================
                                     ALGORITHMIC DATA LAYER SPECS                                       
========================================================================================================

 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  COMPONENT LAYER:          Text Mutation                                                           │
 │  ALGORITHMIC ENGINE:       Gap Buffer Matrix                                                       │
 │  COMPUTATIONAL RECON:      Insertion/Deletion: O(1)  │  Cursor Move: O(k)                          │
 │  ENGINEERING IMPACT:       Shakes off the O(n) cost of typical string arrays. Shifting a dynamic   │
 │                            text gap allows localized edits to execute in constant time.            │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  COMPONENT LAYER:          History Tracking                                                        │
 │  ALGORITHMIC ENGINE:       Command Pattern Engine                                                  │
 │  COMPUTATIONAL RECON:      Push/Pop: O(1)                                                          │
 │  ENGINEERING IMPACT:       Encapsulates every change into reversible transaction tokens stored     │
 │                            inside a dual-stack history model for infinite tracking.                │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  COMPONENT LAYER:          Pattern Search                                                          │
 │  ALGORITHMIC ENGINE:       Knuth-Morris-Pratt (KMP)                                                │
 │  COMPUTATIONAL RECON:      Pre-processing: O(m)  │  Matching: O(n)                                 │
 │  ENGINEERING IMPACT:       Scans cross-row document states using a pre-computed Longest Prefix     │
 │                            Suffix (LPS) array without ever backtracking indices.                   │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  COMPONENT LAYER:          Sub-Word Lookup                                                         │
 │  ALGORITHMIC ENGINE:       Prefix Tree (Trie)                                                      │
 │  COMPUTATIONAL RECON:      Suffix Suggestion: O(p + σ)                                             │
 │  ENGINEERING IMPACT:       Organizes dictionary words into recursive character graphs, producing    │
 │                            predictive auto-suggestions under sub-millisecond timelines.             │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  COMPONENT LAYER:          Context Prediction                                                      │
 │  ALGORITHMIC ENGINE:       N-Gram Bigram Frequency Map                                             │
 │  COMPUTATIONAL RECON:      Lookup: O(1)                                                            │
 │  ENGINEERING IMPACT:       Parses historical word pairings inside a double-hash structure to       │
 │                            forecast the most statistically probable next token.                    │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘

========================================================================================================

IV.
IV.  Connecting raw hardware events directly to the editor's core engine required bypassing standard operating system terminal behaviors. By default, terminal standard inputs utilize line-buffering (waiting for an explicit Enter keypress).

The InputHandler reconfigures low-level POSIX/terminal flags (ICANON and ECHO) to turn off buffering and local mirroring,establishing an instantaneous stream. 
Standard characters are parsed directly, while control signals and escape blocks are transformed into a standardized command structure:

```text
========================================================================================================
                               HARDWARE-TO-ABSTRACT COMMAND MAPPING ENGINE                              
========================================================================================================

 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Alphanumeric Keys                                                       │
 │  COMMAND TOKEN:            INSERT_CHARACTER                                                        │
 │  EXECUTION PATH:           Pushes the raw ASCII payload into the active localized gap node.        │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Backspace / Delete                                                      │
 │  COMMAND TOKEN:            DELETE_CHARACTER                                                        │
 │  EXECUTION PATH:           Shifts the gap boundary left, or performs vertical row structural       │
 │                            merges.                                                                 │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Arrow Keys (ESC [ A-D)                                                  │
 │  COMMAND TOKEN:            CURSOR_MOVEMENT                                                         │
 │  EXECUTION PATH:           Repositions boundary offsets or moves lines while adjusting bounds      │
 │                            safely.                                                                 │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Tab Key (\t)                                                            │
 │  COMMAND TOKEN:            AUTOCOMPLETE                                                            │
 │  EXECUTION PATH:           Intercepts the top prediction string and injects missing suffix         │
 │                            characters.                                                             │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Ctrl + Z (ASCII 26)                                                     │
 │  COMMAND TOKEN:            UNDO                                                                    │
 │  EXECUTION PATH:           Reverses the last transaction receipt and updates state properties.     │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Ctrl + Y (ASCII 25)                                                     │
 │  COMMAND TOKEN:            REDO                                                                    │
 │  EXECUTION PATH:           Re-applies popped transaction receipts forward onto the document graph. │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Ctrl + S (ASCII 19)                                                     │
 │  COMMAND TOKEN:            FILE_SAVE                                                               │
 │  EXECUTION PATH:           Serializes active gap buffer sequences back into a clean flat disk text  │
 │                            file.                                                                   │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Ctrl + F (ASCII 6)                                                      │
 │  COMMAND TOKEN:            TEXT_FIND                                                               │
 │  EXECUTION PATH:           Activates KMP text scanning across the current layout coordinates.       │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘
                                                   │
                                                   ▼
 ┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
 │  HARDWARE TRIGGER:         Ctrl + Q (ASCII 17)                                                     │
 │  COMMAND TOKEN:            WORKSPACE_QUIT                                                          │
 │  EXECUTION PATH:           Terminates the raw application frame and safely restores terminal        │
 │                            system states.                                                          │
 └────────────────────────────────────────────────────────────────────────────────────────────────────┘

========================================================================================================

```
V.. Compilation & Deployment Lifecycle
To deploy the application natively, ensure your build environment features a modern compiler toolchain supporting C++17 or higher standard specifications.

# 1. Clone the repository down to your local directory architecture
git clone https://github.com/yourusername/Text-Editor-from-scratch.git
cd Text-Editor-from-scratch

# 2. Compile using a standard C++17 compiler toolchain optimization pass
g++ -std=c++17 day5_editor.cpp -o smart_editor

# 3. Launch the native binary application execution layer
./smart_editor

VI. Engineering Reflections & Lessons Learned
The Hardware-to-Software Bridge (ANSI Sequence Parsing)
Operating systems transmit multi-byte ANSI escape sequences for non-printable triggers(such as ESC [ A for the Up Arrow).
Developing a custom parsing pipeline capable of intercepting these sequences instantly without locking runtime frames or corrupting 
valid character payloads required precise, low-level lookahead scanning of input streams.

The Glitchy Undo Stack (Context Isolation)
A major synchronization hurdle occurred when mapping hardware shortcut triggers (Ctrl+Z, ASCII 26) to the internal transaction stack.
The signal was originally treating the trigger itself as text input, resulting in cursor glitches, unintended text drops, and broken line states.
Resolving this required strict execution filtering: macro keystrokes are completely isolated from text-insertion pipelines. This ensures that the history 
manager logs clean user text modifications, stabilizing state restoration across multiple rows.

