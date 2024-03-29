//$Id$
/*  Copyright (C) 2004-2006 John B. Shumway, Jr.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef ENABLE_MPI
#include <mpi.h>
#endif

#include "parser/MainParser.h"
#include "Help.h"
#include "demo/Demo.h"
#if HAVE_GETOPT_H
  #include <getopt.h>
#else
  #include "gnugetopt.h"
#endif
void usage();
/** @mainpage pi: Path integral quantum Monte Carlo
  * @section usage Usage
  * To run the program, supply a pimc.xml file and any other input files,
  * then run the executable.
  * If you don't have an input file, try
  * @code
  * pi --demo FreeParticles
  * @endcode
  * This will generate a pimc.xml file for free particles.
  * Use "--demo" without any name to see a list of other demos.
  * @section overview Overview
  * <p>This program implements the path integral Monte Carlo technique.
  * The goal of this program is to develop PIMC methods for applied 
  * physics.
  * This code is hosted at http://code.google.com/p/pi-qmc ,
  * where you can also find some documentation and user support. </p>
  * @section structure Code structure
  * <p>To get started understanding the code, take a look at the
  * <a href="hierarchy.html">Class Hierarchy</a>.
  * The objects needed for the simulation are constructed in the
  * XMLParser classes: SimInfoParser, ActionParser, EstimatorParser
  * and PIMCParser. The main Monte Carlo algorithm is implemented in
  * MultiLevelSampler, and path coordinates are stored in Paths and Beads.
  * @author John Shumway */
int main(int argc, char** argv) {
  int rank=0;
#ifdef ENABLE_MPI
  MPI::Init(argc,argv);
  rank=MPI::COMM_WORLD.Get_rank();
#endif
  if (rank==0) {
    std::cout << std::endl;
    std::cout << "PIMC program: "  << PACKAGE_STRING 
#ifdef ENABLE_MPI
              << " (MPI enabled, "
#else
              << " (MPI disabled, "
#endif
#ifdef ENABLE_SPRNG
              << " SPRNG enabled, "
#else
              << " SPRNG disabled, "
#endif
              <<  NDIM << "-d)" << std::endl;
    std::cout << std::endl;
  }
  // Parse command line.
  //bool verbose=false;
  static struct option longopts[] = {
    { "help",     no_argument,  NULL, 'h' },
    { "demo",     optional_argument,  NULL, 'd' },
    { "version",  no_argument,  NULL, 'V' },
    { NULL,       0,            NULL, 0 }
  };
  char ch;
  while (( ch = getopt_long(argc, argv, "hdV", longopts, NULL)) != -1) {
    std::string demoName;
    switch (ch) {
      case 'd': {
        if (optarg!=NULL) {
          demoName=std::string(optarg);
          std::cout << "Requested demo: " << demoName << "\n" << std::endl;
          Demo *demo=Demo::getDemo(demoName);
          if (demo) {
            demo->generate();
            exit(0);
          } else {
            Demo::listDemos(std::cout);
            exit(-1);
          }
        } else {
          Demo::listDemos(std::cout);
          exit(0);
        } }
        break;
      case 'V':
#ifdef CONFIG_FLAGS
        std::cout<<"Compiled with options: "<<std::endl<<CONFIG_FLAGS<<std::endl<<std::endl;
#endif
        exit(-1);
      case 'h':
        Help::printHelp();
      default:
        usage();
    }
  }
  argc -= optind;
  argv += optind;

  std::string xmlFileName="pimc.xml";
  if (argc==1) xmlFileName=argv[0];
  MainParser parser(xmlFileName);
  parser.parse();

#ifdef ENABLE_MPI
  MPI::Finalize();
#endif
  return 0;
};

void usage() {
  std::cout << "usage: pi [OPTIONS] [pimc.xml]" << std::endl;
  std::cout << "  OPTIONS" << std::endl;
  std::cout << "     -h, --help           Print a usage message and exit"
            << std::endl;
  std::cout << "     -V, --version        Print version number and exit"
            << std::endl;
  std::cout << "     -d, --demo=name      Write input file for a demo\n";
  std::cout << "                          (omit name to get a list of "
                                      << "available demos)"
            << std::endl;
  exit(0);
}
