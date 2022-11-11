// William Yang
// double_hashing.h: A hash table with double hashing implementation.

#ifndef DOUBLE_HASHING_H
#define DOUBLE_HASHING_H

#include <vector>
#include <algorithm>
#include <functional>



// Class HashTableDouble:
// A hash table container with double hashing implementation.
template <typename HashedObj>
class HashTableDouble {
public:
    // Used to determine if hash entries are ACTIVE, EMPTY, or DELETED.
    enum EntryType { ACTIVE, EMPTY, DELETED };

    // Default constructor for hash table.
    // Size set to 101 by default, unless specified.
    // If size is specified and is prime, size is set to the desired number,
    // otherwise set the size as the next prime number after given number.
    // R value set to 89 by default, unless specified.
    explicit HashTableDouble(size_t size = 101, int r = 89) : array_(NextPrime(size)), r_value_(r) {
        MakeEmpty();
    }

    // Check if the hash table contains x.
    // Return true if x is found;
    // false otherwise.
    bool Contains(const HashedObj& x) {
        return IsActive(FindPos(x));
    }

    // Clear the hash table.
    void MakeEmpty() {
        current_size_ = 0;
        for (auto& entry : array_)
            entry.info_ = EMPTY;
    }

    // Insert x into the hash table.
    // Returns true if successful;
    // false otherwise.
    bool Insert(const HashedObj& x) {
        size_t current_pos = FindPos(x);
        if (IsActive(current_pos))  // Failed to insert.
            return false;

        // Insert x as active.
        array_[current_pos].element_ = x;
        array_[current_pos].info_ = ACTIVE;

        // Rehash.
        if (++current_size_ > array_.size() / 2)
            Rehash();
        return true;
    }

    // Move insert x into the hash table.
    // Returns true if successful;
    // false otherwise.
    bool Insert(HashedObj&& x) {
        size_t current_pos = FindPos(x);
        if (IsActive(current_pos))  // Failed to insert.
            return false;

        // Insert x as active.
        array_[current_pos] = std::move(x);
        array_[current_pos].info_ = ACTIVE;

        // Rehash.
        if (++current_size_ > array_.size() / 2)
            Rehash();

        return true;
    }

    // Removes x from the hash table.
    // Returns true if successful;
    // false otherwise.
    bool Remove(const HashedObj& x) {
        size_t current_pos = FindPos(x);
        if (!IsActive(current_pos)) // Failed to remove.
            return false;

        // Removed x, set as deleted.
        array_[current_pos].info_ = DELETED;
        return true;
    }

    // Returns the current size of the hash table.
    size_t Size() const {
        return current_size_;
    }

    // Returns the capacity of the hash table.
    size_t Capacity() const {
        return array_.capacity();
    }

    // Returns the load factor of the hash table.
    float LoadFactor() const {
        return (float)current_size_ / array_.capacity();
    }

    // Returns the total collisions counter.
    size_t TotalCollisions() const {
        return collisions_;
    }

    // Returns the average collisions of total collisions divided by the current size.
    float AverageCollisions() const {
        return (float)collisions_ / current_size_;
    }

    // Return probes used for the latest FindPos() function call.
    size_t ProbesUsed() const {
        return probes_used_;
    }

private:
    // Hash entry of the hash table.
    struct HashEntry {
        // The actual hashed element.
        HashedObj element_;
        // The current state of the hash entry (ACTIVE, EMPTY, DELETED).
        EntryType info_;

        // Constructor for hash entry.
        HashEntry(const HashedObj& e = HashedObj{}, EntryType i = EMPTY)
            :element_{ e }, info_{ i } { }

        // Move constructor for hash entry.
        HashEntry(HashedObj&& e, EntryType i = EMPTY)
            :element_{ std::move(e) }, info_{ i } {}
    };

    // The hash table.
    std::vector<HashEntry> array_;
    // Current size of table.
    size_t current_size_;
    // Prime r value used in the double hash function.
    int r_value_;
    // Total collisions counter.
    size_t collisions_ = 0;

    // Check if position in table is ACTIVE (holds an element).
    bool IsActive(size_t current_pos) const {
        return array_[current_pos].info_ == ACTIVE;
    }

    size_t probes_used_;   // To track number of probes used for find (most recent find).

    // Return the position of x.
    // Counts the number of probes used to find x.
    // Automatically resets the counter with each call.
    size_t FindPos(const HashedObj& x) {
        probes_used_ = 1;
        size_t current_pos = InternalHash(x);

        while (array_[current_pos].info_ != EMPTY &&
            array_[current_pos].element_ != x) {
            probes_used_++;
            collisions_++;
            current_pos += DoubleHash(x);  // Compute ith probe.
            if (current_pos >= array_.size())   // Wrap around table.
                current_pos -= array_.size();
        }
        return current_pos;
    }

    // Rehash hash table, table is getting full.
    void Rehash() {
        std::vector<HashEntry> old_array = array_;

        // Create new double-sized, empty table.
        array_.resize(NextPrime(2 * old_array.size()));
        for (auto& entry : array_)
            entry.info_ = EMPTY;

        // Copy table over.
        current_size_ = 0;
        for (auto& entry : old_array)
            if (entry.info_ == ACTIVE)
                Insert(std::move(entry.element_));
    }

    // Hash function.
    size_t InternalHash(const HashedObj& x) const {
        static std::hash<HashedObj> hf;
        return hf(x) % array_.size();
    }

    // Double hash function for double hashing.
    size_t DoubleHash(const HashedObj& x) const {
        static std::hash<HashedObj> hf;
        return r_value_ - (hf(x) % r_value_);
    }

    // Internal method to test if a positive number is prime.
    bool IsPrime(size_t n) {
        if (n == 2 || n == 3)
            return true;

        if (n == 1 || n % 2 == 0)
            return false;

        for (size_t i = 3; i * i <= n; i += 2)
            if (n % i == 0)
                return false;

        return true;
    }

    // Internal method to return a prime number at least as large as n.
    size_t NextPrime(size_t n) {
        if (n % 2 == 0)
            ++n;
        while (!IsPrime(n)) n += 2;
        return n;
    }
};

#endif	// DOUBLE_HASHING_H
