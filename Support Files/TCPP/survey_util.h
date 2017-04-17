/* 
    survey_util.h

    Chris Lacher
    09/28/16

    Levelorder(BFS)
    Preorder(DFS)
    Postorder(DFS)
    WriteData(BFS)
    WriteData(DFS)
    Arguments()   - called by fbfs_ug, fbfs_dg, fdfs_ug,fdfs_dg

    Copyright 2016, R.C. Lacher
*/

#ifndef _SURVEY_UTIL_H
#define _SURVEY_UTIL_H

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <bfsurvey.h>
#include <dfsurvey.h>

template < class G >
void Levelorder (const fsu::BFSurvey<G>& bfs, std::ostream& os)
{
  // for BFS, dtime is 1-1 map [0,size) -> [0,size)
  // complicated  when search is partial (some undiscovered vertices)
  size_t size = bfs.VrtxSize();
  fsu::Vector <size_t> dorder(size,size);

  // invert dtime
  size_t blacks = 0;
  for (size_t i = 0; i < size; ++i)
  {
    if (bfs.Color()[i] == 'b')
    {
      ++blacks;
      dorder[bfs.DTime()[i]] = i;
    }
  }

  // compress
  if (blacks < size)
  {
    size_t skip = 0;
    for (size_t i = 0; i < size; ++i)
    {
      if (dorder[i] ==  size)  // infinity
      {
        ++skip;
      }
      else
      {
        dorder[i - skip] = dorder[i];
      }
      // std::cout << " skip = " << skip << '\n';
    } // */
    // assert (blacks + skip == size)
    if (blacks + skip != size)
    {
      std::cerr << " ** Levelorder Error: black_count = " << blacks << '\n' 
                << "                       skip_count = " << skip << '\n' 
                << "                     vertex_count = " << size << "\n\n"; 
    }
  }
  // now all black vertices are discovered
  // discovery order for black vertices is dorder[0..blacks-1]

  os << "  Vertex discovery order:";
  for (size_t i = 0; i < blacks; ++i)
  {
    os << ' ' << dorder[i];
  }
  os << '\n';

  os << "     grouped by distance:";
  size_t i = 0; // i = discovery order
  size_t vertex   = dorder[i];
  size_t distance = bfs.Distance()[vertex];
  os << " [ ( " << vertex; 
  while (i < blacks - 1)
  {
    size_t nextvertex = dorder[i+1];
    size_t nextdistance = bfs.Distance()[nextvertex];
    if (nextdistance == 0)
    {
      os << " ) ] [ ( " << nextvertex;
    }
    else if (nextdistance == distance)
    {
      os << ' ' << nextvertex;
    }
    else if (nextdistance < distance)
    {
      std::cerr << " ** logic error in distance grouping\n";
      return;
    }
    else // nextdistance > distance
    {
      os << " ) ( " << nextvertex;
    }
    distance = nextdistance;
    ++i;
  }
  os << " ) ]\n"; // 
}

template < class G >
void Preorder (const fsu::DFSurvey<G>& dfs, std::ostream& os)
{
  // for DFS, dtime is 1-1 map [0,size) -> [0,2*size)
  size_t size = dfs.VrtxSize();
  fsu::Vector <size_t> dorder(2*size,2*size);
 
  // invert dtime
  /* game plan:
     (1) "invert" dtime by mapping to dorder
          then 1/2 of the entries will be unhit (still 2|V| = infinity)
     (2) compress dorder by skipping the infinity entries
     (3) complicated when search is partial - process only black vertices
  */

  // map 
  size_t blacks = 0;
  for (size_t i = 0; i < size; ++i)
  {
    if (dfs.Color()[i] == 'b')
    {
      ++blacks;
      dorder[dfs.DTime()[i]] = i;
    }
  }
  // compress
  size_t skip = 0;
  for (size_t i = 0; i < 2*size; ++i)
  {
    if (dorder[i] ==  2*size)  // infinity
    {
      ++skip;
    }
    else
    {
      dorder[i - skip] = dorder[i];
    }
    // std::cout << " skip = " << skip << '\n';
  } // */
  // assert (blacks + skip == size)
  if (blacks + skip != 2*size)
  {
    std::cerr << " ** Preorder Error: black_count = " << blacks << '\n' 
              << "                     skip_count = " << skip << '\n' 
              << "                 vertex_count*2 = " << 2*size << "\n\n"; 
  }
  
  os << "  Vertex discovery order:";
  for (size_t i = 0; i < blacks; ++i)
    os << ' ' << dorder[i];
  os << '\n';
}

