#include "parser.h"

#include <iomanip>
#include <fstream>

void
gnvoerk(const vector<vector<double>>& xss, ostream& out) {
    out << setprecision(12);
    for (auto& xs : xss) {
        for (auto x : xs) {
            out << x << " ";
        }
        out << "\n";
    }
}

int
main(int argc, char* argv[]) {
    if(argc != 3){
        cerr << "Usage: " << argv[0] << " in.txt out.txt\n";
        return 1;
    }

    ifstream in{argv[1]};
    if (!in.is_open()) {
        cerr << "Unable to read input file.\n";
        return 1;
    }

    ofstream out{argv[2]};
    if (!out.is_open()) {
        cerr << "Unable to write output file.\n";
        return 1;
    }

    auto xss = go(in);
    gnvoerk(xss, out);
    return 0;
}
