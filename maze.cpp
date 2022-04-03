#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>

#include <iostream>
#include <unordered_map>
#include <fstream>

using namespace std;

[[noreturn]] void print_usage_and_exit() {
    cerr << "Usage: maze [OPTION]...\n";
    cerr << "  -h        Prints this help message.\n";
    cerr << "  -n SIZE   First dimension of the maze. (int32)\n";
    cerr << "            Default: 8.\n";
    cerr << "  -m SIZE   Second dimension of the maze. (int32)\n";
    cerr << "            Default: Option n.\n";
    cerr << "  -t MTYPE  Type of the maze. [rectangle/rect, hexagonal/hex, moebius/moeb]\n";
    cerr << "            Default: rectangle.\n\n";
    cerr << "Example: \n";
    cerr << "   maze -n 5 -m 10 -t rect\n";
    exit(EXIT_FAILURE);
}

enum MazeType {
    Rectangle,
    Hexagonal,
    Moebius
};

int generate_maze(int n, int m, MazeType t) {
    cout << "Gen Maze:\n";
    cout << "  n : " << n << "\n";
    cout << "  m : " << m << "\n";
    cout << "  t : " << t << "\n";
    return 0;
}

int main(int argc, char* argv[]) {
    int n = 8, m = 8;
    MazeType t = MazeType::Rectangle;

    while (true) {
        int opt = getopt(argc, argv, "hn:m:t:");
        switch(opt) {
            case -1:
                break;
            case '?':
            default :
            case 'h': {
                print_usage_and_exit();
                break;
            }
            case 'n': {
                n = atoi(optarg);
                if (errno) {
                    cerr << "Error parsing argument : " << optarg << "\n";
                    print_usage_and_exit();
                }
                continue;
            }
            case 'm': {
                m = atoi(optarg);
                if (errno) {
                    cerr << "Error parsing argument : " << optarg << "\n";
                    print_usage_and_exit();
                }
                continue;
            }
            case 't': {
                if ((strncmp(optarg, "rectangle", sizeof("rectangle")) == 0) || (strncmp(optarg, "rect", sizeof("rect")) == 0)) {
                    t = MazeType::Rectangle;
                    continue;
                }
                if ((strncmp(optarg, "hexagonal", sizeof("hexagonal")) == 0) || (strncmp(optarg, "hex", sizeof("hex")) == 0)) {
                    t = MazeType::Hexagonal;
                    continue;
                }
                if ((strncmp(optarg, "moebius", sizeof("moebius")) == 0) || (strncmp(optarg, "moeb", sizeof("moeb")) == 0)) {
                    t = MazeType::Moebius;
                    continue;
                }
                cerr << "Unknown maze type : " << optarg << "\n";
                print_usage_and_exit();
                break;
            }
        }
        break;
    }

    if (optind != argc) {
        cerr << "Unknown argument(s)\n";
        print_usage_and_exit();
        return -1;
    }

    return generate_maze(n, m, t);
    /*
    ofstream ofs("image.png");
    if (!ofs) {
        cerr << "Failed to write file image.png\n";
        return 1;
    }
    print_usage_and_exit();
    return 0;
    */
}