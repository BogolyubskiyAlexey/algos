#include <random>
#include <limits>
#include <optional>

namespace {
    template <class ValueType>
    ValueType next_rand(ValueType left, ValueType right) {
        static std::mt19937 randomGenerator(0);
        std::uniform_int_distribution<ValueType> range(left, right);
        return range(randomGenerator);
    }
}

namespace alexbogo {
    template <class KeyType, class LevelType=uint64_t>
    class treap {
    public:
        using value_type = KeyType;
        using level_type = LevelType;

    protected:
        struct node;
        using node_ptr = node*;

        struct node {
            node(const value_type& value, level_type level)
                : Value(value)
                , Level(level)
                , Left(nullptr)
                , Right(nullptr)
            {}

            value_type Value;
            level_type Level;

            node_ptr Left;
            node_ptr Right;
        };

    private:
        node_ptr merge(node_ptr a, node_ptr b);
        void split(node_ptr node, const value_type& value, node_ptr& a, node_ptr& b);

        node_ptr search(node_ptr tree, const value_type& value) const;
        node_ptr erase(node_ptr tree, const value_type& value, bool& erased);
        node_ptr lower_bound(node_ptr tree, const value_type& value);
        void clear(node_ptr tree);

    public:
        bool insert(const value_type& value);
        bool erase(const value_type& value);
        size_t size() const;
        size_t count(const value_type& value) const;
        std::optional<value_type> lower_bound(const value_type& value) const;
        void clear();
        treap();
        ~treap();

    private:
        node_ptr Root;
        size_t Size;
    };

    template<class value_type, class level_type>
    typename treap<value_type, level_type>::node_ptr treap<value_type, level_type>::merge(
        node_ptr a,
        node_ptr b
    ) {
        if (!a && !b) {
            return nullptr;
        }
        if (!a || !b) {
            return a ? a : b;
        }
        if (a->Level > b->Level) {
            a->Right = merge(a->Right, b);
            return a;
        }
        b->Left = merge(a, b->Left);
        return b;
    }

    template<class value_type, class level_type>
    void treap<value_type, level_type>::split(node_ptr node, const value_type& value, node_ptr& a, node_ptr& b) {
        if (!node) {
            a = b = nullptr;
            return;
        }
        if (node->Value <= value) {
            split(node->Right, value, node->Right, b);
            a = node;
        } else {
            split(node->Left, value, a, node->Left);
            b = node;
        }
    }

    template<class value_type, class level_type>
    typename treap<value_type, level_type>::node_ptr treap<value_type, level_type>::search(node_ptr tree, const value_type& value) const {
        if (!tree) {
            return nullptr;
        }
        if (tree->Value == value) {
            return tree;
        }
        if (tree->Value < value) {
            return search(tree->Right, value);
        }
        return search(tree->Left, value);
    }

    template<class value_type, class level_type>
    typename treap<value_type, level_type>::node_ptr treap<value_type, level_type>::erase(node_ptr tree, const value_type& value, bool& erased) {
        if (!tree) {
            return nullptr;
        }
        if (tree->Value == value) {
            node_ptr tmp = merge(tree->Left, tree->Right);
            delete tree;
            erased = true;
            return tmp;
        }
        if (tree->Value < value) {
            tree->Right = erase(tree->Right, value, erased);
        } else {
            tree->Left = erase(tree->Left, value, erased);
        }
        return tree;
    }

    template<class value_type, class level_type>
    typename treap<value_type, level_type>::node_ptr treap<value_type, level_type>::lower_bound(node_ptr tree, const value_type& value) {
        if (!tree) {
            return nullptr;
        }
        if (tree->Value == value) {
            return tree;
        }
        if (tree->Value < value) {
            return lower_bound(tree->Right, value);
        }
        node_ptr node = lower_bound(tree->Left, value);
        return node ? node : tree;
    }

    template<class value_type, class level_type>
    void treap<value_type, level_type>::clear(node_ptr tree) {
        if (!tree) {
            return;
        }
        clear(tree->Left);
        clear(tree->Right);
        delete tree;
    }

    template<class value_type, class level_type>
    bool treap<value_type, level_type>::insert(const value_type& value) {
        if (search(Root, value)) {
            return false;
        }
        level_type level = next_rand<level_type>(0, std::numeric_limits<level_type>::max());
        node_ptr node = new struct node(value, level);
        node_ptr a = nullptr;
        node_ptr b = nullptr;
        split(Root, value, a, b);
        Root = merge(a, merge(node, b));
        Size++;
        return true;
    }
        
    template<class value_type, class level_type>
    bool treap<value_type, level_type>::erase(const value_type& value) {
        bool erased = false;
        Root = erase(Root, value, erased);
        if (erased) {
            Size--;
        }
        return erased;
    }

    template<class value_type, class level_type>
    size_t treap<value_type, level_type>::size() const {
        return Size;
    }

    template<class value_type, class level_type>
    size_t treap<value_type, level_type>::count(const value_type& value) const {
        return search(Root, value) ? 1 : 0;
    }

    template<class value_type, class level_type>
    std::optional<value_type> treap<value_type, level_type>::lower_bound(const value_type& value) const {
        node_ptr node = lower_bound(Root, value);
        if (node) {        
            return &node->Value;
        }
        return {};
    }

    template<class value_type, class level_type>
    void treap<value_type, level_type>::clear() {
        clear(Root);
        Root = nullptr;
        Size = 0;
    }

    template<class value_type, class level_type>
    treap<value_type, level_type>::treap()
        : Root(nullptr)
        , Size(0)
    {}

    template<class value_type, class level_type>
    treap<value_type, level_type>::~treap() {
        clear();
    }
}
