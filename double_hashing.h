#ifndef DOUBLE_HASHING_H
#define DOUBLE_HASHING_H

#include <vector>
#include <algorithm>
#include <functional>


template <typename HashedObj>
class HashTableDouble {
public:
    enum EntryType { ACTIVE, EMPTY, DELETED };

    explicit HashTableDouble(size_t size = 101, int r = 89) : array_(NextPrime(size)), r_value_(r)
    {
        MakeEmpty();
    }

    bool Contains(const HashedObj& x) {
        return IsActive(FindPos(x));
    }

    void MakeEmpty() {
        current_size_ = 0;
        for (auto& entry : array_)
            entry.info_ = EMPTY;
    }

    bool Insert(const HashedObj& x) {
        // Insert x as active
        size_t current_pos = FindPos(x);
        if (IsActive(current_pos))
            return false;

        array_[current_pos].element_ = x;
        array_[current_pos].info_ = ACTIVE;

        // Rehash; see Section 5.5
        if (++current_size_ > array_.size() / 2)
            Rehash();
        return true;
    }

    bool Insert(HashedObj&& x) {
        // Insert x as active
        size_t current_pos = FindPos(x);
        if (IsActive(current_pos))
            return false;

        array_[current_pos] = std::move(x);
        array_[current_pos].info_ = ACTIVE;

        // Rehash; see Section 5.5
        if (++current_size_ > array_.size() / 2)
            Rehash();

        return true;
    }

    bool Remove(const HashedObj& x) {
        size_t current_pos = FindPos(x);
        if (!IsActive(current_pos))
            return false;

        array_[current_pos].info_ = DELETED;
        return true;
    }

    size_t Size() const {
        return current_size_;
    }

    size_t Capacity() const {
        return array_.capacity();
    }

    float LoadFactor() const {
        return (float)current_size_ / array_.capacity();
    }

    size_t TotalCollisions() const {
        return collisions_;
    }

    float AverageCollisions() const {
        return (float)collisions_ / current_size_;
    }

    size_t ProbesUsed() const {
        return probes_used_;
    }

    // Return element x in the hash table.
    HashedObj GetElement(const HashedObj& x) const {
        if (!Contains(x))
        {
            cerr << "Error: element does not exist." << endl;
            exit(1);
        }
        return array_[FindPos(x)];
    }

private:
    struct HashEntry {
        HashedObj element_;
        EntryType info_;

        HashEntry(const HashedObj& e = HashedObj{}, EntryType i = EMPTY)
            :element_{ e }, info_{ i } { }

        HashEntry(HashedObj&& e, EntryType i = EMPTY)
            :element_{ std::move(e) }, info_{ i } {}
    };


    std::vector<HashEntry> array_;
    size_t current_size_;
    int r_value_;
    size_t collisions_ = 0;

    bool IsActive(size_t current_pos) const
    {
        return array_[current_pos].info_ == ACTIVE;
    }

    size_t probes_used_;   // To track number of probes used for find (most recent find).

    size_t FindPos(const HashedObj& x) {
        probes_used_ = 1;
        size_t current_pos = InternalHash(x);

        while (array_[current_pos].info_ != EMPTY &&
            array_[current_pos].element_ != x) {
            probes_used_++;
            collisions_++;
            current_pos += DoubleHash(x);  // Compute ith probe.
            if (current_pos >= array_.size())
                current_pos -= array_.size();
        }
        return current_pos;
    }

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

    size_t InternalHash(const HashedObj& x) const {
        static std::hash<HashedObj> hf;
        return hf(x) % array_.size();
    }

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
