
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include<bits/stdc++.h>

class GapBuffer {
    
public:

    std::vector<char> buffer;
    int gapStart;
    int gapEnd; // gapEnd is exclusive (points to the first character after the gap)

    // Helper function to grow the buffer when it runs out of space
    void resizeBuffer() {
        int oldSize = buffer.size();
        int newSize = oldSize * 2;
        int gapLength = gapEnd - gapStart;
        
        // Resize the vector to the new capacity
        buffer.resize(newSize);

        // Shift everything after the gap to the very end of the new buffer
        int elementsToShift = oldSize - gapEnd;
        int newGapEnd = newSize - elementsToShift;

        for (int i = elementsToShift - 1; i >= 0; --i) {
            buffer[newGapEnd + i] = buffer[gapEnd + i];
        }

        // Update the gapEnd to its new position
        gapEnd = newGapEnd;
    }


    // Step 1: Initialize the internal representation
    GapBuffer(int initialCapacity = 8) {
        buffer.resize(initialCapacity);
        gapStart = 0;
        gapEnd = initialCapacity;
    }
    // Splits the current buffer at the cursor (gapStart).
// It keeps everything before the cursor, and returns a new GapBuffer with everything after.
GapBuffer splitAtCursor() {
    GapBuffer newLine;
    
    // Temporarily get everything after the gap
    std::vector<char> remainingText;
    for (int i = gapEnd; i < buffer.size(); ++i) {
        remainingText.push_back(buffer[i]);
    }
    
    // Clear the right side of the gap in this buffer
    buffer.resize(gapStart);
    gapEnd = gapStart; 
    
    // Insert the remaining text into the new line buffer
    for (char c : remainingText) {
        newLine.insert(c);
    }
    
    // Move the new line's cursor to its beginning
    while (newLine.gapStart > 0) {
        newLine.moveLeft();
    }
    
    return newLine;
}

    // Step 2: Implement insertion logic
    void insert(char c) {
        // If gapStart meets gapEnd, our gap is full! We must resize.
        if (gapStart == gapEnd) {
            resizeBuffer();
        }

        // Put character at the beginning of the gap
        buffer[gapStart] = c;
        // Move the start of the gap forward
        gapStart++;
    }
    void insertString(const std::string& str) {
    int strLen = str.length();

    // 1. Ensure the gap is wide enough for the entire string
    while ((gapEnd - gapStart) < strLen) {
        resizeBuffer(); // Keep doubling until it fits
    }

    // 2. Copy the whole string into the gap
    for (int i = 0; i < strLen; ++i) {
        buffer[gapStart + i] = str[i];
    }

    // 3. Move gapStart forward by the length of the string
    // The cursor is now at the end of the newly inserted string
    gapStart += strLen;
    }

    // Step 4: Implement deletion (Backspace)
    void deleteChar() {
        // We can only delete if there is text to the left of the gap
        if (gapStart > 0) {
            // Moving gapStart back effectively "swallows" the character into the gap
            gapStart--;
        }
    }

    // Step 3: Implement cursor movement (Left Arrow)
    void moveLeft() {
        // Can only move left if there is text to the left of the gap
        if (gapStart > 0) {
            // Move character from left side of gap to the right side of gap
            gapStart--;
            gapEnd--;
            buffer[gapEnd] = buffer[gapStart];
        }
    }

    // Step 3: Implement cursor movement (Right Arrow)
    void moveRight() {
        // Can only move right if there is text to the right of the gap
        if (gapEnd < buffer.size()) {
            // Move character from right side of gap to the left side of gap
            buffer[gapStart] = buffer[gapEnd];
            gapStart++;
            gapEnd++;
        }
    }

    // Dynamic text retrieval for testing
    std::string getText() {
        std::string result = "";
        
        // 1. Grab everything before the gap
        for (int i = 0; i < gapStart; ++i) {
            result += buffer[i];
        }
        
        // 2. Grab everything after the gap
        for (int i = gapEnd; i < buffer.size(); ++i) {
            result += buffer[i];
        }
        
        return result;
    }
};
#include <stack>

// Enumeration to distinguish the type of action performed
enum class ActionType {
    INSERT_CHAR,
    DELETE_CHAR,
    PRESS_ENTER,    // <-- Add this
    MOVE_UP,        // <-- Add this
    MOVE_DOWN       // <-- Add this
};

// The tiny "Receipt" object that holds exactly what changed
struct EditorAction {
    ActionType type;
    char ch;        // The character involved
    int line;       // The row line where it happened
    int col;        // The column where it happened
};

class HistoryManager {
private:
    std::stack<EditorAction> undoStack;
    std::stack<EditorAction> redoStack;

public:
    // Record a brand new action taken by the user
    void recordAction(ActionType type, char ch, int line, int col) {
        undoStack.push({type, ch, line, col});
        
        // CRITICAL: Any new user action wipes out the Redo history!
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }

    EditorAction popUndo() {
        EditorAction action = undoStack.top();
        undoStack.pop();
        redoStack.push(action); // Move it over to redo
        return action;
    }

    EditorAction popRedo() {
        EditorAction action = redoStack.top();
        redoStack.pop();
        undoStack.push(action); // Move it back to undo
        return action;
    }
};






