/*******************************************************************************
File:          BinomialQueue.h
Author:        Dandan Lin
Date Created:  04/11/2017

Description:  This file was originally provided by Professor. Stamos.
Following functions were modified/added for the completion of the assignment:
   BinomialQueue(item); - One parameter constructor
   Insert(x); - Added insertion to the hash table
   DeleteMin(x); - Added deletion from the hash table
   MakeEmpty(); - Empty theTrees and the Hash Table
   Remove(x); -  Remove function was craeted based on DeleteMin()function
   FindIndex(x); - Finds the index of the target item
*******************************************************************************/

#ifndef BINOMIAL_QUEUE_H
#define BINOMIAL_QUEUE_H

#include <iostream>
#include <vector>
#include "dsexceptions.h"
#include "DoubleHashing.h"

using namespace std;

template <typename Comparable>
class BinomialQueue {
   public:
      BinomialQueue() : theTrees(DEFAULT_TREES) {
         for(auto & root_ : theTrees)
            root_ = nullptr;
         currentSize = 0;
      }

      // This one parameter Constructor was modified
      BinomialQueue(const Comparable & item) : theTrees(1), currentSize{ 1 } {
         theTrees[0] = new BinomialNode{ item, nullptr, nullptr, nullptr };
      }

      BinomialQueue(const BinomialQueue & rhs)
      : theTrees(rhs.theTrees.size()),currentSize{ rhs.currentSize } {
         for(int i = 0; i < rhs.theTrees.size(); ++i)
            theTrees[i] = clone(rhs.theTrees[i]);
      }

      BinomialQueue(BinomialQueue && rhs)
      : theTrees{ std::move(rhs.theTrees) }, currentSize{ rhs.currentSize }
      { }

      ~BinomialQueue() { makeEmpty(); }


      /**
      * Deep copy.
      */
      BinomialQueue & operator=(const BinomialQueue & rhs) {
         BinomialQueue copy = rhs;
         std::swap(*this, copy);
         return *this;
      }

      /**
      * Move.
      */
      BinomialQueue & operator=(BinomialQueue && rhs) {
         std::swap(currentSize, rhs.currentSize);
         std::swap(theTrees, rhs.theTrees);

         return *this;
      }

      /**
      * Return true if empty; false otherwise.
      */
      bool isEmpty() const { return currentSize == 0; }

      /**
      * Returns minimum item.
      * Throws UnderflowException if empty.
      */
      const Comparable & findMin() const {
         if(isEmpty())
         throw UnderflowException{ };

         return theTrees[findMinIndex()]->element;
      }

      /**
      * Insert item x into the priority queue; allows duplicates.
      */
      void insert(const Comparable & x) {
         BinomialQueue oneItem{ x };
         // Insert the item to the hash table
         hash_table.Insert(x, oneItem.theTrees[0]);
         merge(oneItem);
      }

      /**
      * Insert item x into the priority queue; allows duplicates.
      */
      void insert(Comparable && x) {
         BinomialQueue oneItem{ std::move(x) };
         // Insert item to the hash table
         hash_table.Insert(std::move(x), oneItem.theTrees[0]);
         merge(oneItem);
      }

      /**
      * Insert item x into the priority queue; allows duplicates.
      */
      void newInsert(const Comparable & x) {
         BinomialQueue oneItem{ x };
         // Insert the item to the hash table
         hash_table.Insert(x, oneItem.theTrees[0]);
         newMerge(x);
      }

      /**
      * Insert item x into the priority queue; allows duplicates.
      */
      void newInsert(Comparable && x) {
         BinomialQueue oneItem{ std::move(x) };
         // Insert item to the hash table
         hash_table.Insert(std::move(x), oneItem.theTrees[0]);
         newMerge(x);
      }

      /**
      * Remove the smallest item from the priority queue.
      * Throws UnderflowException if empty.
      */
      void deleteMin() {
         Comparable x;
         deleteMin(x);
      }

