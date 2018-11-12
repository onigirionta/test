#include "file.h"
#include "commport.h"
#include "parser.h"
#include "output.h"

enum class Input {
    File,
    COM
};

struct Options {
    Input input_type;
    std::string input_path;
    std::string source_path;
    std::string matrix_path;
};

// TODO: use <getopt> or boost::program_options
std::optional<Options>
parse_options(int argc, const char* argv[]) {
    if (argc != 5) {
        return {};
    }

    Input type;
    if (argv[1] == std::string{"-f"}) {
        type = Input::File;
    } else if (argv[1] == std::string{"-c"}) {
        type = Input::COM;
    } else {
        return {};
    }

    Options options;
    options.input_type = type;
    options.input_path = argv[2];
    options.source_path = argv[3];
    options.matrix_path = argv[4];
    return options;
}

int
main(int argc, const char* argv[]) {
    const auto options = parse_options(argc, argv);
    if (!options) {
        cerr << "Usage:\n" <<
             "\t" << argv[0] << " -f in.txt out.txt matrix.txt\n" <<
             "\t\tto read from file\n"
             "\t" << argv[0] << " -c COM7 out.txt matrix.txt\n" <<
             "\t\tto read from COM port\n";
        return 1;
    }

    ofstream out_source{options->source_path};
    if (!out_source.is_open()) {
        cerr << "Unable to write output file.\n";
        return 1;
    }

    ofstream out_matrix{options->matrix_path};
    if (!out_matrix.is_open()) {
        cerr << "Unable to write matrix output file.\n";
        return 1;
    }

    try {
        Result result;
        switch (options->input_type) {
        case Input::File: {
            File file{options->input_path};
            result = go(file);
            break;
        }
        case Input::COM: {
            CommPort port{options->input_path};
            result = go(port);
            break;
        }
        }

        gnvoerk(result.source, out_source);
        gnvoerk(result.matrix, out_matrix);
        return 0;
    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