#include <unordered_set>
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>

// ==========================================
// PART 1: TRIE AUTOCOMPLETE
// ==========================================
struct TrieNode {
    std::unordered_map<char, TrieNode*> children;
    bool isEndOfWord = false;
};

class TrieAutocomplete {
private:
    TrieNode* root;

    void getSuggestionsRecursive(TrieNode* curr, std::string currentPrefix, std::vector<std::string>& results) {
        if (results.size() >= 3) return; // Cap at top 3 suggestions
        if (curr->isEndOfWord) {
            results.push_back(currentPrefix);
        }
        for (auto const& [ch, childNode] : curr->children) {
            getSuggestionsRecursive(childNode, currentPrefix + ch, results);
        }
    }

public:
    TrieAutocomplete() { root = new TrieNode(); }

    void insert(const std::string& word) {
        TrieNode* curr = root;
        for (char ch : word) {
            if (curr->children.find(ch) == curr->children.end()) {
                curr->children[ch] = new TrieNode();
            }
            curr = curr->children[ch];
        }
        curr->isEndOfWord = true;
    }

    std::vector<std::string> suggest(const std::string& prefix) {
        std::vector<std::string> results;
        if (prefix.empty()) return results;

        TrieNode* curr = root;
        for (char ch : prefix) {
            if (curr->children.find(ch) == curr->children.end()) {
                return results; // Prefix not found
            }
            curr = curr->children[ch];
        }
        getSuggestionsRecursive(curr, prefix, results);
        return results;
    }
};

// ==========================================
// PART 2: SPELL CHECKER
// ==========================================
class SpellChecker {
private:
    std::unordered_set<std::string> dictionary;

public:
    void loadWord(const std::string& word) {
        dictionary.insert(word);
    }

    bool check(const std::string& word) const {
        if (word.empty()) return true;
        // Strip punctuation for pure word validation
        std::string cleanWord = "";
        for (char c : word) {
            if (std::isalpha(c)) cleanWord += std::tolower(c);
        }
        if (cleanWord.empty()) return true;
        return dictionary.find(cleanWord) != dictionary.end();
    }
};

// ==========================================
// PART 3: WORD RECOMMENDATION (NEXT-WORD PREDICTION)
// ==========================================
class WordRecommender {
private:
    // Maps a word to a sub-map tracking next-words and their frequency counts
    std::unordered_map<std::string, std::unordered_map<std::string, int>> bigramMap;

public:
    void train(const std::string& prev, const std::string& next) {
        if (prev.empty() || next.empty()) return;
        bigramMap[prev][next]++;
    }

    std::vector<std::string> recommendNextWords(const std::string& prevWord) {
        std::vector<std::string> recommendations;
        if (bigramMap.find(prevWord) == bigramMap.end()) return recommendations;

        // Sort by frequency count using a vector of pairs
        std::vector<std::pair<std::string, int>> sortedWords(bigramMap[prevWord].begin(), bigramMap[prevWord].end());
        std::sort(sortedWords.begin(), sortedWords.end(), [](const auto& a, const auto& b) {
            return a.second > b.second; 
        });

        // Fetch top 3 context recommendations
        for (size_t i = 0; i < sortedWords.size() && i < 3; ++i) {
            recommendations.push_back(sortedWords[i].first);
        }
        return recommendations;
    }
};



#include <iostream>

// The unified command layout representing operations the editor can execute
enum class EditorCommandType {
    NONE,
    INSERT_CHARACTER,
    DELETE_CHARACTER,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    PRESS_ENTER,
    AUTOCOMPLETE,
    UNDO,
    REDO,
    SAVE,
    OPEN,
    FIND,
    METRICS,
    QUIT
};

// Wraps the command type with an optional character payload (for text insertion)
struct EditorCommand {
    EditorCommandType type;
    char payloadChar = '\0';
};






#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

class KeyboardReader {
public:
    // Reads a single raw byte directly from the keyboard terminal buffer
    static int readRawKey() {
#ifdef _WIN32
        return _getch();
#else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Disable buffering and local echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore settings
        return ch;
#endif
    }
};




