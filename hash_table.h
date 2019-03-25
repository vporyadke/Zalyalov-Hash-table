#include <iostream>
#include <list>
#include <stdexcept>
#include <vector>


template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    typedef typename std::list<std::pair<const KeyType, ValueType>>::iterator iterator;
    typedef typename std::list<std::pair<const KeyType, ValueType>>::const_iterator const_iterator;

private:
    std::list<std::pair<const KeyType, ValueType>> values;


    size_t sz = 100000;
    std::vector<std::list<iterator>> table;
    Hash hasher;

    void copy_to_table() {
        for (iterator it = values.begin(); it != values.end(); ++it) {
            table[hasher(it->first) % sz].push_back(it);
        }
    }

    void realloc() {
        sz *= 2;
        table = std::vector<std::list<iterator>>(sz);
        copy_to_table();
    }

public:
    HashMap(Hash h = Hash{}) : table(sz), hasher(h) {}

    iterator begin() {
        return values.begin();
    }

    iterator end() {
        return values.end();
    }

    const_iterator begin() const {
        return values.cbegin();
    }

    const_iterator end() const {
        return values.cend();
    }

    iterator find(KeyType key) {
        size_t h = hasher(key) % sz;
        for (iterator& i : table[h]) {
            if (i->first == key) {
                return i;
            }
        }
        return end();
    }

    const_iterator find(KeyType key) const {
        size_t h = hasher(key);
        for (const iterator& i : table[h]) {
            if (i->first == key) {
                return i;
            }
        }
        return end();
    }


    void insert(std::pair<const KeyType, ValueType> p) {
        if (find(p.first) != end()) {
            return;
        }
        values.push_back(p);
        table[hasher(p.first) % sz].push_back(--end());
    }

    template<typename Iterator>
    HashMap(Iterator begin, Iterator end, Hash h = Hash{}) : values(begin, end), table(sz), hasher(h) {
        copy_to_table();
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> elems, Hash h = Hash{}) : values(elems.begin(), elems.end()), table(sz), hasher(h) {
        copy_to_table();
    }

    size_t size() const {
        return values.size();
    }

    bool empty() const {
        return (values.size() == 0);
    }

    Hash hash_function() const {
        return hasher;
    }

    void erase(KeyType key) {
        size_t h = hasher(key) % sz;
        for (auto it = table[h].begin(); it != table[h].end(); ++it) {
            if ((*it)->first == key) {
                values.erase(*it);
                table[h].erase(it);
                return;
            }
        }
    }

    ValueType& operator[](KeyType key) {
        iterator val = find(key);
        if (val != end()) {
            return val->second;
        }
        values.push_back({key, ValueType()});
        size_t h = hasher(key) % sz;
        table[h].push_back(--values.end());
        return values.back().second;
    }

    const ValueType& at(KeyType key) const {
        const_iterator val = find(key);
        if (val != end()) {
            return val->second;
        }
        throw std::out_of_range("HashMap key not found");
    }

    void clear() {
        for (auto& p : values) {
            table[hasher(p.first) % sz].clear();
        }
        values.clear();
    }

    HashMap& operator=(const HashMap& other) {
        if (values == other.values) {
            return *this;
        }
        clear();
        values = std::list<std::pair<const KeyType, ValueType>>(other.begin(), other.end());
        sz = other.sz;
        table.resize(sz);
        hasher = other.hasher;
        copy_to_table();
        return *this;
    }


    HashMap(const HashMap& other) : values(other.begin(), other.end()), sz(other.sz), table(sz), hasher(other.hasher) {
        copy_to_table();
    }


};