template < class G >
void Postorder (const fsu::DFSurvey<G>& dfs, std::ostream& os)
{
  // for DFS, ftime is 1-1 map [0,size) -> [0,2*size)
  size_t size = dfs.VrtxSize();
  fsu::Vector <size_t> forder(2*size,2*size);
 
  // invert ftime
  /* game plan:
     (1) "invert" ftime by mapping to forder
          then 1/2 of the entries will be unhit (still 2|V| = infinity)
     (2) compress forder by skipping the infinity entries
     (3) complicated when search is partial - process only black vertices
  */

  // map
  size_t blacks = 0; 
  for (size_t i = 0; i < size; ++i)
  {
    if (dfs.Color()[i] == 'b')
    {
      ++blacks;
      forder[dfs.FTime()[i]] = i;
    }
  }
  // compress
  size_t skip = 0;
  for (size_t i = 0; i < 2*size; ++i)
  {
    if (forder[i] ==  2*size)  // infinity
    {
      ++skip;
    }
    else
    {
      forder[i - skip] = forder[i];
    }
    // std::cout << " skip = " << skip << '\n';
  } // */
  // assert (blacks + skip == size)
  if (blacks + skip != 2*size)
  {
    std::cerr << " ** Postorder Error: black_count = " << blacks << '\n' 
              << "                      skip_count = " << skip << '\n' 
              << "                  vertex_count*2 = " << 2*size << "\n\n"; 
  }
  
  os << "  Vertex finishing order:";
  for (size_t i = 0; i < blacks; ++i)
    os << ' ' << forder[i];
  os << '\n';
}

template < class G >
void WriteData (const fsu::BFSurvey<G>& bfs, std::ostream& os)
{
  int c1 = 8,
      c2 = 10,
      c3 = 11,
      c4 = 12,
      c5 = 13;

  os << '\n';
  os << std::setw(c1 + c2 - 2) << "bf survey data" << '\n';
  os << std::setw(c1 + c2 - 2) << "==============" << '\n';
  os << std::setw(c1) << "vertex"
     << std::setw(c2) << "distance"
     << std::setw(c3) << "dtime"
     << std::setw(c4) << "parent"
     << std::setw(c5) << "color" << '\n';
  os << std::setw(c1) << "------"
     << std::setw(c2) << "--------"
     << std::setw(c3) << "-----"
     << std::setw(c4) << "------"
     << std::setw(c5) << "-----" << '\n';
  for (size_t i = 0; i < bfs.VrtxSize(); ++i)
  {
    os << std::setw(c1) << i
       << std::setw(c2) << bfs.Distance()[i]
       << std::setw(c3) << bfs.DTime()[i];
    if ((bfs.Parent())[i] < bfs.VrtxSize())
      os << std::setw(c4) << (bfs.Parent())[i];
    else
      os << std::setw(c4) << "NULL";
    os << std::setw(c5) << bfs.Color()[i]
       << '\n';
  }
}

template < class G >
void WriteData (const fsu::DFSurvey<G>& dfs, std::ostream& os)
{
  int c1 = 8,
      c2 = 10,
      c3 = 11,
      c4 = 12,
      c5 = 13;

  os << '\n';
  os << std::setw(c1 + c2 - 2) << "df survey data" << '\n';
  os << std::setw(c1 + c2 - 2) << "==============" << '\n';
  os << std::setw(c1) << "vertex"
     << std::setw(c2) << "dtime"
     << std::setw(c3) << "ftime"
     << std::setw(c4) << "parent"
     << std::setw(c5) << "color" << '\n';
  os << std::setw(c1) << "------"
     << std::setw(c2) << "-----"
     << std::setw(c3) << "-----"
     << std::setw(c4) << "------"
     << std::setw(c5) << "-----" << '\n';
  for (size_t i = 0; i < dfs.VrtxSize(); ++i)
  {
    os << std::setw(c1) << i
       << std::setw(c2) << dfs.DTime()[i]
       << std::setw(c3) << dfs.FTime()[i];
    if ((dfs.Parent())[i] < dfs.VrtxSize())
      os << std::setw(c4) << (dfs.Parent())[i];
    else
      os << std::setw(c4) << "NULL";
    os << std::setw(c5) << dfs.Color()[i]
       << '\n';
  }
}

void Arguments(std::ostream& os)
{
  os << " Required arguments\n"
     << " ------------------\n"
     << " [1]: char*     input_filename\n"
     << " [2]: char      's' = single search, 'f' = full survey\n"
     << " [3]: unsigned  start_vertex [0..vsize)\n"
     << " [4]: bool      '0' = no trace, '1' = trace\n";
} 

#endif
