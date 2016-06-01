#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "search.hpp"
#include "search-driver.hpp"
#include "output.hpp"
#include "utils.hpp"

#include "rlutil/rlutil.h"

using namespace search;

void
outputHelp()
{
    static const char HELP_TEXT[] =
       "Search, a simple search engine written\n"
       "  by Clarkok Zhang (mail@clarkok.com)\n"
       "\n"
       "Version:\n"
       "  " VERSION "\n"
       "\n"
       "Usage:\n"
       "  search <command> [options]\n"
       "\n"
       "Command:\n"
       "  add <files...>            add files to the index\n"
       "  help                      print this help\n"
       "  query [options] <query>   commit a query\n"
       "    -o <file>               output result to a given file\n"
       "    -f <format>             output format (default = color)\n"
       "      txt                   plain text\n"
       "      color                 colored detail\n"
       "    -l <limit>              output limit, 0 to output all results (default = 5)\n"
       "  reset                     reset the index database, remove everything\n"
       "\n"
       "Note:\n"
       "  default index directory is under ~/.search, change it by setting environment\n"
       "  variable " ENV_VAR_NAME "\n"
       "\n"
    ;

    std::cout << HELP_TEXT << std::endl;
}

std::string
getDatabasePath()
{
    std::string ret;
    auto env = std::getenv(ENV_VAR_NAME);
    if (env) {
        return env;
    }

    auto home = std::getenv("HOME");
    if (home) {
        return ret + home + "/" + DEFAULT_DB_DIR;
    }

    ret = getpwuid(getuid())->pw_dir;
    return ret + "/" + DEFAULT_DB_DIR;
}

void
checkDatabaseDir(std::string path)
{
    struct stat st = {0};

    if (stat(path.c_str(), &st) == -1) {
        mkdir(path.c_str(), 0755);
    }
}

std::string
resolveAbsolutePath(const char *path)
{
    char buffer[PATH_MAX];
    if (!realpath(path, buffer)) {
        std::cerr << "Unable to resolve " << path << std::endl;
        exit(-1);
    }
    return buffer;
}

std::string
outputProgressBar(float value)
{
    std::string ret = "  " + std::to_string(static_cast<int>(value * 100)) + "%";
    while (ret.length() < 8) {
        ret.push_back(' ');
    }

    int filled = value * 70;
    for (int i = 0; i < filled; ++i) {
        ret.push_back('*');
    }
    for (;filled < 70; ++filled) {
        ret.push_back('-');
    }

    return ret;
}

int
main(int argc, char **argv)
{
    --argc; ++argv;

    if (!argc) {
        std::cerr << "No command error!\n" << std::endl;

        outputHelp();
        return -1;
    }

    std::string command(*argv++);   --argc;

    if (command == "help") {
        outputHelp();
        return 0;
    }

    if (command == "reset") {
        std::string cmd = "rm -rf " + getDatabasePath();
        auto retval = std::system(cmd.c_str());
        (void)retval;
        return 0;
    }

    std::string db_path = getDatabasePath();
    checkDatabaseDir(db_path);
    SearchDriver driver(db_path);

    if (command == "dump") {
        driver.dumpIndex(std::cout);
        return 0;
    }

    if (command == "add") {
        int total = argc;
        auto progress = outputProgressBar(0);
        // rlutil::setString(progress.c_str());
        std::cout << progress << std::endl;
        while (argc--) {
            if ((total - argc) % 128 == 0) {
                progress = outputProgressBar((total - argc) * 1.0 / total);
                // rlutil::setString(progress.c_str());
                std::cout << progress << std::endl;
            }
            std::string path(resolveAbsolutePath(*argv++));
            driver.addDocument(Utils::readFile(path), path);
        }
        std::cout << outputProgressBar(1) << std::endl;
        return 0;
    }

    if (command == "query") {
        std::string output;
        std::string format = "color";
        size_t limit = 5;
        std::string query;

        while (argc && **argv == '-') {
            --argc;
            if ((*argv)[1] == 'o') {
                --argc; ++argv;
                output = *argv++;
            }
            else if ((*argv)[1] == 'f') {
                --argc; ++argv;
                format = *argv++;

                if (
                    format != "txt" &&
                    format != "color"
                ) {
                    std::cerr << "Unknown output format: " << format << std::endl << std::endl;
                    outputHelp();
                    return -1;
                }
            }
            else if ((*argv)[1] == 'l') {
                --argc; ++argv;
                limit = std::atoi(*argv++);
            }
            else if ((*argv)[1] == 'd') {
                ++argv;
                is_debug = true;
            }
            else {
                std::cerr << "Unknown query options: " << *argv << std::endl << std::endl;
                outputHelp();
                return -1;
            }
        }

        while (argc--) {
            query += (*argv++);
            query += ' ';
        }

        if (!query.length()) {
            std::cerr << "No query provided" << std::endl << std::endl;
            outputHelp();
            return -1;
        }

        auto token_list = driver.queryTokenList(query);
        rlutil::saveDefaultColor();
        rlutil::setColor(rlutil::LIGHTGREEN);
        std::cout << "Querying:";
        for (auto &token : token_list)  {
            std::cout << " " << token;
        }
        std::cout << std::endl;

        auto result = driver.query(token_list);
        std::cout << "Found " << result.size() << " result" << (result.size() > 1 ? "s" : "") << std::endl;
        rlutil::resetColor();

        if (format == "txt") {
            if (output.length()) {
                std::ofstream fout(output);
                getTxtOutputer(fout, limit)(result);
            }
            else {
                getTxtOutputer(std::cout, limit)(result);
            }
        }
        else if (format == "color") {
            getColorOutputer(token_list, limit)(result);
        }
        else {
            std::cerr << "Internal error" << std::endl;
            return -1;
        }
    }

    return 0;
}