class InputHandler {
public:
    static EditorCommand parseInput() {
        int key = KeyboardReader::readRawKey();
        EditorCommand cmd;
        cmd.type = EditorCommandType::NONE;

        // --- Step 4 & 8: Handle Special Key Escape Sequences (Unix Arrow Keys) ---
        if (key == 27) { // Escape key hit
            int next1 = KeyboardReader::readRawKey();
            if (next1 == '[') {
                int next2 = KeyboardReader::readRawKey();
                switch (next2) {
                    case 'A': cmd.type = EditorCommandType::MOVE_UP;    return cmd;
                    case 'B': cmd.type = EditorCommandType::MOVE_DOWN;  return cmd;
                    case 'C': cmd.type = EditorCommandType::MOVE_RIGHT; return cmd;
                    case 'D': cmd.type = EditorCommandType::MOVE_LEFT;  return cmd;
                }
            }
            return cmd;
        }

        // --- Step 7: Handle Control Key Bitmask Mappings ---
        if (key == 19) { cmd.type = EditorCommandType::SAVE;   return cmd; } // CTRL+S (ASCII 19)
        if (key == 26) { cmd.type = EditorCommandType::UNDO;   return cmd; } // CTRL+Z (ASCII 26)
        if (key == 25) { cmd.type = EditorCommandType::REDO;   return cmd; } // CTRL+Y (ASCII 25)
        if (key == 6)  { cmd.type = EditorCommandType::FIND;   return cmd; } // CTRL+F (ASCII 6)
        if (key == 13) { cmd.type = EditorCommandType::OPEN;   return cmd; } // CTRL+M -> re-mapped for File Open
        if (key == 17) { cmd.type = EditorCommandType::QUIT;   return cmd; } // CTRL+Q (ASCII 17)

        // --- Step 8: Handle Backspace/Delete & Enter Mappings ---
        if (key == 127 || key == 8) {
            cmd.type = EditorCommandType::DELETE_CHARACTER;
            return cmd;
        }
        if (key == '\n' || key == 10) {
            cmd.type = EditorCommandType::PRESS_ENTER;
            return cmd;
        }
        if (key == '\t' || key == 9) {
            cmd.type = EditorCommandType::AUTOCOMPLETE;
            return cmd;
        }

        // --- Step 3: Handle Regular Typable Text Character Input ---
        if (key >= 32 && key <= 126) {
            cmd.type = EditorCommandType::INSERT_CHARACTER;
            cmd.payloadChar = static_cast<char>(key);
            return cmd;
        }

        return cmd;
    }
};




class Document {
private:
    std::vector<GapBuffer> lines;
    int currentLine;
    HistoryManager history; // Day 3 History Integration

    // Clean internal coordinate locator used strictly to reposition the cursor during history rollbacks
    void jumpToCoordinates(int targetLine, int targetCol) {
        if (targetLine >= 0 && targetLine < lines.size()) {
            currentLine = targetLine;
            // Snap gap back to absolute left bound
            while (lines[currentLine].gapStart > 0) {
                lines[currentLine].moveLeft();
            }
            // Navigate rightwards precisely to the historic transaction slot
            while (lines[currentLine].gapStart < targetCol && 
                   lines[currentLine].gapStart < lines[currentLine].getText().length()) {
                lines[currentLine].moveRight();
            }
        }
    }

public:
    Document() {
        // Start with one empty line
        lines.push_back(GapBuffer());
        currentLine = 0;
    }

    // Integrated recording safely inside your original Proxy function
    void insertChar(char c, bool record = true) {
        int colBefore = lines[currentLine].gapStart;
        lines[currentLine].insert(c);
        
        if (record) {
            history.recordAction(ActionType::INSERT_CHAR, c, currentLine, colBefore);
        }
    }

    void deleteChar(bool record = true) {
        if (record && lines[currentLine].gapStart > 0) {
            int colBefore = lines[currentLine].gapStart;
            char deletedChar = lines[currentLine].getText()[colBefore - 1];
            history.recordAction(ActionType::DELETE_CHAR, deletedChar, currentLine, colBefore - 1);
        }
        lines[currentLine].deleteChar();
    }

    // Step 1: Cursor Position Tracking
    int getCurrentLineIndex() const { return currentLine; }
    
    int getCurrentColumnIndex() {
        return lines[currentLine].getText().substr(0, lines[currentLine].splitAtCursor().gapStart).length(); 
    }
    
    int getColumn() {
        return lines[currentLine].getText().length(); 
    }

    // Step 2: Implement Enter Key Logic
    
    void handleEnter(bool record = true) {
    if (record) {
        // Record that Enter was pressed at the current cursor position
        history.recordAction(ActionType::PRESS_ENTER, '\n', currentLine, lines[currentLine].gapStart);
    }

    // ... your existing Day 2 splitting logic goes here ...
    GapBuffer newLine = lines[currentLine].splitAtCursor();
    lines.insert(lines.begin() + currentLine + 1, newLine);
    currentLine++;
    }

    void moveLeft() {
        lines[currentLine].moveLeft();
    }

    void moveRight() {
        lines[currentLine].moveRight();
    }

    // Integrated mid-character backspace history inside your deletion routing logic
    void handleBackspace(bool record = true) {
        // Track interior characters disappearing
        if (lines[currentLine].gapStart > 0) {
            int colBefore = lines[currentLine].gapStart;
            char deletedChar = lines[currentLine].getText()[colBefore - 1];
            
            lines[currentLine].deleteChar();
            
            if (record) {
                history.recordAction(ActionType::DELETE_CHAR, deletedChar, currentLine, colBefore - 1);
            }
        } 
        // Fallback to structural multi-line merge mechanism from Day 2 if col == 0
        else if (lines[currentLine].getText() == "" || true) { 
            if (currentLine > 0) {
                std::string textToMove = lines[currentLine].getText();
                lines.erase(lines.begin() + currentLine);
                currentLine--;
                
                for(char c : textToMove) {
                    lines[currentLine].insert(c);
                }
            }
        }
    }
    void moveUp(bool record = true) {
    if (currentLine > 0) {
        if (record) {
            history.recordAction(ActionType::MOVE_UP, ' ', currentLine, lines[currentLine].gapStart);
        }
        currentLine--;
    }
}

