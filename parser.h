#pragma once

#include <iostream>
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

vector<vector<double>>
go(istream& in) {
    vector<vector<double>> out;
    int line_counter = 0;
    int line_error_counter = 0;
    while (in) {
        string line;
        getline(in, line);
        if (!in && in.eof()) {
            break;
        }
        line_counter++;
        try{
            out.push_back(parse_line(line));
        }catch(...){
            line_error_counter ++;
            //пропуск ошибочной строки
        }
    }
    cerr << "Lines processed: " << line_counter
         <<"\nError lines: " << line_error_counter << "\n";
    return out;
}
