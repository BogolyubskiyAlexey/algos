#include "treap.h"

#include <iostream>
#include <set>
#include <cassert>

template<class TContainer1, class TContainer2>
void FullCheck(const TContainer1& a, const TContainer2& b) {
    assert(a.size() == b.size());
    for (auto& value : a) {
        assert(b.count(value));
    }
}

template<class TContainer1, class TContainer2>
void TestEqualData(int iterationsCount = 1000 * 1000, int fullCheckAfter = 10000) {
    using TData = typename TContainer1::value_type;
    TContainer1 a;
    TContainer2 b;

    std::mt19937 randomGenerator(1000);
    std::uniform_int_distribution<TData> range(
        std::numeric_limits<TData>::min(),
        std::numeric_limits<TData>::max()
    );

    int addRequests = 0;
    int removeRequests = 0;
    int added = 0;
    int removed = 0;
    for (int iteration = 0; iteration < iterationsCount; ++iteration) {
        TData value = range(randomGenerator);
        bool insert = range(randomGenerator) % 2 == 1;
        if (insert) {
            bool addedA = a.insert(value).second;
            bool addedB = b.insert(value);

            assert((addedA && addedB) || !(addedA || addedB));
            addRequests++;
            added += addedA ? 1 : 0;
        } else {
            size_t removedA = a.erase(value);
            size_t removedB = b.erase(value);

            assert(removedA == removedB);
            removeRequests++;
            removed += removedA;
        }

        assert(a.size() == b.size());
        assert(a.count(value) == b.count(value));
        if (iteration % fullCheckAfter == 0) {
            FullCheck(a, b);
        }
    }
    FullCheck(a, b);

    std::cerr << "=======================" << std::endl;
    std::cerr << "Test stat:" << std::endl;
    std::cerr << "Added: " << added << " / " << addRequests << std::endl;
    std::cerr << "Removed: " << removed << " / " << removeRequests << std::endl;
}

int main() {
    TestEqualData<std::set<int>, alexbogo::treap<int>>();
}