    void moveDown(bool record = true) {
    if (currentLine < lines.size() - 1) {
        if (record) {
            history.recordAction(ActionType::MOVE_DOWN, ' ', currentLine, lines[currentLine].gapStart);
        }
        currentLine++;
    }
    }

    // New Step 3: Implement Undo Handler
    void undo() {
        if (!history.canUndo()) {
            std::cout << "Nothing to Undo!\n";
            return;
        }

        EditorAction action = history.popUndo();
        jumpToCoordinates(action.line, action.col);

        if (action.type == ActionType::INSERT_CHAR) {
            // Undo an insertion by manually scrubbing the text slot (record=false prevents infinite feedback)
            lines[currentLine].moveRight(); 
            lines[currentLine].deleteChar();
        } 
        else if (action.type == ActionType::DELETE_CHAR) {
            // Undo a deletion by pushing the original character back into place
            lines[currentLine].insert(action.ch);
            lines[currentLine].moveLeft(); 
        }
        
    else if (action.type == ActionType::PRESS_ENTER) {
    // Inverse of Enter is a Line Merge!
    // Go to the line right below the split line
    currentLine = action.line + 1;
    // Pull text up and delete the split row container, restoring Day 2 state
    std::string textToMove = lines[currentLine].getText();
    lines.erase(lines.begin() + currentLine);
    currentLine--;
    
    // Append back to the end of the original line
    while (lines[currentLine].gapStart < lines[currentLine].getText().length()) {
        lines[currentLine].moveRight();
    }
    for (char c : textToMove) lines[currentLine].insert(c);
} 
else if (action.type == ActionType::MOVE_UP) {
    // Undo a move up by shifting back down
    currentLine = action.line; 
} 
else if (action.type == ActionType::MOVE_DOWN) {
    // Undo a move down by shifting back up
    currentLine = action.line;
}
    }

    // New Step 4: Implement Redo Handler
    void redo() {
        if (!history.canRedo()) {
            std::cout << "Nothing to Redo!\n";
            return;
        }

        EditorAction action = history.popRedo();
        jumpToCoordinates(action.line, action.col);

        if (action.type == ActionType::INSERT_CHAR) {
            lines[currentLine].insert(action.ch);
        } 
        else if (action.type == ActionType::DELETE_CHAR) {
            lines[currentLine].moveRight();
            lines[currentLine].deleteChar();
        }
       
        // --- Inside Document::redo() ---

else if (action.type == ActionType::PRESS_ENTER) {
    // Simply call your enter logic again without recording a new action
    jumpToCoordinates(action.line, action.col);
    handleEnter(false);
} 
else if (action.type == ActionType::MOVE_UP) {
    // Re-execute the up movement
    currentLine = action.line - 1;
} 
else if (action.type == ActionType::MOVE_DOWN) {
    // Re-execute the down movement
    currentLine = action.line + 1;
}
    }

    
    

    void render() {
        std::cout << "\n----------------\n";
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i == currentLine) {
                std::cout << "> " << lines[i].getText() << "  <-- (Active)\n";
            } else {
                std::cout << "  " << lines[i].getText() << "\n";
            }
        }
        std::cout << "----------------\n";
        std::cout << "Cursor Position: line " << currentLine << "\n";
    }
    
    
std::vector<std::string> getAllLinesText() {
std::vector<std::string> textLines;
    for (auto& line : lines) {
        textLines.push_back(line.getText());
    }
    return textLines;
}


// Helper to fetch the last word and the word before it from the current active line
std::pair<std::string, std::string> getContextWords() {
        std::string fullLineText = lines[currentLine].getText();
        std::stringstream ss(fullLineText);
        std::string word;
        std::vector<std::string> words;
        
        while (ss >> word) {
            words.push_back(word);
        }

        std::string currentWord = "";
        std::string previousWord = "";
        
        if (!words.empty()) currentWord = words.back();
        if (words.size() > 1) previousWord = words[words.size() - 2];

        return {previousWord, currentWord};
    }

void executeCommand(const EditorCommand& cmd) {
        switch (cmd.type) {
            case EditorCommandType::INSERT_CHARACTER:
                insertChar(cmd.payloadChar);
                break;
            case EditorCommandType::DELETE_CHARACTER:
                handleBackspace(); // Maps directly to your structural backspace/merge logic
                break;
            case EditorCommandType::MOVE_LEFT:
                moveLeft();
                break;
            case EditorCommandType::MOVE_RIGHT:
                moveRight();
                break;
            case EditorCommandType::MOVE_UP:
                moveUp();
                break;
            case EditorCommandType::MOVE_DOWN:
                moveDown();
                break;
            case EditorCommandType::PRESS_ENTER:
                handleEnter();
                break;
            case EditorCommandType::UNDO:
                undo();
                break;
            case EditorCommandType::REDO:
                redo();
                break;
            default:
                break; // Handled at outer application frame loop layer
        }
    }

};
// class Document {
// private:
//     std::vector<GapBuffer> lines;
//     int currentLine;
//     HistoryManager history;
// public:
//     Document() {
//         // Start with one empty line
//         lines.push_back(GapBuffer());
//         currentLine = 0;
//     }

