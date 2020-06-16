#include "cars.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>

static void process_report(std::vector<cars::record> v)
{
    using std::cout;

    auto by_year = [](auto& x, auto& y) { return x.year < y.year; };
    auto add_length = [](double x, auto& y) { return x + y.length; };
    auto print_all = [](auto first, auto last) {
        std::for_each(first, last, [](auto& x) { x.brief(cout) << '\n'; });
    };

    std::sort(begin(v), end(v), by_year);

    cout << "oldest:\n";
    print_all(cbegin(v),
              std::upper_bound(cbegin(v), cend(v), v.front(), by_year));

    cout << '\n';

    cout << "newest:\n";
    print_all(std::lower_bound(cbegin(v), cend(v), v.back(), by_year),
              cend(v));

    cout << '\n';

    cout << "average length:\n";
    cout.precision(3);
    cout << std::accumulate(cbegin(v), cend(v), 0., add_length) / size(v)
         << '\n';
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: " << argv[0] << " <input.csv>\n";
        return 2;
    }

    try
    {
        std::fstream fin(argv[1]);
        process_report(cars::from_stream(fin));
    }
    catch (std::exception& exc)
    {
        std::cerr << "error:" << exc.what() << '\n';
        return 1;
    }
}
