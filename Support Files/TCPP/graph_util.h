/* 
    graph_util.h
    Chris Lacher
    03/19/17

    Copyright 2016, R.C. Lacher
*/

#ifndef _GRAPH_UTIL_H
#define _GRAPH_UTIL_H

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <map>
#include <cmath>
#include <graph.h>
#include <compare.h>
#include <gsort.h>
#include <pair.h>

template < class G >
void OutDegreeFrequencyDistribution (const G& g, fsu::Vector<size_t>& v)
{
  std::map<size_t, size_t> degcount;
  size_t deg, maxdeg = 0;
  for (size_t i = 0; i < g.VrtxSize(); ++i)
  {
    deg = g.OutDegree(i);
    if (maxdeg < deg) maxdeg = deg;
    ++degcount[deg];
  }
  v.SetSize(1+maxdeg,0);
  for(auto pr : degcount)
  {
    v[pr.first] = pr.second;
  }
}

template < class G >
void OutDegreeFrequencyDistribution (const G& g, std::ostream& os = std::cout)
{
  fsu::Vector<size_t> v(0);
  OutDegreeFrequencyDistribution(g,v);
  int cw = floor(log10(g.VrtxSize()));
  if (cw < 4) cw = 4;
  cw += 3;
  os << "  out-degree frequency distribution:"  << '\n';
  os  << std::setw(cw) << " deg" << std::setw(cw) << "freq" << '\n'
      << std::setw(cw) << " ---" << std::setw(cw) << "----" << '\n';
  for (size_t i = v.Size(); i > 0; )
  {
    --i;
    if (v[i] > 0) os << std::setw(cw) <<  i << std::setw(cw) << v[i] << '\n';
  }
}
 
template < class G >
bool OutDegreeFrequencyDistribution (const G& g, const char* filename)
{
  std::ofstream ofs;
  ofs.open(filename);
  if (ofs.fail())
    return 0;
  OutDegreeFrequencyDistribution(g,ofs);
  ofs.close();
  return 1;
}
 
template < class G >
void OutDegreeSequence (const G& g, fsu::Vector < fsu::Pair < size_t , size_t > > & degree)
{
  degree.SetSize(g.VrtxSize());
  for (size_t i = 0; i < degree.Size(); ++i)
  {
    degree[i].first_  = g.OutDegree(i);
    degree[i].second_ = i;
  }
  fsu::GreaterThan < fsu::Pair < size_t , size_t > > gt;
  fsu::g_merge_sort_bu(degree.Begin(),degree.End(),gt);
}

template < class G >
void OutDegreeSequence (const G& g, size_t maxToDisplay, std::ostream& os = std::cout)
{
  int cw1 = floor(log10(g.VrtxSize()));
  if (cw1 < 4) cw1 = 4;
  cw1 += 3;
  fsu::Vector < fsu::Pair < size_t , size_t > > degree(0);
  OutDegreeSequence(g,degree);
  size_t enough = degree.Size();
  if (0 < maxToDisplay && maxToDisplay < enough) enough = maxToDisplay;
  if (degree.Size() == enough)
    os << "  complete out-degree sequence:"  << '\n';
  else
    os << "  top " << enough << " vertex out-degrees ranked by degree:" << '\n';
  os << std::setw(cw1) << "rank"
     << std::setw(cw1) << "deg"
     << std::setw(cw1) << "vrtx" << '\n'
     << std::setw(cw1) << "----"
     << std::setw(cw1) << "---"
     << std::setw(cw1) << "----" << '\n';
  for (size_t i = 0; i < enough; ++i)
  {
    os << std::setw(cw1) << 1 + i
       << std::setw(cw1) << degree[i].first_
       << std::setw(cw1) << degree[i].second_
       << '\n';
    if (degree[i].first_ == 0 && 1 + i < degree.Size())
    {
      os << std::setw(cw1) << '*'
         << std::setw(cw1) << 0
         << std::setw(cw1) << "  (the remaining " << (degree.Size() - i - 1) << " vertices have degree 0)\n";
      break;
    }
  }
}

template < class G >
bool OutDegreeSequence (const G& g, size_t maxToDisplay, const char* outFile)
{
  std::ofstream outStream;
  outStream.open(outFile);
  if (outStream.fail())
  {
    return 0;;
  }
  OutDegreeSequence(g, maxToDisplay, outStream);
  outStream.close();
  return 1;
}

template < class G >
bool CheckSymmetry (const G& g, bool verbose = 1, std::ostream& os = std::cout)
{
  bool symmetric = 1;
  for (typename G::Vertex v = 0; v < g.VrtxSize(); ++v)
  {
    for (typename  G::AdjIterator j = g.Begin(v); j != g.End(v); ++j)
    {
      if (!g.HasEdge(*j,v))
      {
        symmetric = 0;
        if (verbose)
        {
          os << " ** Symmetry ALERT: graph is asymmetric at"
             << " (" << v << ',' << *j << ")\n";
        }
     /* else // Carolyn Wyllis innovation
        {
          return symmetric;
        } // */
      }
    }
  }
  return symmetric;
}

