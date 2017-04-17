/*
    gsort_non-rec.h
    09/15/16
    Chris Lacher

    generic sort algorithms applicable to a range specified by iterators

    includes these generic sort algorithms:

      g_selection_sort
      // g_merge_sort
      g_merge_sort_bu
      g_insertion_sort
      // g_quick_sort
 
    note that g_heap_sort is located in gheap.h

    Copyright 2016, R. C. Lacher
*/

#include <genalg.h> // Swap, g_copy
#include <gset.h>   // g_set_merge
#include <debug.h>

#ifndef _GSORT_H
#define _GSORT_H

// const int qs_cutoff_threshold   = 7;
// const int qs3w_cutoff_threshold = 7;
// const int ms_cutoff_threshold   = 7;

namespace fsu
{

  template < class ForwardIterator >
  void g_selection_sort (ForwardIterator beg, ForwardIterator end)
  {
    ForwardIterator i, j, k;
    for (i = beg; i != end; ++i)
    {
      k = i;
      for (j = i; j != end; ++j)
        if (*j < *k)
          k = j;
      Swap (*i, *k);
    }
  }

  template < class ForwardIterator, class Comparator >
  void g_selection_sort (ForwardIterator beg, ForwardIterator end, Comparator& cmp)
  {
    ForwardIterator i, j, k;
    for (i = beg; i != end; ++i)
    {
      k = i;
      for (j = i; j != end; ++j)
        if (cmp(*j,*k))
          k = j;
      Swap (*i, *k);
    }
  }

  template < class BidirectionalIterator >
  void g_insertion_sort (BidirectionalIterator beg, BidirectionalIterator end)
  {
    BidirectionalIterator i, j, k;
    typename BidirectionalIterator::ValueType t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && t < *k; --j, --k)
        *j = *k;
      *j = t;
    }
  }

  template < class BidirectionalIterator, class Comparator >
  void g_insertion_sort (BidirectionalIterator beg, BidirectionalIterator end, Comparator& cmp)
  {
    BidirectionalIterator i, j, k;
    typename BidirectionalIterator::ValueType t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && cmp(t,*k); --j, --k)
        *j = *k;
      *j = t;
    }
  }

  // specialization for pointers
  template < typename T >
  void g_insertion_sort (T* beg, T* end)
  {
    // fsu::Debug ("g_insertion_sort");
    if (end - beg < 2) return;
    T *i, *j, *k;
    T t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && t < *k; --j, --k)
        *j = *k;
      *j = t;
    }
  }

  // specialization for pointers
  template < typename T , class Comparator >
  void g_insertion_sort (T* beg, T* end, Comparator& cmp)
  {
    if (end - beg < 2) return;
    T *i, *j, *k;
    T t;
    for (i = beg; i != end; ++i)
    {
      t = *i;
      for (k = i, j = k--; j != beg && cmp(t,*k); --j, --k)
        *j = *k;
      *j = t;
    }
  }

  namespace mergesort
  {

    // these are the special "merge" functions supporting merge sort

    // for random access iterators and default order
    template < typename RAIterator >
    void Merge(RAIterator beg, RAIterator mid, RAIterator end)
    {
      // std::cerr << "mergesort::Merge(" << end - beg << ")\n";
      fsu::Vector < typename RAIterator::ValueType > B(end-beg);  // temp storage for merged copy
      g_set_merge(beg, mid, mid, end, B.Begin());           // merge the two parts of A to B
      g_copy(B.Begin(), B.End(), beg);             // copy B back to A[p,r)
    }

    // for random access iterators and order determined by a predicate object
    template < typename RAIterator , class Comparator >
    void Merge(RAIterator beg, RAIterator mid, RAIterator end, Comparator& cmp)
    {
      // std::cerr << "mergesort::Merge(" << end - beg << ")\n";
      fsu::Vector < typename RAIterator::ValueType > B(end-beg);
      g_set_merge(beg, mid, mid, end, B.Begin(),cmp);
      g_copy(B.Begin(), B.End(), beg);
    }

    // specialization for pointers and default order
    template < typename T >
    void Merge(T* beg, T* mid, T* end)
    {
      // std::cerr << "mergesort::Merge(" << end - beg << ")\n";
      fsu::Vector < T > B(end-beg);
      g_set_merge(beg, mid, mid, end, B.Begin());
      g_copy(B.Begin(), B.End(), beg);
    }

    // specialization for pointers and predicate order
    template < typename T , class Comparator >
    void Merge(T* beg, T* mid, T* end, Comparator& cmp)
    {
      // std::cerr << "mergesort::Merge(" << end - beg << ")\n";
      fsu::Vector < T > B(end-beg);
      g_set_merge(beg, mid, mid, end, B.Begin(),cmp);
      g_copy(B.Begin(), B.End(), beg);
    }

  } // namespace mergesort

  template < class RAIterator , class Comparator >
  void g_merge_sort_bu (RAIterator beg, RAIterator end, Comparator& cmp)
  {
    size_t n = end - beg;
    if (n < 2) return;
    for (size_t i = 1; i < n; i = i+i)
    {
      for (size_t j = 0; j < n - i; j += i+i)
      {
        if (n < j+i+i)
          mergesort::Merge(beg + j, beg + j+i, end, cmp);
        else
          mergesort::Merge(beg + j, beg + j+i, beg + j+i+i, cmp);
      }
    }
  }

  template < class RAIterator >
  void g_merge_sort_bu (RAIterator beg, RAIterator end)
  {
    size_t n = end - beg;
    if (n < 2) return;
    for (size_t i = 1; i < n; i = i+i)
    {
      for (size_t j = 0; j < n - i; j += i+i)
      {
        if (n < j+i+i)
          mergesort::Merge(beg + j, beg + j+i, end);
        else
          mergesort::Merge(beg + j, beg + j+i, beg + j+i+i);
      }
    }
  }

} // namespace 

#endif
