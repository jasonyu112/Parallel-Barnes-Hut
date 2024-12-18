#include "argparse.h"

void get_opts(int argc,
    char** argv,
    struct options_t* opts)
{
    if (argc == 1)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t-i inputfile <char*>" << std::endl;
        std::cout << "\t-o outputfile <char*>" << std::endl;
        std::cout << "\t-s steps <int>" << std::endl;
        std::cout << "\t-t theta <double>" << std::endl;
        std::cout << "\t-d dt <double>" << std::endl;
        std::cout << "\t-v <OPTIONAL>" << std::endl;
        exit(0);
    }
    
    opts->v = false;

    struct option l_opts[] = {
        {"inputfilename", required_argument, NULL, 'i'},
        {"outputfilename", required_argument, NULL, 'o'},
        {"steps", required_argument, NULL, 's'},
        {"theta", required_argument, NULL, 't'},
        {"dt", required_argument, NULL, 'd'},
        {"v", no_argument, NULL, 'v'},
    };

    int ind, c;
    while ((c = getopt_long(argc, argv, "i:o:s:t:d:v", l_opts, &ind)) != -1)
    {
        switch (c)
        {
        case 0:
            break;
        case 'i':
            opts->in_file = (char*)optarg;
            break;
        case 'o':
            opts->out_file = (char*)optarg;
            break;
        case 's':
            opts->steps = atoi((char*)optarg);
            break;
        case 't':
            opts->theta = std::strtod((char*)optarg, nullptr);
            break;
        case 'd':
            opts->dt = std::strtod((char*)optarg, nullptr);
            break;
        case 'v':
            opts->v = true;
            break;

        case ':':
            std::cerr << argv[0] << ": option -" << (char)optopt << "requires an argument." << std::endl;
            exit(1);
        }
    }
    
}
