#pragma once

const int MAXLEN = 256;
typedef unsigned long long TValue;
typedef char TKey;

struct TNode
{
    int id = -1;
    int bit;

    TKey *key;
    TValue value;

    TNode *left;
    TNode *right;

    void Initialize(int b, TKey *k, TValue v, TNode *l, TNode *r)
    {
        bit = b;
        if (k)
        {
            key = new char[strlen(k) + 1];
            strcpy(key, k);
        }
        else
            key = k;
        value = v;
        left = l;
        right = r;
    }

    TNode()
    {
        Initialize(-1, 0, 0, this, this);
    }

    TNode(int b, TKey *k, TValue v)
    {
        Initialize(b, k, v, this, this);
    }

    TNode(int b, TKey *k, TValue v, TNode *l, TNode *r)
    {
        Initialize(b, k, v, l, r);
    }

    ~TNode()
    {
        delete [] key;
    }
};

