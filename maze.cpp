#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <list>
#include <chrono>
#include <random>
#include <algorithm>
#include <fstream>

using namespace std;

void catch_interrupt(int sig) {
    exit(EXIT_FAILURE);
}

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

struct Maze {
    int n;
    int m;
    MazeType t;
    vector<pair<pair<int,int>, pair<int, int>>> doors;

    void print_ascii() {
        cout << "Maze ";
        switch (t) {
            case MazeType::Rectangle:
                cout << "(Rectangle)\n";
                break;
            case MazeType::Hexagonal:
                cout << "(Hexagonal)\n";
                break;
            case MazeType::Moebius:
                cout << "(Moebius)\n";
                break;
        }
        cout << n << " x " << m << "\n\n";
        vector<bool> table((2 * n + 1) * (2 * m + 1), true);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                table[2 * n + 2 + 2 * i + 2 * j * (2 * n + 1)] = false;
            }
        }
        for (auto& elem : doors) {
            switch (t) {
                case MazeType::Moebius:
                    if ((elem.first.first == 0) && (elem.second.first == n - 1)) {
                        table[elem.first.second * 2 * (2 * n + 1) + 2 * n + 1] = false;
                        table[elem.second.second * 2 * (2 * n + 1) + 2 * n + 1 + 2 * n] = false;
                    } else if ((elem.first.first == n - 1) && (elem.second.first == 0)) {
                        table[elem.second.second * 2 * (2 * n + 1) + 2 * n + 1] = false;
                        table[elem.first.second * 2 * (2 * n + 1) + 2 * n + 1 + 2 * n] = false;
                    }
                    // NO BREAK!
                case MazeType::Rectangle:
                    if (elem.first.first == elem.second.first - 1) {
                        table[2 * n + 1 + elem.first.second * 2 * (2 * n + 1) + 2 * elem.first.first + 2] = false;
                    } else if (elem.first.first == elem.second.first + 1) {
                        table[2 * n + 1 + elem.first.second * 2 * (2 * n + 1) + 2 * elem.first.first] = false;
                    } else if (elem.first.second == elem.second.second - 1) {
                        table[(elem.first.second + 1) * 2 * (2 * n + 1) + 2 * elem.first.first + 1] = false;
                    } else if (elem.first.second == elem.second.second + 1) {
                        table[elem.first.second * 2 * (2 * n + 1) + 2 * elem.first.first + 1] = false;
                    }
                    break;
                case MazeType::Hexagonal:
                    break;
            }
        }
        for (int j = 0; j < 2 * m + 1; ++j) {
            for (int i = 0; i < 2 * n + 1; ++i) {
                if (table[i + j * (2 * n + 1)]) {
                    cout << '#';
                } else {
                    cout << ' ';
                }
            }
            cout << "\n";
        }
    }

    void print_png() {
        ofstream ofs("image.png");
        if (!ofs) {
            cerr << "Failed to write file image.png\n";
        }
    }
};

struct hashForIntPair {
    size_t operator()(const pair<int, int>& p) const {
        return p.first ^ p.second;
    }
};

void print_edges(const vector<pair<pair<int, int>, pair<int, int>>>& seq) {
    cout << "[";
    for (auto& elem : seq) {
        cout << "{(" << elem.first.first << ", " << elem.first.second << "),(" << elem.second.first << ", " << elem.second.second << ")} ";
    }
    cout << "]\n";
}

