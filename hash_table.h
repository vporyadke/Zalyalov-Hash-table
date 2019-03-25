#ifndef HASH_MAP
#define HASH_MAP

#include <list>
#include <stdexcept>
#include <vector>


namespace HashMapNamespace {
    template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
    class HashMap {
    public:
        typedef typename std::list<std::pair<const KeyType, ValueType>>::iterator iterator;
        typedef typename std::list<std::pair<const KeyType, ValueType>>::const_iterator const_iterator;

    private:
        static constexpr double MAX_LOAD_FACTOR = 0.75;
        static constexpr double MIN_LOAD_FACTOR = MAX_LOAD_FACTOR * 0.25;
        static constexpr size_t START_SIZE = 32;

        std::list<std::pair<const KeyType, ValueType>> values;
        size_t sz = START_SIZE;
        std::vector<std::list<iterator>> table;
        Hash hasher;

        void copy_to_table() {
            for (iterator it = values.begin(); it != values.end(); ++it) {
                table[hasher(it->first) % sz].push_back(it);
            }
        }

        void realloc(size_t new_size) {
            sz = new_size;
            table = std::vector<std::list<iterator>>(sz);
            copy_to_table();
        }

    public:
        explicit HashMap(Hash h = Hash{}) : table(sz), hasher(h) {}

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
            if (values.size() > sz * MAX_LOAD_FACTOR) {
                realloc(sz * 2);
            }
        }

        template<typename InputIterator>
        HashMap(InputIterator begin, InputIterator end, Hash h = Hash{}) : values(begin, end), hasher(h) {
            while (values.size() > sz * MAX_LOAD_FACTOR) {
                sz *= 2;
            }
            table.resize(sz);
            copy_to_table();
        }

        HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> elems, Hash h = Hash{}) : HashMap(elems.begin(), elems.end(), h) {}

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

        void shrink_to_fit() {
            size_t new_size = sz;
            while (new_size > 1 && values.size() < new_size * MIN_LOAD_FACTOR) {
                new_size /= 2;
            }
            if (new_size != sz) {
                realloc(new_size);
                return;
            }
            for (auto& i : table) {
                i.clear();
            }
        }

    };

}

#endif // HASH_MAP