//     // Proxy functions to send standard operations to the active line
//     void insertChar(char c) {
//         lines[currentLine].insert(c);
//     }

    
//     void deleteChar() {
//         lines[currentLine].deleteChar();
//     }

//     // Step 1: Cursor Position Tracking
//     int getCurrentLineIndex() const { return currentLine; }
//     int getCurrentColumnIndex() {
//         // In a GapBuffer, gapStart is exactly the 0-indexed column position!
//         return lines[currentLine].getText().substr(0, lines[currentLine].splitAtCursor().gapStart).length(); 
//         // A cleaner way without mutating: just expose gapStart or get it from your class.
//     }
    
//     // Alternative clean logic for getting column:
//     // (Since gapStart is private, we look at the text length to the left of the gap)
//     int getColumn() {
//         // Let's implement a quick trick: extract the line text, 
//         // but since we want the actual gapStart index, let's look at the current line state.
//         // For Day 2, we can temporarily use the split or add a getter. 
//         // Assuming you can add a public 'int getCursorCol() { return gapStart; }' to GapBuffer, use that!
//         // Otherwise, this fallback calculates it based on total length minus characters after gap:
//         return lines[currentLine].getText().length(); // Simplified fallback for Day 2 tracking
//     }

//     // Step 2: Implement Enter Key Logic
//     void handleEnter() {
//         // 1. Split the current line at its cursor position
//         GapBuffer newLine = lines[currentLine].splitAtCursor();
        
//         // 2. Insert the new line right below the current line
//         lines.insert(lines.begin() + currentLine + 1, newLine);
        
//         // 3. Move cursor to the beginning of the new line
//         currentLine++;
//     }

//     // Step 3: Implement Backspace at line beginning
    
// // Updated Move Left: Handle line wrapping
//     void moveLeft() {
//         lines[currentLine].moveLeft();
//     }

//     // Updated Move Right: Handle line wrapping
//     void moveRight() {
//         lines[currentLine].moveRight();
//     }

//     // Updated Backspace: Handle both mid-line deletion and line-merging
//     void handleBackspace() {
//         if (lines[currentLine].getText() == "" || true) { // Day 2 structural logic
//             if (currentLine > 0) {
//                 // Fetch text from the line we are deleting
//                 std::string textToMove = lines[currentLine].getText();
                
//                 // Remove the current line completely
//                 lines.erase(lines.begin() + currentLine);
                
//                 // Drop up to the previous line
//                 currentLine--;
                
//                 // Append the text to the end of the previous line
//                 for(char c : textToMove) {
//                     lines[currentLine].insert(c);
//                 }
//             }
//         }
//     }
//     // Step 4: Add vertical movement
//     void moveUp() {
//         if (currentLine > 0) {
//             currentLine--;
//             // Implementation detail: Real editors adjust column if target line is shorter,
//             // but for core logic, switching the active line index satisfies Day 2!
//         }
//     }

//     void moveDown() {
//         if (currentLine < lines.size() - 1) {
//             currentLine++;
//         }
//     }

//     // Step 5: Add document rendering
//     void render() {
//         std::cout << "\n----------------\n";
//         for (size_t i = 0; i < lines.size(); ++i) {
//             if (i == currentLine) {
//                 std::cout << "> " << lines[i].getText() << "  <-- (Active)\n";
//             } else {
//                 std::cout << "  " << lines[i].getText() << "\n";
//             }
//         }
//         std::cout << "----------------\n";
//         std::cout << "Cursor Position: line " << currentLine << "\n";
//     }
// };

// Step 5: Testing
/*int main() {
    GapBuffer editor;
    char choice;
    char inputChar;

    while (true) {
        std::cout << "\n------------------------------------------\n";
        std::cout << "Current Buffer Text: [" << editor.getText() << "]\n";
        std::cout << "Commands: (i)nsert, (l)eft, (r)ight, (d)elete, (e)xit\n";
        std::cout << "Enter command: ";
        std::cin >> choice;

        if (choice == 'e') break;

        switch (choice) {
            case 'i':
                std::cout << "Enter character to insert: ";
                std::cin >> inputChar;
                editor.insert(inputChar);
                break;
            case 'l':
                editor.moveLeft();
                std::cout << "Moved cursor left.\n";
                break;
            case 'r':
                editor.moveRight();
                std::cout << "Moved cursor right.\n";
                break;
            case 'd':
                editor.deleteChar();
                std::cout << "Deleted character.\n";
                break;
            default:
                std::cout << "Invalid command!\n";
        }
    }
    return 0;
} */
//main loop for testing day1
// int main() {
//     GapBuffer editor;

//     // Insert a whole string at once
//     editor.insertString("hello");
//     std::cout << "Inserted string: [" << editor.getText() << "]\n";

//     // Go back 3 characters one by one
//     editor.moveLeft();
//     editor.moveLeft();
//     editor.moveLeft();
    
//     // Insert a single character inside the word
//     editor.insert('-'); 

//     // Expected: Uni-versal
//     std::cout << "After moving back 3 and inserting '-': [" << editor.getText() << "]\n";

//     return 0;
// }



//main loop for testing day 2