Maze generate_maze(int n, int m, MazeType t) {
    Maze maze{n, m, t};

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    unordered_set<pair<int, int>, hashForIntPair> visited_nodes;
    list<pair<pair<int, int>, pair<int, int>>> edges;

    visited_nodes.insert(make_pair(0, 0));
    switch (maze.t) {
        case MazeType::Moebius:
            edges.push_back(make_pair(make_pair(0, 0), make_pair(n - 1, m - 1)));
            // NO BREAK!
        case MazeType::Rectangle:
            edges.push_back(make_pair(make_pair(0, 0), make_pair(0, 1)));
            edges.push_back(make_pair(make_pair(0, 0), make_pair(1, 0)));
            break;
        case MazeType::Hexagonal:
            break;
    }

    while (edges.size() != 0) {
        uniform_int_distribution<int> dist(0, edges.size() - 1);
        int steps = dist(gen);
        auto it = edges.begin();
        advance(it, steps);
        pair<int, int> node = it->second;
        maze.doors.push_back(*it);
        visited_nodes.insert(node);
        edges.remove_if([node](pair<pair<int, int>, pair<int, int>> edge) {
            return (edge.second.first == node.first) && (edge.second.second == node.second);
        });
        switch (maze.t) {
            case MazeType::Moebius:
                // Twist at the end
                if (node.first == n - 1) {
                    auto neighbour = make_pair(0, m - 1 - node.second);
                    if (visited_nodes.count(neighbour) == 0) {
                        edges.push_back(make_pair(node, neighbour));
                    }
                } else if (node.first == 0) { // Twist at the beginning
                    auto neighbour = make_pair(n - 1, m - 1 - node.second);
                    if (visited_nodes.count(neighbour) == 0) {
                        edges.push_back(make_pair(node, neighbour));
                    }
                }
                // NO BREAK!
            case MazeType::Rectangle:
                // UP, DOWN, LEFT, RIGHT, VISITED_NODES
                if (node.second > 0) {
                    auto neighbour = make_pair(node.first, node.second - 1);
                    if (visited_nodes.count(neighbour) == 0) {
                        edges.push_back(make_pair(node, neighbour));
                    }
                }
                // DOWN
                if (node.second < m - 1) {
                    auto neighbour = make_pair(node.first, node.second + 1);
                    if (visited_nodes.count(neighbour) == 0) {
                        edges.push_back(make_pair(node, neighbour));
                    }
                }
                // LEFT
                if (node.first > 0) {
                    auto neighbour = make_pair(node.first - 1, node.second);
                    if (visited_nodes.count(neighbour) == 0) {
                        edges.push_back(make_pair(node, neighbour));
                    }
                }
                // RIGHT
                if (node.first < n - 1) {
                    auto neighbour = make_pair(node.first + 1, node.second);
                    if (visited_nodes.count(neighbour) == 0) {
                        edges.push_back(make_pair(node, neighbour));
                    }
                }
                break;
            case MazeType::Hexagonal:
                break;
        }
    }

    if (maze.doors.size() == 0) cout << "No doors yet!\n";
    return maze;
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
            case 'h':
                print_usage_and_exit();
                break;
            case 'n':
                n = atoi(optarg);
                if (errno) {
                    cerr << "Error parsing argument : " << optarg << "\n";
                    print_usage_and_exit();
                }
                if (n < 2 || n > 500) {
                    cerr << "Dimension is out of range (2..500) : " << optarg << "\n";
                    print_usage_and_exit();
                    break;
                }
                continue;
            case 'm':
                m = atoi(optarg);
                if (errno) {
                    cerr << "Error parsing argument : " << optarg << "\n";
                    print_usage_and_exit();
                }
                if (m < 2 || m > 500) {
                    cerr << "Dimension is out of range (2..500) : " << optarg << "\n";
                    print_usage_and_exit();
                    break;
                }
                continue;
            case 't':
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
        break;
    }

    if (optind != argc) {
        cerr << "Unknown argument(s)\n";
        print_usage_and_exit();
        return -1;
    }

    if (t == MazeType::Hexagonal) {
        cout << "Sorry, Hexagonal mazes are not supported yet.\n";
        return 0;
    }

    signal(SIGINT, catch_interrupt);

    Maze maze = generate_maze(n, m, t);
    // print_edges(maze.doors);
    maze.print_ascii();
    maze.print_png();
    return 0;
}