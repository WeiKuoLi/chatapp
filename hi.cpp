#include <iostream>


char _getch() {
    char input;
    std::cin >> std::noskipws >> input;
    return input;
}



int main() {
    std::cout << "Enter your text. Press Shift+Enter to continue next line and Enter alone to send:\n";

    std::string inputText;
    char c;
    while (1) {
        c = _getch(); // Read a single character without Enter key processing

        if ((int)c == 13) { // Check if Enter key is pressed
            if (_getch() == -32) { // Check if the next character is the result of a special key (like arrow key or function key)
                c = _getch(); // Read the special key
                if ((int)c == 13) { // Check if the special key represents Enter (13 for Enter key)
                    // Shift+Enter was pressed, continue to the next line
                    inputText += '\n';
                }
            } else {
                // Enter alone was pressed, send the input
                break;
            }
        } else {
            inputText += c;
        }
    }

    std::cout << "Input text:\n" << inputText << std::endl;

    return 0;
}

