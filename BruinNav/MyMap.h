// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#include <unistd.h>
#include <iostream>
using namespace std;


template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap()
    {
        m_root = NULL;
        m_size = 0;
    }
	~MyMap()
    {
        clear();
    }
	void clear()
    {
        freeTree(m_root);
        m_root = NULL;
        m_size = 0;
    }
	int size() const
    {
        return m_size;
    }
	void associate(const KeyType& key, const ValueType& value)
    {
        ValueType* ptr = find(key);
        if(ptr != nullptr)
            *ptr = value;
        else
            insert(key, value);
    }

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const
    {
        Node* cur = m_root;
        while( cur != NULL)
        {
            if (cur->key == key)
                return &(cur->value);
            else if (cur->key < key)
                cur = cur->right;
            else
                cur = cur->left;
        }
        return nullptr;
    }

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
    struct Node
    {
        Node(const KeyType& akey, const ValueType& avalue)
        :key(akey)
        {
            //key = akey;
            value = avalue;
            left = right = NULL;
        }
        const KeyType key;
        ValueType value;
        Node* left;
        Node* right;
    };
    Node* m_root;
    int m_size;
    void freeTree(Node* cur)
    {
        if (cur == NULL)
            return;
        freeTree(cur->left);
        freeTree(cur->right);
        delete cur;
    }
    void insert(const KeyType& key, const ValueType& value)
    {
        if(m_root == nullptr)
        {
            m_root = new Node(key, value);
            m_size++;
            return;
        }
        Node* cur = m_root;
        for(;;)
        {
            if(cur->key == key) return;
            else if(cur->key < key)
            {
                if(cur->right == nullptr){
                    cur->right = new Node(key, value);
                    m_size++;
                    return;
                }
                else cur = cur->right;
            }
            else if(cur->key > key)
            {
                if(cur->left == nullptr)
                {
                    cur->left = new Node(key, value);
                    m_size++;
                    return;
                }
                else cur = cur->left;
            }
        }
    }
};