      /**
      * Remove the minimum item and place it in minItem.
      * Throws UnderflowException if empty.
      */
      void deleteMin(Comparable & minItem) {
         if(isEmpty())
            throw UnderflowException{ };

         int minIndex = findMinIndex();
         minItem = theTrees[minIndex]->element;

         BinomialNode *oldRoot = theTrees[minIndex];
         BinomialNode *targetItem = oldRoot->leftChild;
         delete oldRoot;

         // Remove the target from the hash_table
         hash_table.Remove(minItem);

         // Construct H''
         BinomialQueue deletedQueue;
         deletedQueue.theTrees.resize(minIndex);
         deletedQueue.currentSize = (1 << minIndex) - 1;

         for(int j = minIndex - 1; j >= 0; --j) {
            deletedQueue.theTrees[j] = targetItem;
            targetItem = targetItem->nextSibling;
            deletedQueue.theTrees[j]->nextSibling = nullptr;
         }

         // Construct H'
         theTrees[minIndex] = nullptr;
         currentSize -= deletedQueue.currentSize + 1;

         merge(deletedQueue);
      }

      /**
      * Make the priority queue logically empty.
      */
      void makeEmpty() {
         currentSize = 0;
         for(auto & root_ : theTrees)
            makeEmpty(root_);
         // Clear the hash_table
         hash_table.MakeEmpty();
      }

      /**
      * Merge rhs into the priority queue.
      * rhs becomes empty. rhs must be different from this.
      * Exercise 6.35 needed to make this operation more efficient.
      */
      void merge(BinomialQueue & rhs) {
         if(this == &rhs)    // Avoid aliasing problems
            return;

         currentSize += rhs.currentSize;

         if(currentSize > capacity()) {
            int oldNumTrees = theTrees.size();
            int newNumTrees = max(theTrees.size(), rhs.theTrees.size()) + 1;
            theTrees.resize(newNumTrees);

            for(int i = oldNumTrees; i < newNumTrees; ++i)
               theTrees[i] = nullptr;
         }

         BinomialNode *carry = nullptr;
         for(int i = 0, j = 1; j <= currentSize; ++i, j *= 2) {
            BinomialNode *t1 = theTrees[i];
            BinomialNode *t2 = i < rhs.theTrees.size() ? rhs.theTrees[i] : nullptr;

            int whichCase = t1 == nullptr ? 0 : 1;
            whichCase += t2 == nullptr ? 0 : 2;
            whichCase += carry == nullptr ? 0 : 4;

            switch(whichCase) {
               case 0: /* No trees */
               case 1: /* Only this */
                  break;
               case 2: /* Only rhs */
                  theTrees[i] = t2;
                  rhs.theTrees[i] = nullptr;
                  break;
               case 4: /* Only carry */
                  theTrees[i] = carry;
                  carry = nullptr;
                  break;
               case 3: /* this and rhs */
                  carry = combineTrees(t1, t2);
                  theTrees[i] = rhs.theTrees[i] = nullptr;
                  break;
               case 5: /* this and carry */
                  carry = combineTrees(t1, carry);
                  theTrees[i] = nullptr;
                  break;
               case 6: /* rhs and carry */
                  carry = combineTrees(t2, carry);
                  rhs.theTrees[i] = nullptr;
                  break;
               case 7: /* All three */
                  theTrees[i] = carry;
                  carry = combineTrees(t1, t2);
                  rhs.theTrees[i] = nullptr;
                  break;
            }
         }

         // Update theTrees
         for (auto & root_ : theTrees) {
            if (root_ != nullptr) {
               root_->root = nullptr;
            }
         }

         for(auto & root_ : rhs.theTrees) {
            root_ = nullptr;
         }

         rhs.currentSize = 0;
      }

      void newMerge(const Comparable & x) {
         currentSize++;
         insert(x);

         // Update theTrees
         for (auto & root_ : theTrees) {
            if (root_ != nullptr) {
               root_->root = nullptr;
            }
         }
      }

      // This function was modified from deleteMin function
      bool remove(const Comparable & x) {
         if (!hash_table.Contains(x))
            return false;

         if (hash_table.IsEmpty(x)) {
            return false;
         }

         BinomialNode *temp = hash_table.Find(x);
         int index = findIndex(temp);

         // In priority queue, in order to remove an item
         // we need to percolate it up to the root and then delet the root
         BinomialNode *targetItem = percolateUp(temp);

         BinomialNode *oldRoot = targetItem;
         targetItem = targetItem->leftChild;
         oldRoot = nullptr;

         delete oldRoot;

         hash_table.Remove(x); //removes item from HashTable

         // Construct H''
         BinomialQueue deletedQueue;
         deletedQueue.theTrees.resize(index);
         deletedQueue.currentSize = (1 << index) - 1;

         for(int j = index - 1; j >= 0; --j) {
            deletedQueue.theTrees[j] = targetItem;
            targetItem = targetItem->nextSibling;
            deletedQueue.theTrees[j]->nextSibling = nullptr;
         }

         // Construct H'
         theTrees[index] = nullptr;
         currentSize -= deletedQueue.currentSize + 1;

         merge(deletedQueue);

         return true;
      }