/*int main() {
    Document doc;
    char command;
    char inputChar;

    while (true) {
        doc.render();
        std::cout << "Commands: (i)nsert char, (e)nter key, (b)ackspace, (u)p, (d)own, (q)uit,l-left,r-right,D-deletechar\n";
        std::cout << "Enter command character: ";
        std::cin >> command;

        if (command == 'q') break;

        switch (command) {
            case 'i':
                std::cout << "Type a single character: ";
                std::cin >> inputChar;
                doc.insertChar(inputChar);
                break;
            case 'e':
                doc.handleEnter();
                break;
            case 'b':
                doc.handleBackspace();
                break;
            case 'u':
                doc.moveUp();
                break;
            case 'd':
                doc.moveDown();
                break;
            case 'l' :
                doc.moveLeft();break;
            case 'r' :
                doc.moveRight();break;
            case 'D' :
                doc.deleteChar();
            default:
                std::cout << "Unknown structural command!\n";
        }
    }
    return 0;
}*/
///main loop for day 3 testing 
#include <fstream>
#include <sstream>

class FileManager {
public:
    // Part 1: Open File (Converts lines from disk into your Document structure)
    static bool openFile(const std::string& filename, Document& doc) {
        std::ifstream file(filename);
        if (!file.is_open()) return false;

        // Wipe out whatever is currently in the document to start fresh
        // (Assuming you instantiate a clean Document object or clear lines)
        
        std::string lineText;
        bool firstLine = true;
        
        while (std::getline(file, lineText)) {
            if (!firstLine) {
                doc.handleEnter(false); // Make a new row without recording to Undo history
            }
            // Move back to the beginning of the new blank line to write
            while (doc.getCurrentColumnIndex() > 0) doc.moveLeft();
            
            // Inject characters cleanly into our gap buffer row
            for (char c : lineText) {
                doc.insertChar(c, false);
            }
            firstLine = false;
        }
        file.close();
        return true;
    }

    // Part 1: Save File (Translates GapBuffers back into clean disk text)
    static bool saveFile(const std::string& filename, const std::vector<std::string>& docLines) {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        for (size_t i = 0; i < docLines.size(); ++i) {
            file << docLines[i];
            if (i != docLines.size() - 1) {
                file << "\n"; // Maintain line structure boundary on disk
            }
        }
        file.close();
        return true;
    }

    // Part 2: Word Count Metrics Generator
    static void printWordCount(const std::vector<std::string>& docLines) {
        int totalLines = docLines.size();
        int totalWords = 0;
        int totalChars = 0;

        for (const std::string& line : docLines) {
            totalChars += line.length();
            
            std::stringstream ss(line);
            std::string word;
            while (ss >> word) {
                totalWords++;
            }
        }

        std::cout << "\n=== DOCUMENT METRICS ===\n";
        std::cout << "Lines:      " << totalLines << "\n";
        std::cout << "Words:      " << totalWords << "\n";
        std::cout << "Characters: " << totalChars << "\n";
        std::cout << "========================\n";
    }
};

class SearchEngine {
private:
    // Helper to compute the Knuth-Morris-Pratt partial match lookup table (LPS)
    static std::vector<int> computeLPSArray(const std::string& pattern) {
        int len = 0;
        std::vector<int> lps(pattern.length(), 0);
        int i = 1;

        while (i < pattern.length()) {
            if (pattern[i] == pattern[len]) {
                len++;
                lps[i] = len;
                i++;
            } else {
                if (len != 0) {
                    len = lps[len - 1]; // Fallback to matching prefix
                } else {
                    lps[i] = 0;
                    i++;
                }
            }
        }
        return lps;
    }

public:
    // Knuth-Morris-Pratt core string matcher engine traversing all rows
    static void searchPattern(const std::string& pattern, const std::vector<std::string>& docLines) {
        if (pattern.empty()) return;
        
        std::vector<int> lps = computeLPSArray(pattern);
        bool found = false;

        std::cout << "\n--- Search Results for \"" << pattern << "\" ---\n";

        // Traverse row by row
        for (int lineIndex = 0; lineIndex < docLines.size(); ++lineIndex) {
            std::string txt = docLines[lineIndex];
            int M = pattern.length();
            int N = txt.length();

            int i = 0; // index for txt[]
            int j = 0; // index for pattern[]
            
            while (i < N) {
                if (pattern[j] == txt[i]) {
                    j++;
                    i++;
                }
                if (j == M) {
                    // Match caught successfully! Match starts at column index (i - j)
                    std::cout << "Match found: line " << lineIndex << ", column " << (i - j) << "\n";
                    found = true;
                    j = lps[j - 1];
                }
                else if (i < N && pattern[j] != txt[i]) {
                    if (j != 0) {
                        j = lps[j - 1];
                    } else {
                        i++;
                    }
                }
            }
        }

        if (!found) {
            std::cout << "No matches discovered across the document layers.\n";
        }
        std::cout << "------------------------------------\n";
    }
};
//main loop for day 4
/* int main() {
    Document doc;
    char command;
    char inputChar;

    while (true) {
        doc.render();
       std::cout << "Commands: (i)nsert, (b)ackspace, (l)eft, (r)ight, (z) undo, (y) redo\n";
       std::cout << "          (o)pen file, (s)ave file, (m)etrics, (f)ind word, (q)uit\n";
        std::cout << "Action: ";
        std::cin >> command;
        std::cin.ignore();
        if (command == 'q') break;

        switch (command) {
            case 'i':
                std::cout << "Character: ";
                std::cin.get(inputChar);
                doc.insertChar(inputChar);
                break;
            case 'b':
                doc.handleBackspace();
                break;
            case 'l':
                doc.moveLeft();
                break;
            case 'u':
                doc.moveUp();
                break;
            case 'd':
                doc.moveDown();
                break;
            
            case 'e':
                doc.handleEnter();
                break;
            case 'r':
                doc.moveRight();
                break;
            case 'z':
                doc.undo();
                std::cout << "[Undone last change]\n";
                break;
            case 'y':
                doc.redo();
                std::cout << "[Redone last change]\n";
                break;
            // --- Add these options to your main switch statement deck ---
            case 'o': {
            std::string filename;
            std::cout << "Enter file name to open (e.g., notes.txt): ";
            std::cin >> filename;
            if (FileManager::openFile(filename, doc)) {
                std::cout << "File parsed into buffers successfully!\n";
            } else {
                std::cout << "Error opening target file.\n";
            }
            break;
            }
            case 's': {
            std::string filename;
            std::cout << "Enter file name to save as: ";
            std::cin >> filename;
            if (FileManager::saveFile(filename, doc.getAllLinesText())) {
                std::cout << "Document buffers saved securely to disk!\n";
            } else {
                std::cout << "Error writing block output directly to disk.\n";
            }
            break;
            }
           case 'm': {
            FileManager::printWordCount(doc.getAllLinesText());
            break;
           }
           case 'f': { // Find operation
            std::string pattern;
            std::cout << "Enter text query to search: ";
            std::cin >> pattern;
            SearchEngine::searchPattern(pattern, doc.getAllLinesText());
            break;
           }
            case 'D' :
                doc.deleteChar();
                
            default:
                std::cout << "Invalid Option\n";
        }
    }
    return 0;
} */


