/*
    primes.cpp
    12/21/16
    Chris Lacher

    implementations of functions prototyped in primes.h

    Copyright 2009-16, R.C. Lacher
*/

#ifndef _PRIMES_CCP
#define _PRIMES_CCP

#include <primes.h>
#include <timer.h>
#include <iomanip>

#include <timer.cpp>

namespace fsu
{
  size_t PrimeBelow (size_t n, bool ticker)
  // returns largest prime number <= n
  // This is likely to be more efficient than PrimeAbove(), because
  // it only needs a bitvector of size n+1
  {
    if (n <= 1) return 0;
    if (n == 2) return 2;
    if (n >= n+1)
    {
      std::cerr << " ** PrimeBelow: argument too large for implementation. Execution terminating.\n";
      exit (EXIT_FAILURE);
    }
    fsu::BitVector b(1+n);
    Sieve(b, ticker);
    if (n%2 == 0) --n; // make n odd
    while (n > 2)
    {
      if (b.Test(n))
        return n;
      n -= 2;
    }
    return 2;
  }

  size_t PrimeBelowOpt (size_t n, bool ticker)
  // odd integer version
  {

    if (n <= 1) return 0;
    if (n == 2) return 2;

    if (1 == n%2 && n < n+1) // n is odd and not the largest size_t
      ++n;
    fsu::BitVector b(n/2);
    SieveOpt(b, ticker);
    for (size_t k = n/2; k > 1; )
    {
      --k;
      if (b.Test(k))
        return 2*k+1;
    }
    return 2;
  }

  size_t PrimeAbove (size_t n, bool ticker)
  // returns smallest prime number >= n
  // or zero if input is too large
  {
    if (n <= 2) return 2;
    size_t m = n << 1; // m = 2n 
    if (m <= n)        // 2n <= n => overflow
    {
      return 0;
    }
    fsu::BitVector b(m);
    Sieve(b, ticker);

    while (n < m)
    {
      if (b.Test(n))
        return n;
      ++n;
    }
    return PrimeAbove(m,ticker);
  }

  // tested experimental version 
  void AllPrimesBelowOpt (size_t n, std::ostream& os, bool ticker)
  // writes all primes <= n to os
  {
    if (n < n + 1) // n is not the largest size_t
      ++n;
    fsu::BitVector b(n/2);
    SieveOpt(b, ticker);
    if (n > 2) os << ' ' << 2;
    for (size_t i = 1; i < n/2; ++i)
      if (b.Test(i))
	os << ' ' << 2*i+1;
    os << std::endl;
  }

  // tested experimental version
  void SieveOpt(BitVector& b, bool ticker)
  // pre:  b is a BitVector
  // post: for all odd n < b.Size(), 
  //       n is prime iff 1 = b.Test(n)
  {
    // set up timer
    fsu::Timer timer;
    fsu::Instant time;
    if (ticker)
    {
      timer.EventReset();
    }

    const size_t max = 1+2*b.Size();
    size_t sqrt = 2;
    while (sqrt*sqrt < max)
    {
      ++sqrt;
    }

    b.Set();
    b.Unset(0);  // 1 = 2*0+1 is not prime

    // clear bits at multiples of all odd primes < sqrt(max)
    sqrt /= 2;
    for (size_t i = 1, k = 3; i < sqrt; i += 1, k += 2)  // k = 2*i+1
    {
      if (b.Test(i)) // k is prime
      {
        if (ticker) std::cout << '.' << std::flush;
        // for (size_t j = k*k; j < max; j+= 2*k) // these are the odd j's - see Lemmas
        // {
	//   b.Unset(j/2);
        // }
        // for (size_t j = k*k, l = j/2; j < max; j+= 2*k, l+= k) // these are the odd j's - see Lemmas
        for (size_t l = k*k/2; l < max/2; l+= k) // j = 2l+1 are the odd j's - see Lemmas
        {
	  b.Unset(l);
        }
      }
    }
    if (ticker)
    {
      time = timer.EventTime();
      std::cout << '\n';
      std::cout << " Sieve time: ";
      time.Write_seconds(std::cout,2);
      std::cout << " sec\n";
    }
  }  // end Sieve()
  // */

  // tested version 2 - ignores all even bits
  void AllPrimesBelow (size_t n, std::ostream& os, bool ticker)
  // writes all primes <= n to os
  {
    if (n < n + 1) // n is not the largest size_t
      ++n;
    fsu::BitVector b(n);
    Sieve(b, ticker);
    if (n >= 2) os << ' ' << 2;
    for (size_t i = 3; i < n; i += 2)
      if (b.Test(i))
	os << ' ' << i;
    os << std::endl;
  }

  void Sieve(BitVector& b, bool ticker)
  // pre:  b is a BitVector
  // post: for all odd n < b.Size(), 
  //       n is prime iff 1 = b.Test(n)
  {
    // set up timer
    fsu::Timer timer;
    fsu::Instant time;
    if (ticker)
    {
      timer.EventReset();
    }
    const size_t max = b.Size();
    size_t sqrt = 2;
    while (sqrt*sqrt < max)
    {
      ++sqrt;
    }
    // initialize b
    b.Set();
    // b.Unset(0);  // 0 is not prime
    // b.Unset(1);  // 1 is not prime

    // clear bits at multiples of all odd primes < sqrt(max)
    for (size_t i = 3; i < sqrt; i += 2)
    {
      if (b.Test(i)) // i is prime
      {
        if (ticker) std::cout << '.' << std::flush;
	// for (size_t j = i+i; j < max; j+= i)  // clear all multiples of i
        for (size_t j = i*i; j < max; j+= i)  // clear all multiples of i
        {
          b.Unset(j);
        }
      }
    }
    if (ticker)
    {
      time = timer.EventTime();
      std::cout << '\n';
      std::cout << " Sieve time: ";
      time.Write_seconds(std::cout,2);
      std::cout << " sec\n";
    }
  }  // end Sieve()
  // */

  /*
  void DumpValue(const fsu::BitVector& b, std::ostream& os = std::cout)
  {
    os << "  ";
    for (size_t i = 0; i < b.Size(); ++i)
      os << b.Test(i);
    os << '\n';

  }
  void DumpIndex(const fsu::BitVector& b, std::ostream& os = std::cout)
  {
    os << "  ";
    for (size_t i = 0; i < b.Size(); ++i)
      os << (2*i + 1)%10;
    os << '\n';
  }
  */

} // namespace fsu

#endif
