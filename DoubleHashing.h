/**********************************************************************
***********************************************************************
File:          DoubleHashing.h
Author:        Dandan Lin
Date:          04/11/2017

Description: This DoubleHashing.h file was taken from Assignment 3.
             Following functions are modified for completion purpose.

             Insert(x, v); Remove(x); Find(x)
**********************************************************************/

#ifndef DOUBLE_HASHING_H
#define DOUBLE_HASHING_H

#include <vector>
#include <algorithm>
#include <functional>

namespace {
   // Seperate from the global functions
   // Internal method to test if a positive number is prime.
   bool IsPrime(size_t n) {
      if( n == 2 || n == 3 )
         return true;

      if( n == 1 || n % 2 == 0 )
         return false;

      for( int i = 3; i * i <= n; i += 2 )
         if( n % i == 0 )
            return false;

      return true;
   }

   // Internal method to return a prime number at least as large as n.
   int NextPrime(size_t n) {
      if (n % 2 == 0)
         ++n;
      while (!IsPrime(n)) n += 2;
         return n;
   }
}  // namespace

// Double Hashing implementation.
template <typename HashedKey, typename HashedValue>
class HashTable {
   public:
      enum EntryType {ACTIVE, EMPTY, DELETED};

      explicit HashTable(size_t size = 101) : array_(NextPrime(size)) { MakeEmpty(); }

      bool Contains(const HashedKey & x, size_t &number_of_probes) const {
         return IsActive(FindPos(x, number_of_probes));
      }

      bool Contains(const HashedKey & x) const {
         return IsActive(FindPos(x));
      }

      void MakeEmpty() {
         current_size_ = 0;
         for (auto &entry : array_)
            entry.info_ = EMPTY;
      }

      bool Insert(const HashedKey & x, const HashedValue & v) {
         // Insert x as active
         size_t current_pos = FindPos(x);
         if (IsActive(current_pos))
            return false;

         array_[current_pos].key_ = x;
         array_[current_pos].value_ = v;
         array_[current_pos].info_ = ACTIVE;

         // Rehash; see Section 5.5
         if (++current_size_ > array_.size() / 2)
            Rehash();
         return true;
      }

      bool Insert(HashedKey && x, HashedValue && v) {
         // Insert x as active
         size_t current_pos = FindPos(x);
         if (IsActive(current_pos))
            return false;

         array_[current_pos].key_ = std::move(x);
         array_[current_pos].value_ = v;
         array_[current_pos].info_ = ACTIVE;

         // Rehash; see Section 5.5
         if (++current_size_ > array_.size() / 2)
            Rehash();

         return true;
      }

      bool Remove(const HashedKey & x) {
         size_t current_pos = FindPos(x);
         if (!IsActive(current_pos))
            return false;

         array_[current_pos].value_ = nullptr;
         array_[current_pos].info_ = DELETED;
            return true;
      }

      HashedValue Find(const HashedKey & x) const {
         int current_pos = FindPos(x);

         if (!IsActive(current_pos)) {
            return nullptr;
         } else {
            return array_[current_pos].value_;
         }
      }

      const bool IsEmpty(const HashedKey & x) {
         size_t current_pos = FindPos(x);

         return array_[current_pos].info_ == EMPTY;
      }

      void Update(const HashedKey & x, const HashedValue & v) {
         int current_pos = FindPos(x);

         array_[current_pos].value_ = v;
      }
   /*******************************************************************************
         Five public functions added to compute:
            NumberOfCollisions
            NumberOfProbes
            NumberOfItems
            LoadFactor
            TableSize
   *******************************************************************************/
      size_t NumberOfCollisions() {
         return number_of_collisions;
      }

      size_t NumberOfItems() {
         return current_size_;
      }

      size_t TableSize() {
         return array_.capacity();
      }

      double LoadFactor() {
         return static_cast<double>(NumberOfItems())/static_cast<double>(TableSize());
      }
/*
   size_t NumberOfProbes() {
      return number_of_probes;
   }

   */
/*******************************************************************************
*******************************************************************************/

   private:
      struct HashEntry {
         HashedKey key_;
         HashedValue value_;
         EntryType info_;

         HashEntry(const HashedKey& e = HashedKey{}, const HashedValue & v = HashedValue{}, EntryType i = EMPTY):key_{e}, value_{v}, info_{i} { }
         HashEntry(HashedKey && e, HashedValue && v, EntryType i = EMPTY):key_{std::move(e)}, value_{std::move(v)}, info_{ i } {}
      };

/*******************************************************************************
 Two Private members added for computeing NumberOfCollisions and NumberOfProbes
*******************************************************************************/
      mutable size_t number_of_collisions = 0;
      //mutable size_t number_of_probes = 1;
/*******************************************************************************
*******************************************************************************/

      std::vector<HashEntry> array_;
      size_t current_size_;

      unsigned R = 71;

      bool IsActive(size_t current_pos) const {
         return array_[current_pos].info_ == ACTIVE;
      }

      size_t FindPos(const HashedKey & x) const {
         size_t offset = 1;
         size_t current_pos = InternalHashFirst(x);

         while (array_[current_pos].info_ != EMPTY &&
            array_[current_pos].key_ != x) {
               number_of_collisions++; // Count the number of collisions

               current_pos += offset;  // Compute ith probe.
               current_pos = (current_pos + (offset*InternalHashSecond(x)) % array_.capacity());

               offset ++;;
               if (current_pos >= array_.size()) {
                  current_pos -= array_.size();
               }
            }
            return current_pos;
      }

      size_t FindPos(const HashedKey & x, size_t& number_of_probes) const {
         size_t offset = 1;
         size_t current_pos = InternalHashFirst(x);

         while (array_[current_pos].info_ != EMPTY &&
            array_[current_pos].key_ != x) {
               number_of_collisions++; // Count the number of collisions
               number_of_probes ++;

               current_pos += offset;  // Compute ith probe.
               current_pos = (current_pos + (offset*InternalHashSecond(x)) % array_.capacity());

               offset ++;;
               if (current_pos >= array_.size()) {
                  current_pos -= array_.size();
               }
            }
            return current_pos;
      }

      void Rehash() {
         std::vector<HashEntry> old_array = array_;

         // Create new double-sized, empty table.
         array_.resize(NextPrime(2 * old_array.size()));
         for (auto & entry : array_) {
            entry.info_ = EMPTY;
         }

         // Copy table over.
         current_size_ = 0;
         for (auto & entry :old_array) {
            if (entry.info_ == ACTIVE) {
               Insert(std::move(entry.key_), std::move(entry.value_));
            }
         }
      }

      // Modified InternalHash Function
      size_t InternalHashFirst(const HashedKey & x) const {
         static std::hash<HashedKey> hf;
         return hf(x) % array_.size( );
      }
      // Added InternalHashSecond Function to compute DoubelHashing
      size_t InternalHashSecond(const HashedKey & x) const {
         static std::hash<HashedKey> hf;
         return R - (hf(x) % R);
      }
};

   #endif