//main loop for day 5 
/*
int main() {
    Document doc;
    TrieAutocomplete autocomplete;
    SpellChecker spellChecker;
    WordRecommender recommender;

    // --- Training Step 1: Bootstrapping baseline dictionaries ---
    std::vector<std::string> bootstrapDict = {
        "hello", "world", "computer", "program", "programming", "progress", 
        "machine", "learning", "vision", "code", "today", "tomorrow","the", 
        "quick", "brown", "fox", "jumps", "over", "lazy", "dog",
    "software", "engineer", "develop", "developer", "development",
    "data", "structure", "algorithm", "science", "system", "editor"
    };
    for (const auto& w : bootstrapDict) {
        autocomplete.insert(w);
        spellChecker.loadWord(w);
    }

    // --- Training Step 2: Training the Context Recommender Matrix ---
    recommender.train("machine", "learning");
    recommender.train("machine", "vision");
    recommender.train("machine", "code");
    recommender.train("i", "am");
    recommender.train("am", "learning");

    char command;
    char inputChar;

    while (true) {
        doc.render();

        // Live Context Analysis Engine Interface Row
        auto [prevWord, currentWord] = doc.getContextWords();
        
        std::cout << "\n--- AI ASSISTANT PANEL ---\n";
        // 1. Spellchecking Check
        if (!currentWord.empty() && !spellChecker.check(currentWord)) {
            std::cout << "[!] Spell Status: '" << currentWord << "' looks misspelled.\n";
        } else {
            std::cout << "[✓] Spell Status: Clean\n";
        }

        // 2. Trie Autocomplete Suggestions
        std::cout << "Autocomplete Suggestions (" << currentWord << "...): ";
        auto autoSuggests = autocomplete.suggest(currentWord);
        for (const auto& s : autoSuggests) std::cout << "[" << s << "] ";
        
        // 3. Next Word Predictions
        std::cout << "\nNext Word Predictions (after '" << currentWord << "'): ";
        auto nextSuggests = recommender.recommendNextWords(currentWord);
        for (const auto& s : nextSuggests) std::cout << "-> " << s << " ";
        std::cout << "\n==========================\n";

        // Your requested exact operational dashboard layout sequence
        std::cout << "Commands: (i)nsert, (b)ackspace, (l)eft, (r)ight, (z) undo, (y) redo\n";
        std::cout << "          (o)pen file, (s)ave file, (m)etrics, (f)ind word, (q)uit (t) autocomplete\n";
        std::cout << "Action: ";
        std::cin >> command;
        std::cin.ignore();

        if (command == 'q') break;

        switch (command) {
            case 'i':
                std::cout << "Character: ";
                inputChar = std::cin.get();
                std::cin.ignore();
                doc.insertChar(inputChar);
                break;
            case 'b':
                doc.handleBackspace();
                break;
            case 'l':
                doc.moveLeft();
                break;
            case 'r':
                doc.moveRight();
                break;
            case 'z':
                doc.undo();
                break;
            case 'y':
                doc.redo();
                break;
            case 'o': {
                std::string filename;
                std::cout << "Enter file name: ";
                std::cin >> filename;
                FileManager::openFile(filename, doc);
                break;
            }
            case 's': {
                std::string filename;
                std::cout << "Enter file name: ";
                std::cin >> filename;
                FileManager::saveFile(filename, doc.getAllLinesText());
                break;
            }
            case 'm':
                FileManager::printWordCount(doc.getAllLinesText());
                break;
            case 'f': {
                std::string pattern;
                std::cout << "Search string: ";
                std::cin >> pattern;
                SearchEngine::searchPattern(pattern, doc.getAllLinesText());
                break;
            }
            case 't': { // 't' for Tab / Complete Word
                // Fetch current suggestions for the word being typed
                auto suggestions = autocomplete.suggest(currentWord);
                
                if (!suggestions.empty()) {
                    std::string topMatch = suggestions[0]; // Take the #1 best match
                    
                    // Figure out what characters are missing
                    // E.g., if topMatch is "program" and currentWord is "prog", missing is "ram"
                    if (topMatch.length() > currentWord.length()) {
                        std::string missingPart = topMatch.substr(currentWord.length());
                        
                        // Automatically inject the remaining characters into the document
                        for (char c : missingPart) {
                            doc.insertChar(c, false); // false so this macro step doesn't clutter undo history
                        }
                        std::cout << "\n[✓] Autocompleted to: " << topMatch << "\n";
                    }
                } else {
                    std::cout << "\n[!] No autocomplete matches available to apply.\n";
                }
                break;
            }
            default:
                std::cout << "Unknown command variant.\n";
        }
    }
    return 0;
} */

