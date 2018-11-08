#include "parser.h"

#include <iomanip>

int
main() {
    auto xss = go(cin);
    cout << setprecision(12);
    for (auto& xs : xss) {
        for (auto x : xs) {
            cout << x << " ";
        }
        cout << "\n";
    }
    return 0;
}
