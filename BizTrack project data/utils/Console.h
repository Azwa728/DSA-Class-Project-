#pragma once
// ---------------------------------------------------------------
// Console helpers + the "boxed screen".
//
// HOW THE BOX WORKS
//   At program start a Screen object takes over std::cout. Everything the
//   program prints is collected in a text buffer instead of going straight
//   to the terminal. Whenever the program waits for input (readLine), the
//   Screen clears the terminal and draws the buffered text inside a box in
//   the centre of the window, with the cursor placed right after the prompt.
//   A new "screen" starts after every menu choice, "Press Enter" and page
//   break (the buffer is emptied, so the terminal refreshes).
//   No other file needs to know about any of this.
// ---------------------------------------------------------------
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include "structures/DynamicArray.h"

#ifdef _WIN32
  #ifndef NOMINMAX
  #define NOMINMAX
  #endif
  #include <windows.h>
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

class Screen : public std::streambuf {
    std::streambuf* original;     // the real terminal output
    std::string buf;              // text of the current screen
    bool fresh;                   // true -> the next character printed starts a new screen

    static const int MIN_INNER = 56;   // minimum text width inside the box

    static void terminalSize(int& cols, int& rows) {
        cols = 80; rows = 24;
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
            cols = info.srWindow.Right - info.srWindow.Left + 1;
            rows = info.srWindow.Bottom - info.srWindow.Top + 1;
        }
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0) { cols = w.ws_col; rows = w.ws_row; }
#endif
    }

    static bool isRule(const std::string& s) {          // a line made only of '=' (menu title rules)
        if (s.size() < 10) return false;
        for (size_t i = 0; i < s.size(); i++) if (s[i] != '=') return false;
        return true;
    }
    static std::string trim(const std::string& s) {
        size_t a = s.find_first_not_of(' ');
        if (a == std::string::npos) return "";
        return s.substr(a, s.find_last_not_of(' ') - a + 1);
    }
    static std::string centred(const std::string& s, int width) {
        int pad = (width - (int)s.size()) / 2;
        return std::string(pad > 0 ? pad : 0, ' ') + s;
    }

    void put(char c) {
        if (fresh) { buf.clear(); fresh = false; }
        buf.push_back(c);
    }

protected:
    int overflow(int c) override { if (c != EOF) put((char)c); return c; }
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        for (std::streamsize i = 0; i < n; i++) put(s[i]);
        return n;
    }

public:
    Screen() : original(nullptr), fresh(false) {
#ifdef _WIN32
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);       // allow ANSI escape codes
        DWORD mode = 0;
        if (GetConsoleMode(h, &mode)) SetConsoleMode(h, mode | 0x0004);
#endif
        original = std::cout.rdbuf(this);
    }
    ~Screen() override {
        render(false);                                    // show whatever was printed last (e.g. "Goodbye!")
        std::cout.rdbuf(original);
    }

    void markFresh() { fresh = true; }                    // next output starts a clean screen
    void record(const std::string& typed) { buf += typed; buf += '\n'; }   // echo what the user typed

    // Clears the terminal and draws the buffer inside a centred box.
    // withPrompt: the last line of the buffer is a prompt; leave the cursor after it.
    void render(bool withPrompt) {
        DynamicArray<std::string> raw, lines;
        std::string cur;
        for (size_t i = 0; i < buf.size(); i++) {
            if (buf[i] == '\n') { raw.push_back(cur); cur.clear(); }
            else if (buf[i] != '\r') cur.push_back(buf[i]);
        }
        if (withPrompt || !cur.empty()) raw.push_back(cur);
        for (int k = 0; k < raw.size(); k++) {            // drop leading blank lines, collapse blank runs
            bool blank = trim(raw[k]).empty();
            if (blank && (lines.size() == 0 || trim(lines[lines.size() - 1]).empty())) continue;
            lines.push_back(raw[k]);
        }
        if (lines.size() == 0) return;

        int n = lines.size();
        int inner = MIN_INNER;
        for (int i = 0; i < n; i++) if ((int)lines[i].size() > inner) inner = (int)lines[i].size();

        // Titles between two '=' rules are centred in the box; the rules fill the box width.
        int i = 0;
        while (i < n) {
            if (!isRule(lines[i])) { i++; continue; }
            int j = i + 1;
            while (j < n && !isRule(lines[j])) j++;
            if (j < n && j - i - 1 >= 1 && j - i - 1 <= 3) {
                bool ok = true;
                for (int k = i + 1; k < j; k++) if (trim(lines[k]).empty()) ok = false;
                if (ok) for (int k = i + 1; k < j; k++) lines[k] = centred(trim(lines[k]), inner);
            }
            i = (j < n) ? j + 1 : n;
        }
        for (int k = 0; k < n; k++) if (isRule(lines[k])) lines[k] = std::string(inner, '=');

        int cols, rows;
        terminalSize(cols, rows);
        int pads = (rows > n + 4) ? 1 : 0;                           // blank padding row above/below the text
        int boxW = inner + 4, boxH = n + 2 + 2 * pads;               // text + padding + 2 borders
        int left = cols > boxW ? (cols - boxW) / 2 : 0;
        int top  = rows > boxH ? (rows - boxH) / 2 : 0;

        std::string margin(left, ' ');
        std::string border = margin + "+" + std::string(inner + 2, '-') + "+\n";
        std::string blank  = margin + "| " + std::string(inner, ' ') + " |\n";

        std::string out = "\033[H\033[2J";                           // cursor home + clear screen
        out += std::string(top, '\n');
        out += border;
        if (pads) out += blank;
        for (int k = 0; k < n; k++)
            out += margin + "| " + lines[k] + std::string(inner - (int)lines[k].size(), ' ') + " |\n";
        if (pads) out += blank;
        out += border;
        if (withPrompt) {                                            // put the cursor right after the prompt
            int col = left + 3 + (int)lines[n - 1].size();
            out += "\033[" + std::to_string(2 + pads) + "A\033[" + std::to_string(col) + "G";
        }
        std::ostream real(original);
        real << out << std::flush;
    }
};

