#pragma once

#include <string>
#include <vector>
#include <ios>

namespace cars
{

struct record
{
    int year = 0;
    std::string vendor;
    std::string brand;
    double length = 0.;

    auto brief(std::ostream&) const -> std::ostream&;
};

auto from_stream(std::istream&) -> std::vector<record>;

}