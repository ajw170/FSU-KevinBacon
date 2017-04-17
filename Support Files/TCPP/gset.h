/*
    gset.h
    01/01/13
    Chris Lacher

    generic set algorithms

    All of these algorithms have a common set of preconditions, as follows:

    1. I1, I2 are input iterators and I3 is output iterator. 
       (I3 may be an insert iterator.)
    2. The input ranges are specified by pairs of iterators of type I1, I2 respectively
    3. The beginning of the output range is specified by an iterator of type I3
    4. The input ranges have the same ValueType,
       which is convertible to the output range ValueType
    5. P is a predicate order class defined on the ValueType of the input iterators
    6. p is a predicate object of type P
    7. Note that p is passed by reference to allow passing predicates with state
    7. The two input ranges are sorted using p where present and < otherwise

    A common postcondition is: the output range is sorted (using p where present
    or < otherise)

    Copyright 2009-13, R. C. Lacher
*/

#ifndef _GSET_H
#define _GSET_H

namespace fsu
{

  template <class I1, class I2, class I3, class P>
  void g_set_union(I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest, P& p)
  // range3 = range1 union range2
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if (p(*beg1, *beg2))
      {
	*dest = *beg1;
	++beg1;
      }
      else if (p(*beg2, *beg1))
      {
	*dest = *beg2;
	++beg2;
      }
      else
      {
	*dest = *beg1;
	++beg1;
	++beg2;
      }
      ++dest;
    }
    while (beg1 != end1)
    {
      *dest++ = *beg1++;
    }
    while (beg2 != end2)
    {
      *dest++ = *beg2++;
    }
  }

  template <class I1, class I2, class I3>
  void g_set_union(I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest) 
  // default order version uses operator <
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if (*beg1 < *beg2)
      {
	*dest = *beg1;
	++beg1;
      }
      else if (*beg2 < *beg1)
      {
	*dest = *beg2;
	++beg2;
      }
      else
      {
	*dest = *beg1;
	++beg1;
	++beg2;
      }
      ++dest;
    }
    while (beg1 != end1)
    {
      *dest++ = *beg1++;
    }
    while (beg2 != end2)
      {
	*dest++ = *beg2++;
      }
  }

  template <class I1, class I2, class I3, class P>
  void g_set_merge (I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest, P& p)
  // range3 = range1 merge range2 
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if (p(*beg2,*beg1))
      {
	*dest++ = *beg2++;
      }
      else 
      {
	*dest++ = *beg1++;
      }
    }
    while (beg1 != end1)
    {
      *dest++ = *beg1++;
    }
    while (beg2 != end2)
    {
      *dest++ = *beg2++;
    }
  }

  template <class I1, class I2, class I3>
  void g_set_merge (I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest)
  // default order version uses operator < ()
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if ((*beg2) < (*beg1))
      {
	*dest++ = *beg2++;
      }
      else 
      {
	*dest++ = *beg1++;
      }
    }
    while (beg1 != end1)
    {
      *dest++ = *beg1++;
    }
    while (beg2 != end2)
    {
      *dest++ = *beg2++;
    }
  }

  template <class I1, class I2, class I3, class P>
  void g_set_intersection(I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest, P& p)
  // range3 = range1 intersection range2 
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if (p(*beg2,*beg1))
      {
	++beg2;
      }
      else if (p(*beg1, *beg2))
      {
	++beg1;
      }
      else
      {
	*dest++ = *beg1; // *beg1 == *beg2
	++beg1;
	++beg2;
      }
    }
  }

  template <class I1, class I2, class I3>
  void g_set_intersection(I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest)
  // default order version uses operator <()
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if ((*beg2) < (*beg1))
      {
	++beg2;
      }
      else if ((*beg1) < (*beg2))
      {
	++beg1;
      }
      else
      {
	*dest++ = *beg1; // *beg1 == *beg2
	++beg1;
	++beg2;
      }
    }
  }

  template <class I1, class I2, class I3, class P>
  void g_set_difference(I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest, P& p)
  // range3 = range1 difference range2 
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if (p(*beg2,*beg1))
      {
	++beg2;
      }
      else if (p(*beg1, *beg2))
      {
	*dest++ = *beg1++;
      }
      else
      {
	++beg1;
	++beg2;
      }
    }
    while (beg1 != end1)
      *dest++ = *beg1++;
  }

  template <class I1, class I2, class I3>
  void g_set_difference(I1 beg1, I1 end1, I2 beg2, I2 end2, I3 dest)
  // default order version uses operator <()
  {
    while (beg1 != end1 && beg2 != end2)
    {
      if ((*beg2) < (*beg1))
      {
	++beg2;
      }
      else if ((*beg1) < (*beg2))
      {
	*dest++ = *beg1++;
      }
      else
      {
	++beg1;
	++beg2;
      }
    }
    while (beg1 != end1)
      *dest++ = *beg1++;
  }

  template <class I1, class I2, class P>
  bool g_subset_of(I1 beg1, I1 end1, I2 beg2, I2 end2, P& p)
  // returns true iff range 1 is contained in range 2
  {
    while (beg1 != end1 && beg2 != end2)
      if (p(*beg1, *beg2))       // *beg1 lies below *beg2 in ValueType
	return 0;                  // hence *beg1 not in range2
      else if (p(*beg2, *beg1))  // *beg1 lies above *beg2 in ValueType
	++beg2;                    // *beg1 may still be in range2
      else                       // *beg1 == *beg2
	++beg1, ++beg2;            // *beg1 found in range2
    return (beg1 == end1);       // true iff all elements of range1 have been found in range2
  }

  template <class I1, class I2>
  bool g_subset_of (I1 beg1, I1 end1, I2 beg2, I2 end2)
  // default version uses operator <()
  {
    while (beg1 != end1 && beg2 != end2)
      if ((*beg1) < (*beg2))       // *beg1 lies below *beg2 in ValueType
	return 0;
      else if ((*beg2) < (*beg1))  // *beg1 lies above *beg2 in ValueType
	++beg2;
      else                         // *beg1 == *beg2
	++beg1, ++beg2;
    if (beg1 == end1)              // all elements of range1 have been found in range2
      return 1;
    return 0;
  }

} // namespace fsu

#endif
