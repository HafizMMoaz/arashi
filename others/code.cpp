#include <iostream>
#include <windows.h>

void detectKeyPress() {
    // Loop to keep checking for key presses
    while (true) {
        for (char c = 'A'; c <= 'Z'; ++c) {
            // Get the virtual key code for the current character (both lowercase and uppercase)
            SHORT state = GetAsyncKeyState(c); // 'A' to 'Z' maps to keycodes 0x41 to 0x5A (65 to 90)

            // Check if the key is pressed
            if (state & 0x8000) { // 0x8000 means the high-order bit is set, indicating key is pressed
                std::cout << "Key " << c << " is pressed!" << std::endl;
                // Optional: Add a short sleep to prevent spamming of output
                Sleep(200); // Sleep for 200ms to reduce console spamming
            }
        }
    }
}

int main() {
    std::cout << "Press any alphabetic key to see the output..." << std::endl;
    detectKeyPress();
    return 0;
}