#pragma once

#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

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
