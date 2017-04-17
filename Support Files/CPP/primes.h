/*
    primes.h
    12/21/2016
    Chris Lacher

    prototypes of functions 
     PrimeBelow, PrimeAbove, AllPrimesBelow, Sieve
    plus optimized versions of each

    all use BitVectors, and the first three all call sieve(), an
    implementation of the Sieve of Eratosthenes

    Copyright 2016, R.C. Lacher
*/

#ifndef _PRIMES_H
#define _PRIMES_H

#include <iostream>
#include <bitvect.h>

namespace fsu
{

  size_t PrimeBelow (size_t n, bool ticker = 0);
  // returns largest prime number <= n
  // or zero if input is too large

  size_t PrimeAbove (size_t n, bool ticker = 0);
  // returns smallest prime number >= n
  // or zero if input is too large

  void AllPrimesBelow (size_t n, std::ostream& os = std::cout, bool ticker = 0);
  // prints all primes less than n

  void Sieve (fsu::BitVector& b, bool ticker = 0);
  // The Sieve of Eratosthenes
  // pre:  b is a BitVector
  // post: for n < b.Size(), 
  //       n is prime iff 1 = b.Test(n)

  /* 
     These opt versions code the sieve process for odd integers only.
     First handle the special case 2, then only look at odd integers from 3.
     The BitVector then "codes" primeness of 2i+1 at index i. We already know
     that 2i is not prime, so no need to waste time and space on these 50% of
     the positive integers. 

     Need two Lemmas:

     Lemma 1: If k is odd then k*k + (2n+1)k is even.

     Proof: Factor k*k + (2n+1)k = k(k+2n+1) = A*B.
     If A = k is odd then B = k+2n+1 is even.
     If A = k is even then B = k+2n+1 is odd.
     Either way, A or B is even so AB is even.

     Lemma 2: if k is odd then k*k + 2nk is odd.

     Proof: k = 2i+1 odd => k*k = 4i^2 + 4i + 1 is odd, hence k*k + even is odd.
  */ 

  size_t PrimeBelowOpt     (size_t n, bool ticker);
  size_t PrimeAboveOpt     (size_t n, bool ticker);
  void   AllPrimesBelowOpt (size_t n, std::ostream& os, bool ticker);
  void   SieveOpt          (BitVector& b, bool ticker);
}   // namespace fsu

#endif
