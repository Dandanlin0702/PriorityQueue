/**********************************************************************
File:          QuadraticProbing.h
Author:        Dandan Lin
Date:          3/28/2017
**********************************************************************/

#ifndef QUADRATIC_PROBING_H
#define QUADRATIC_PROBING_H

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

// Quadratic probing implementation.
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

      bool Insert(const HashedKey & x, const HashedValue & p) {
         // Insert x as active
         size_t current_pos = FindPos(x);
         if (IsActive(current_pos))
            return false;

         array_[current_pos].key_ = x;
         array_[current_pos].info_ = ACTIVE;
         array_[current_pos].value_ = p;
         // Rehash; see Section 5.5
         if (++current_size_ > array_.size() / 2)
            Rehash();
         return true;
      }

      bool Insert(HashedKey && x, HashedValue && p) {
         // Insert x as active
         size_t current_pos = FindPos(x);
         if (IsActive(current_pos))
            return false;

         array_[current_pos] = std::move(x);
         array_[current_pos].info_ = ACTIVE;
         array_[current_pos].value_ = p;
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

         current_size_--;
         return true;
      }

      HashedValue Find(const HashedKey &key) const {
         int current_pos = FindPos(key);
         if (IsActive(current_pos)) {
            return array_[current_pos].value_;
         } else {
            return nullptr;
         }
      }

      bool isEmpty(const HashedKey & x) {
         
      }

      void Update(const HashedKey& key, const HashedValue& v) {
         int current_pos = FindPos(key);

         array_[current_pos].value_ = v;
      }
/*******************************************************************************
                  Five public function added for Assignment Part 1
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
/*******************************************************************************
*******************************************************************************/

private:
   struct HashEntry {
      HashedKey key_;
      EntryType info_;
      HashedValue value_;

      HashEntry(const HashedKey& e = HashedKey{}, EntryType i = EMPTY, const HashedValue & p = HashedValue{ }):key_{e}, info_{i}, value_{p} { }

      HashEntry(HashedKey && e, EntryType i = EMPTY, HashedValue && p = HashedValue{}):key_{std::move(e)}, info_{i}, value_{std::move(p)} { }
   };
/*******************************************************************************
         One Private Variables Added For Assignment Part 1
*******************************************************************************/
   mutable size_t number_of_collisions = 0;
/*******************************************************************************
*******************************************************************************/
   std::vector<HashEntry> array_;
   size_t current_size_;

   bool IsActive(size_t current_pos) const {
      return array_[current_pos].info_ == ACTIVE;
   }

   size_t FindPos(const HashedKey & x) const {
      size_t offset = 1;
      size_t current_pos = InternalHash(x);

      while (array_[current_pos].info_ != EMPTY && array_[current_pos].key_ != x) {
         number_of_collisions++; // Count the number of collisions

         current_pos += offset;  // Compute ith probe.

         offset += 2;
         if (current_pos >= array_.size()) {
            current_pos -= array_.size();
         }
      }
      return current_pos;
   }

   size_t FindPos(const HashedKey & x, size_t &number_of_probes) const {
      size_t offset = 1;
      size_t current_pos = InternalHash(x);

      while (array_[current_pos].info_ != EMPTY && array_[current_pos].key_ != x) {
         number_of_collisions++; // Count the number of collisions
         number_of_probes ++;
         current_pos += offset;  // Compute ith probe.

         offset += 2;
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

   size_t InternalHash(const HashedKey & x) const {
      static std::hash<HashedKey> hf;
      return hf(x) % array_.size( );
   }
};

#endif  // QUADRATIC_PROBING_H