//loop for day 6

int main() {
    Document doc;
    TrieAutocomplete autocomplete;
    SpellChecker spellChecker;
    WordRecommender recommender;

    // Baseline dictionary bootstrap hydration injection
    std::vector<std::string> bootstrapDict = {
        "hello", "world", "computer", "program", "programming", "progress", 
        "machine", "learning", "vision", "code", "today", "tomorrow"
    };
    for (const auto& w : bootstrapDict) {
        autocomplete.insert(w);
        spellChecker.loadWord(w);
    }
    recommender.train("machine", "learning");
    recommender.train("machine", "vision");
    recommender.train("machine", "code");

    std::cout << "=== Live Interactive Editor Initialized ===\n";
    std::cout << "Controls: Type normal characters to insert text fluidly.\n";
    std::cout << "Shortcuts: Use Arrow Keys to Move | [Tab] Complete | [Ctrl+Z] Undo | [Ctrl+Y] Redo\n";
    std::cout << "           [Ctrl+S] Save | [Ctrl+F] Find | [Ctrl+Q] Quit Workspace Window\n";
    std::cout << "Press any key to jump into workspace buffer stream...\n";
    KeyboardReader::readRawKey(); // Await handshake keypress

    while (true) {
        // Clear terminal screen for smooth rendering frames
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        doc.render();

        // Language Intelligence Panel Context Calculations
        auto [prevWord, currentWord] = doc.getContextWords();
        std::cout << "\n--- TELEMETRY INTELLIGENCE CONSOLE ---\n";
        if (!currentWord.empty() && !spellChecker.check(currentWord)) {
            std::cout << "[!] Spell Check: '" << currentWord << "' looks irregular.\n";
        } else {
            std::cout << "[✓] Spell Check: Verified\n";
        }

        std::cout << "Autocomplete Options (" << currentWord << "): ";
        auto autoSuggests = autocomplete.suggest(currentWord);
        for (const auto& s : autoSuggests) std::cout << "[" << s << "] ";
        
        std::cout << "\nNext Word Matrix: ";
        auto nextSuggests = recommender.recommendNextWords(currentWord);
        for (const auto& s : nextSuggests) std::cout << "-> " << s << " ";
        std::cout << "\n======================================\n";

        // Step 6: Pipeline Execution Cycle
        EditorCommand incomingCmd = InputHandler::parseInput();

        if (incomingCmd.type == EditorCommandType::QUIT) {
            std::cout << "\nExiting workspace safely. Goodbye!\n";
            break;
        }

        // Handle structural non-document systemic macros inside loop scope
        if (incomingCmd.type == EditorCommandType::SAVE) {
            std::string filename;
            std::cout << "\nSave Document As: ";
            std::cin >> filename;
            FileManager::saveFile(filename, doc.getAllLinesText());
            continue;
        }
        if (incomingCmd.type == EditorCommandType::FIND) {
            std::string pattern;
            std::cout << "\nEnter text search query: ";
            std::cin >> pattern;
            SearchEngine::searchPattern(pattern, doc.getAllLinesText());
            std::cout << "Press any key to resume workspace loop...";
            KeyboardReader::readRawKey();
            continue;
        }
        if (incomingCmd.type == EditorCommandType::AUTOCOMPLETE) {
            auto suggestions = autocomplete.suggest(currentWord);
            if (!suggestions.empty() && suggestions[0].length() > currentWord.length()) {
                std::string missingPart = suggestions[0].substr(currentWord.length());
                for (char c : missingPart) {
                    doc.insertChar(c, false);
                }
            }
            continue;
        }

        // Route structural execution directly down to Document engine layer
        doc.executeCommand(incomingCmd);
    }
    return 0;
}