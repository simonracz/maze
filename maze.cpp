#include <cstdlib>
#include <cerrno>

#include <iostream>
#include <unordered_map>
#include <fstream>

using namespace std;

[[noreturn]] void print_usage_and_exit() {
    cerr << "Usage: maze [OPTION]...\n";
    cerr << "  -n  First dimension of the maze. (int32) \n";
    cerr << "      Default: 8.\n";
    cerr << "  -m  Second dimension of the maze. (int32)\n";
    cerr << "      Default: Option n.\n";
    cerr << "  -t  Type of the maze. [rectangle/rect, hexagonal/hex, moebius/mob]\n";
    cerr << "      Default: rectangle.\n\n";
    cerr << "Example: maze -n 5 -m 10 -t rect\n";
    exit(EXIT_FAILURE);
}

enum MazeType {
    Rectangle,
    Hexagonal,
    Moebius
};

int main(int argc, char* argv[]) {
    int n = 8, m = 8;
    MazeType maze_type = MazeType::Rectangle;

    for (int i = 1; i < argc; ++i) {
    }

    ofstream ofs("image.png");
    if (!ofs) {
        cerr << "Failed to write file image.png\n";
        return 1;
    }
    print_usage_and_exit();
    return 0;
}