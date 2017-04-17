/*
    topsort.h
    08/30/14

    Topological Sort Function Template

    Call: TopSort(diGraph, outQueue) 

     - performs a topological sort on diGraph,
     - places the result in outQueue, and
     - returns 1 if the sort is successful (indicating diGraph is a DAG) 
     - returns 0 if the sort is not successful (outQueue contains a partial sort) 

     IN:  1. diGraph conforms to the API defined in igraph.h
          2. outQueue exposes a public operation Push(int)
          3. outQueue is initially empty

     OUT: 1. diGraph is unchanged
          2. outQueue contains result of topological sort on diGraph
          3. outQueue.Size() == diGraph.vSize() iff diGraph is DAG
          4. return (outQueue.Size() == diGraph.vSize() ? 1 : 0)

    Copyright 2014, R.C. Lacher
*/

#include <queue.h>
#include <stack.h>
#include <vector.h>

namespace fsu
{
  template <class DigraphType, class ResultType>
  bool TopSort (const DigraphType& diGraph, ResultType& outQueue)
  {
    typedef typename DigraphType::Vertex      Vertex;
    typedef typename DigraphType::AdjIterator AdjIterator;

    fsu::Queue < Vertex >   conQueue;
    // fsu::Stack < Vertex >   conStack;
    // conQueue stores current source vertices prior to processing
    // note we could use other structures here to give possibly different topsorts 

    fsu::Vector < Vertex >  inDegree(diGraph.VrtxSize(),0);
    // current in-degree of each vertex

    // preprocess to get all InDegrees (more efficient than search)
    inDegree.SetSize(diGraph.VrtxSize(), 0);
    for (Vertex v = 0; v < diGraph.VrtxSize(); ++v)
    {
      for (AdjIterator i = diGraph.Begin(v); i != diGraph.End(v); ++i)
      {
	++inDegree[(size_t)*i];
      }
    }

    // main algorithm
    Vertex v, w;
    for (v = 0; v < diGraph.VrtxSize(); ++v)
    {
      if (inDegree[v] == 0)
      {
	conQueue.Push(v);
      }
    }
    while (!conQueue.Empty())
    {
      // v = conQueue.Top(); // if conQueue is a Stack
      v = conQueue.Front();
      conQueue.Pop();
      outQueue.Push(v);
      for (AdjIterator i = diGraph.Begin(v); i != diGraph.End(v); ++i)
      {
	w = *i;
	--inDegree[w];
	if (inDegree[w] == 0) conQueue.Push(w);
      }
    } // end while

    // report result
    if (outQueue.Size() != diGraph.VrtxSize())
      return 0;
    return 1;
  } // TopSort

} // fsu