inline Screen g_screen;     // created before main() runs, takes over std::cout

namespace Console {

inline const int PAGE_SIZE = 12;      // rows per page when listing records

inline void stripCR(std::string& s) {            // handles files saved with Windows line endings
    if (!s.empty() && s.back() == '\r') s.pop_back();
}

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    g_screen.render(true);                        // draw the box, cursor after the prompt
    std::string s;
    if (!std::getline(std::cin, s)) { std::cout << "\n"; std::exit(0); }   // end of input
    stripCR(s);
    g_screen.record(s);                           // keep what the user typed on the screen
    return s;
}

inline int readInt(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        try {
            size_t pos = 0;
            int v = std::stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        std::cout << "  Please enter a valid whole number.\n";
    }
}

// A menu choice: after it is read, the next output starts a fresh (cleared) screen.
inline int readChoice(const std::string& prompt = "\n  Enter choice: ") {
    int v = readInt(prompt);
    g_screen.markFresh();
    return v;
}

inline int readIntInRange(const std::string& prompt, int lo, int hi) {
    while (true) {
        int v = readInt(prompt);
        if (v >= lo && v <= hi) return v;
        std::cout << "  Please enter a number between " << lo << " and " << hi << ".\n";
    }
}

// Non-empty text; '|' is replaced because it is our file separator.
inline std::string readText(const std::string& prompt) {
    while (true) {
        std::string s = readLine(prompt);
        for (size_t i = 0; i < s.size(); i++) if (s[i] == '|') s[i] = '-';
        if (!s.empty()) return s;
        std::cout << "  This field cannot be empty.\n";
    }
}

inline std::string toUpper(std::string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = (char)std::toupper((unsigned char)s[i]);
    return s;
}

inline void pause() {
    readLine("\nPress Enter to continue...");
    g_screen.markFresh();                         // go back to a clean screen
}

// Used when listing many rows: ask to continue after each page. Returns false if user stops.
inline bool continuePaging() {
    std::string s = readLine("  -- Enter for more, q to stop -- ");
    g_screen.markFresh();                         // next page replaces this one
    return !(s == "q" || s == "Q");
}

inline void header(const std::string& title) {
    std::cout << "\n========================================\n";
    int pad = (40 - (int)title.size()) / 2;
    if (pad < 0) pad = 0;
    std::cout << std::string(pad, ' ') << title << "\n";
    std::cout << "========================================\n";
}

inline void line() { std::cout << "----------------------------------------\n"; }

inline std::string money(long long v) {          // 1234567 -> "Rs. 1,234,567"
    std::string s = std::to_string(v), out;
    int n = (int)s.size();
    for (int i = 0; i < n; i++) {
        out += s[i];
        int remaining = n - 1 - i;
        if (remaining > 0 && remaining % 3 == 0) out += ',';
    }
    return "Rs. " + out;
}

}  // namespace Console
