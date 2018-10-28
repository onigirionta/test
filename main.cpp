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

double tic = 0;
//разница между тиком на который отзывается slave и текущим тиком master'а
double dtic = 0;
//счетчик количества запаздывающих сигналов
int delay = 0;
//максимальная разница меж тиками
int maxdtic = 0;
//минимальная задержка меж тиками
int  mindtic = 0;

vector<double>
go_line(string in) {
    auto j = json::parse(in);

    int rtcount = j.count("rt");
    if (rtcount) {
        int a_eiudcount = j["rt"].count("A_euid");
        if (a_eiudcount) {
            double flag = 3;
            double ts = j["ts"].get<double>();
            double s = j["rt"]["s"].get<double>();
            tic = s;
            return {flag, ts, s};
        }
        else {
            double flag = 1;
            double ts = j["ts"].get<double>();
            double s = j["rt"]["s"].get<double>();
            if (s != tic){
                dtic = tic - s;
                if ((dtic > 2) || (dtic < 0)){
                    cerr << "Tic of master: " <<tic << " Tic of slave: " << s <<
                    "Difference of tics: " << dtic <<"\n";
                }
//                if (tic == 0){
//                    dtic = dtic + 256;
//                }

                delay ++;
                if (maxdtic < dtic){
                    maxdtic = dtic;
                }
            }

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
            out.push_back(go_line(line));
        }catch(...){
            line_error_counter ++;
            //пропуск ошибочной строки
        }
    }
    cerr << "Lines processed: " << line_counter
        <<"\nError lines: " << line_error_counter << "\n";
    return out;
}

void
test_go() {
    auto v1 =
            go_line("{\"ts\":3400,\"rt\":{\"s\":133,\"A\":\"0x100\",\"B\":\"0x204\",\"ddist_mm\":4254,\"B_euid\":\"0x10205fe110001dd7\"},\"B_baro_cm\":24455}");
    assert ((v1 == std::vector<double>{
            1,
            3400,
            133,
            4,
            4254,
            24455}));

    auto v2 = go_line("{\"ts\":3404,\"a\":[1997,-504,89],\"g\":[-17,-3,4],"
                      "\"q\":[255593053,-754998296,-79182262,-715053453],"
                      "\"m\":[-4,269,-368],\"t\":0,\"p\":98428}");
    assert((v2 == std::vector<double>{
            2,
            3404,
            1997, -504, 89,
            -17, -3, 4,
            255593053, -754998296, -79182262, -715053453,
            -4, 269, -368,
            0,
            98428}));

    auto v3 =
            go_line("{\"ts\":3437,\"rt\":{\"s\":135,\"A\":\"0x100\",\"A_euid\":\"0x10205fe110001d4e\"},\"A_baro_cm\":24740}");
    assert ((v3 == std::vector<double>{
            3,
            3437,
            135}));

    try {
        auto v4 = go_line("");
        assert(!"exception expected");
    }
    catch (...) {}

    istringstream in5{
            "{\"ts\":3400,\"rt\":{\"s\":133,\"A\":\"0x100\",\"B\":\"0x204\","
            "\"ddist_mm\":4254,\"B_euid\":\"0x10205fe110001dd7\"},\"B_baro_cm\":24455}\n"

            "{\"ts\":3404,\"a\":[1997,-504,89],\"g\":[-17,-3,4],"
            "\"q\":[255593053,-754998296,-79182262,-715053453],"
            "\"m\":[-4,269,-368],\"t\":0,\"p\":98428}\n"

            "{\"ts\":3437,\"rt\":{\"s\":135,\"A\":\"0x100\","
            "\"A_euid\":\"0x10205fe110001d4e\"},\"A_baro_cm\":24740}"
    };
    auto v5 = go(in5);
    assert((v5 == vector<vector<double>>{
            {
                    1,
                    3400,
                    133,
                    4,
                    4254,
                    24455},
            {
                    2,
                    3404,
                    1997, -504, 89,
                    -17, -3, 4,
                    255593053, -754998296, -79182262, -715053453,
                    -4, 269, -368,
                    0,
                    98428},
            {
                    3,
                    3437,
                    135}
    }));
}

int
main() {

    test_go();

    go(cin);

//    go_line(cin);
    cerr << "Delay count: " << delay << "\n";
    cerr << "Max delay: " << maxdtic << "\n";
    return 0;
}