      // Check if the target exists in the queue
      bool find_(const Comparable & e) {
         BinomialNode *ifFind = find(e);

         if (ifFind == nullptr) {
            return false;
         } else {
            return true;
         }
      }

   private:
      struct BinomialNode {
         Comparable    element;
         BinomialNode *root;
         BinomialNode *leftChild;
         BinomialNode *nextSibling;

         BinomialNode(const Comparable & e, BinomialNode *p, BinomialNode *lt, BinomialNode *rt)
         : element{ e }, root{ p }, leftChild{ lt }, nextSibling{ rt } { }

         BinomialNode(Comparable && e, BinomialNode *lt, BinomialNode *p, BinomialNode *rt)
         : element{ std::move(e) }, root{ p }, leftChild{ lt }, nextSibling{ rt } { }
      };

      const static int DEFAULT_TREES = 1;
      HashTable<int,BinomialNode*> hash_table;
      vector<BinomialNode *> theTrees;  // An array of tree roots
      int currentSize;                  // Number of items in the priority queue

      /**
      * Find index of tree containing the smallest item in the priority queue.
      * The priority queue must not be empty.
      * Return the index of tree containing the smallest item.
      */
      int findMinIndex() const {
         int i;
         int minIndex;

         for(i = 0; theTrees[i] == nullptr; ++i)
         ;

         for(minIndex = i; i < theTrees.size(); ++i)
            if(theTrees[i] != nullptr && theTrees[i]->element < theTrees[minIndex]->element)
               minIndex = i;
         return minIndex;
      }

      /**
      * Find index of tree containing the smallest item in the priority queue.
      * The priority queue must not be empty.
      * Return the index of tree containing the smallest item.
      */
      int findIndex(BinomialNode * &t) const {
         int index;
         BinomialNode *root_ = t;
         while(root_->root != nullptr) {
            root_ = root_->root;
         }

         for(int i = 0; i < theTrees.size(); ++i)
            if(theTrees[i] != nullptr)
               if(theTrees[i] == root_)
                  index = i;

         return index;
      }

      /**
      * Return the capacity.
      */
      int capacity() const { return (1 << theTrees.size()) - 1; }

      /**
      * Return the result of merging equal-sized t1 and t2.
      */
      BinomialNode * combineTrees(BinomialNode *t1, BinomialNode *t2) {
         if(t2->element < t1->element)
            return combineTrees(t2, t1);

         t2->nextSibling = t1->leftChild;
         t1->leftChild = t2;
         t2->root = t1;

         return t1;
      }


      /**
      * Make a binomial tree logically empty, and free memory.
      */
      void makeEmpty(BinomialNode * & t) {
         if(t != nullptr) {
            makeEmpty(t->leftChild);
            makeEmpty(t->nextSibling);
            delete t;
            t = nullptr;
         }
      }

      /**
      * Internal method to clone subtree.
      */
      BinomialNode * clone(BinomialNode * t) const {
         if(t == nullptr)
            return nullptr;
         else
            return new BinomialNode{ t->element, clone(t->root), clone(t->leftChild), clone(t->nextSibling) };
      }

      /**
      Percolates to the root_ of the tree by changing elements rather than pointers.
      */
      BinomialNode * percolateUp(BinomialNode * & t) {
         if (t->root == nullptr)
            return t;

            Comparable oldElement = t->element;
            // Update the hash tabel whiling percolateUp
            hash_table.Update(t->element, t->root);
            hash_table.Update(t->root->element, t);

            t->element = t->root->element;
            t->root->element = oldElement;

            return percolateUp(t->root);
         }

      // Return true if target is found in the hash table
      BinomialNode * find(const Comparable & x) {
         return hash_table.Find(x);
      }
   };

#endif
