#pragma once

#include <iostream>
#include <optional>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;

double
parse_slave(const json& j);

using namespace std;

double
parse_slave(const json& j) {
    auto s = j["rt"]["B"].get<string>();
    return s[s.size() - 1] - '0';
}

vector<double>
parse_line(string in) {
    auto j = json::parse(in);

    int rtcount = j.count("rt");
    if (rtcount) {
        int a_eiudcount = j["rt"].count("A_euid");
        if (a_eiudcount) {
            double flag = 3;
            double ts = j["ts"].get<double>();
            double s = j["rt"]["s"].get<double>();
            return {flag, ts, s};
        }
        else {
            double flag = 1;
            double ts = j["ts"].get<double>();
            double s = j["rt"]["s"].get<double>();
            double slave = parse_slave(j);
            double ddist = j["rt"]["ddist_mm"].get<double>();
            double baro = j["B_baro_cm"].get<double>();
            return {flag, ts, s, slave, ddist, baro};
        }
    }
    else {
        double flag = 2;
        double ts = j["ts"].get<double>();
        auto a = j["a"].get<vector<double>>();
        auto g = j["g"].get<vector<double>>();
        auto q = j["q"].get<vector<double>>();
        auto m = j["m"].get<vector<double>>();
        double t = j["t"].get<double>();
        double p = j["p"].get<double>();
        return {flag, ts, a[0], a[1], a[2], g[0], g[1], g[2], q[0], q[1], q[2], q[3],
                m[0], m[1], m[2], t, p};
    }
}

class ColumnExtractor {
public:
    ColumnExtractor() : _master_tick{-1}, _column(6), _has_data{false} {}

    optional<vector<double>>
    take_line(const vector<double>& xs) {
        if (xs[0] == 1){
            if (xs[2] != _master_tick){
                return {};
            }
            int index = static_cast<size_t>(xs[3]) - 1;
            _column[index] = xs[4];
            _has_data = true;
            return {};
        }

        if (xs[0] == 3){
            if (_master_tick < 0){
                _master_tick = xs[2];
                return {};
            }

            const auto column = current_column();
            _master_tick = xs[2];
            _has_data = false;
            return column;
        }
        return {};
    }

    optional<vector<double>>
    current_column()const{
        if (_has_data) {
            return _column;
        }
        return {};
    }

private:
    int _master_tick;
    vector<double> _column;
    bool _has_data;
};

struct OfflineResult {
    vector<vector<double>> source;
    vector<vector<double>> matrix;  // column-major (array of columns)
};

OfflineResult
go(istream& in) {
    ColumnExtractor ce;

    OfflineResult out;

    int line_counter = 0;
    int line_error_counter = 0;
    while (in) {
        string line;
        getline(in, line);
        if (!in && in.eof()) {
            break;
        }
        line_counter++;

        try {
            const auto xs = parse_line(line);

            out.source.push_back(xs);

            const auto column = ce.take_line(xs);
            if (column) {
                out.matrix.push_back(*column);
            }
        } catch(...){
            line_error_counter ++;
            //пропуск ошибочной строки
        }
    }

    if (auto column = ce.current_column()) {
        out.matrix.push_back(*column);
    }

    if (out.matrix.size() == 1){
        out.matrix.push_back(out.matrix[0]);
    }

    cerr << "Lines processed: " << line_counter
         <<"\nError lines: " << line_error_counter << "\n";
    return out;
}
