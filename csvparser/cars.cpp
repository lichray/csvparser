#include "cars.h"
#include "csvparser.h"

#include <ostream>
#include <iomanip>

namespace cars
{

auto from_stream(std::istream& in) -> std::vector<record>
{
    std::vector<record> v;
    for (record r; in;)
    {
        csvparser::getrecord(in, r.year, r.vendor, r.brand, r.length);
        v.push_back(std::move(r));
    }

    return v;
}

auto record::brief(std::ostream& out) const -> std::ostream&
{
    out.width(5);
    return out << std::left << year << vendor;
}

}
