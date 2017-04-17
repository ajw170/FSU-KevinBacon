/*
    KevinBacon.cpp
    11/04/15
*/


#include <moviematch.h>
#include <xstring.h>
#include <timer.h>

// in lieu of makefile
#include <xstring.cpp>
#include <bitvect.cpp>
#include <primes.cpp>
#include <hashfunctions.cpp>
#include <timer.cpp>
// */

bool BATCH = 0;

// set up buffer system for read loop
static size_t buffSize = 100;
static char* buffer = new char [buffSize + 1];

fsu::String GetName (std::istream& is)
// reads to end of line, including blanks
{
  size_t currSize = 0;
  char x;
  std::cin >> x;
  while ((x != '\n') && (!is.eof()))
  {
    if (currSize == buffSize)  // need more buffer 
    {
      buffSize += 100;
      char* newbuffer = new char [buffSize + 1];
      for (size_t i = 0; i < currSize; ++i)
      {
        newbuffer[i] = buffer[i];
      }
      delete [] buffer;
      buffer = newbuffer;
    }
    buffer[currSize++] = x;
    x = is.get();
  }
  buffer[currSize] = '\0';
  return buffer;
} // end read block

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cout << "command line arguments:\n"
              << " 1 (required): database file name\n"
              << " 2 (required): root actor name (delimited with single quotes \'Last, First\')\n"
              << " 3 (optional): verbose\n";
    return 0;
  }
  bool VERBOSE = 0;
  if (argc > 3)
    VERBOSE = 1;

  MovieMatch mm;

  // set up timer for Load call
  fsu::Timer timer;
  fsu::Instant time;
  timer.EventReset();
  bool success = mm.Load(argv[1]);
  time = timer.EventTime();
  if (!success)
  {
    std::cout << " ** KB: Failure to Load " << argv[1] << '\n';
    return EXIT_FAILURE;
  }
  else
  {
    std::cout << " Load time: ";
    time.Write_seconds(std::cout,2);
    std::cout << " sec\n";
  }
  timer.EventReset();
  success = mm.Init(argv[2]);
  time = timer.EventTime();
  if (!success)
  {
    std::cout << " ** failure to Init " << argv[2] << '\n';
    std::cout << "    try: \" kb.x movies.txt \'Bacon, Kevin\' \"\n";
    return EXIT_FAILURE;
  }
  else
  {
    std::cout << " Init time: ";
    time.Write_seconds(std::cout,2);
    std::cout << " sec\n";
  }
  if (VERBOSE) mm.Dump(std::cout);
  std::cout << "\nWelcome to MovieMatch ( " << argv[2] << " )\n";
  fsu::String name   = "1";
  fsu::String answer = "yes";
  long kbn = 0;

  while (1)
  {
    std::cout << "Enter actor name ('0' to quit, '$' to shuffle): ";
    name = GetName(std::cin);
    if (BATCH) std::cout << name << '\n';
    if (name.Size() == 1)
    {
      if (name[0] == '0') break;
      if (name[0] == '$')
      {
        mm.Shuffle();
        continue;
      }
    }
    kbn = mm.MovieDistance(name.Cstr()); 
    if (kbn == -3)
    {
      std::cout << " Name \'" << name << "\' not in DB \'" << argv[1] << "\'\n"
                << " Here are some similar name possibilities:\n";
      mm.Hint(name,std::cout);
    }
    else if (kbn == -2)
    {
      std::cout << " Name \'" << name << "\' unreachable from base actor \'" << argv[2] << "\'\n"
                << " Here are the names adjacent to \'" << name << "\' in the DB:\n";
      mm.ShowStar(name, std::cout);
    }
    else if (kbn == -1)
    {
      std::cout << " name \'" << name << "\' is a movie, not an actor\n";
      std::cout << "  Do you want details? ";
      std::cin >> answer;
      if (BATCH) std::cout << answer << '\n';
      if (answer.Element(0) == 'y' || answer.Element(0) == 'Y')
      {
        std::cout << "   The movie is:\n";
        mm.ShowStar(name, std::cout);
      }
    }
    else
    {
      std::cout << " The KB Number of \'" << name << "\' is: "
                << kbn
                << '\n';
      std::cout << "  Do you want proof? ";
      std::cin >> answer;
      if (BATCH) std::cout << answer << '\n';
      if (answer.Element(0) == 'y' || answer.Element(0) == 'Y')
      {
        std::cout << "   A connecting path is:\n";
        mm.ShowPath(std::cout);
        std::cout << "   The path is minimal because it was found with BFS [ref graph theory].\n";
      }
    }
  } // end while
  delete [] buffer;
  std::cout << "Thank you for playing Kevin Bacon\n";
  return EXIT_SUCCESS;
}
