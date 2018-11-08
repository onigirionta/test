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
    if(argc != 4){
        cerr << "Usage: " << argv[0] << " in.txt out.txt matrix.txt\n";
        return 1;
    }

    ifstream in{argv[1]};
    if (!in.is_open()) {
        cerr << "Unable to read input file.\n";
        return 1;
    }

    ofstream out_source{argv[2]};
    if (!out_source.is_open()) {
        cerr << "Unable to write output file.\n";
        return 1;
    }

    ofstream out_matrix{argv[3]};
    if (!out_matrix.is_open()) {
        cerr << "Unable to write matrix output file.\n";
        return 1;
    }

    auto result = go(in);
    gnvoerk(result.source, out_source);
    gnvoerk(result.matrix, out_matrix);
    return 0;
}
