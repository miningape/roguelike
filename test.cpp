#include <iostream>
#include <set>
#include <vector>

using namespace std;

void print(set<string> s) {
    for (auto e : s) {
        cout << e;
    }
    cout << endl;
}

int main() {
    set<string> x = { "1", "24" };
    set<string> y = { "1", "2", "3" };

    cout << "x: ";
    print(x);
    cout << "y: ";
    print(y);

    cout << "int: ";
    set<string> intersect;
    set_difference(x.begin(), x.end(), y.begin(), y.end(), inserter(intersect, begin(intersect)));
    print(intersect);

    // vector<pair<int, int>> v = {
    //     {1, 1}, {1, 2}, {3, 4}
    // };

    // auto i = find(v.begin(), v.end(), pair<int, int>({-1, 2}));
    // if (i == v.end()) {
    //     cout << "not found" << endl;
    // } else {
    //     cout << "found" << endl;
    // }

    return 0;
}