template < class G >
bool Load (std::istream& inStream, G& g)
{
  typename G::Vertex v, w; // general purpose vertex variables
  char ch;                 // general purpose file read variable  
  // skip documentation -- '#' in column 1
  ch = inStream.peek();
  while (ch == '#')
  {
    ch = inStream.get();
    while ((ch != '\n') && (!inStream.eof()))
      ch = inStream.get();
    ch = inStream.peek();
  }
  // read graph data and build graph
  inStream >> v;
  if (inStream.fail())
  {
    std::cerr << " ** Load error: vertex size not readable\n";
    return 0;
  }
  g.SetVrtxSize(v);
  while (inStream >> v >> w)
  {
    g.AddEdge(v,w);
  }
  return 1;
}

template < class G >
bool Load (const char* inFile, G& g)
{
  std::ifstream inStream;
  inStream.open(inFile);
  if (inStream.fail())
  {
    return 0;;
  }
  bool status = Load(inStream,g);
  inStream.clear();
  inStream.close();
  return status;
}

template < class G >
bool Save_UG (const char* filenameBase, const G& g)
{
  size_t size = strlen(filenameBase);
  char filename [size+4];
  strcpy(filename,filenameBase);
  filename[size] = '.';
  filename[size+1] = 'u';
  filename[size+2] = 'g';
  filename[size+3] = '\0';
  std::ofstream outStream;
  outStream.open(filename);
  if (outStream.fail())
  {
    return 0;;
  }
  typename G::Vertex v;
  typename G::AdjIterator i; 
  outStream << g.VrtxSize() << '\n';
  for (v = 0; v < g.VrtxSize(); ++v)
  {
    for (i = g.Begin(v); i != g.End(v); ++i)
    {
      if (v < *i) outStream << v << '\t' << *i << '\n';
    }
  }
  outStream.close();
  std::cout << " ungraph saved as " << filename << '\n';
  return 1;
}

template < class G >
bool Save_DG (const char* filenameBase, const G& g)
{
  size_t size = strlen(filenameBase);
  char filename [size+4];
  strcpy(filename,filenameBase);
  filename[size] = '.';
  filename[size+1] = 'd';
  filename[size+2] = 'g';
  filename[size+3] = '\0';
  std::ofstream outStream;
  outStream.open(filename);
  if (outStream.fail())
  {
    return 0;;
  }
  typename G::Vertex v;
  typename G::AdjIterator i; 
  outStream << g.VrtxSize() << '\n';
  for (v = 0; v < g.VrtxSize(); ++v)
  {
    for (i = g.Begin(v); i != g.End(v); ++i)
    {
      outStream << v << '\t' << *i << '\n';
    }
  }
  outStream.close();
  std::cout << " digraph saved as " << filename << '\n';
  return 1;
}

template < class G >
void ShowAL (const G& g, std::ostream& os = std::cout)
{
  typedef typename G::Vertex Vertex;
  typename G::AdjIterator i;
  for (Vertex v = 0; v < g.VrtxSize(); ++v)
  {
    os << '[' << v << "]->";
    i = g.Begin(v);
    if (i != g.End(v))
    {
      os << *i;
      ++i;
    }
    for ( ; i != g.End(v); ++i)
    {
      os << ',' << *i;
    }
    os << '\n';
  }
}

const char* GraphTypeName(const char* code)
{
  switch(code[0])
  {
    case 'U':
      switch(code[1])
      {
        case 'L': return "undirected adjacency list";
        case 'M': return "undirected adjacency matrix";
      }
    case 'D':
      switch(code[1])
      {
        case 'L': return "directed adjacency list";
        case 'M': return "directed adjacency matrix";
      }
    default: return "unknown typecode";
  }
}

void FileSpec(std::ostream& os)
{
  os << " -----------------------------------------------------------------------\n"
     << " Graph File Specs                                                       \n"
     << " ----------------                                                       \n"
     << " 1 File documentation at beginning of file, lines begin with '#'        \n"
     << " 2 Remainder of file consists of unsigned integers in decimal notation  \n"
     << " 2.1 First number is vertex size                                        \n"
     << " 2.2 Remaining numbers occur in pairs                                   \n"
     << " 2.3 Pair represents an edge \"from\" \"to\"                                \n"
     << " 2.4 Edge vertices must be in range determined by 2.1                   \n"
     << " -----------------------------------------------------------------------\n";
}

#endif
