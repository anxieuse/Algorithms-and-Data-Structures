#pragma once

#include "Node.hpp"

struct TTrie
{
    TNode *root;
    int size;

    TTrie()
    {
        root = new TNode();
        root->key = nullptr;
        size = 0;
    }

    void DestructR(TNode *node)
    {
        if (node->left->bit > node->bit)
            DestructR(node->left);
        if (node->right->bit > node->bit)
            DestructR(node->right);
        delete node;
    }

    ~TTrie()
    {
        DestructR(root);
    }

    TNode *Find(TKey *key)
    {
        TNode *p = root;
        TNode *q = root->left;

        while (p->bit < q->bit)
        {
            p = q;
            q = (GetBit(key, q->bit) ? q->right : q->left);
        }

        if (!Equal(key, q->key))
            return 0;

        return q;
    }

    TNode *Insert(TKey *key, TValue value)
    {
        TNode *p = root;
        TNode *q = root->left;
        while (p->bit < q->bit)
        {
            p = q;
            q = (GetBit(key, q->bit) ? q->right : q->left);
        }

        if (Equal(key, q->key))
            return 0;

        int lBitPos = FirstDifBit(key, q->key);

        p = root;
        TNode *x = root->left;

        while (p->bit < x->bit && x->bit < lBitPos)
        {
            p = x;
            x = (GetBit(key, x->bit) ? x->right : x->left);
        }

        try
        {
            q = new TNode();
        }
        catch (const std::bad_alloc &e)
        {
            std::cout << "ERROR: fail to allocate the requested storage space\n";
            return 0;
        }

        q->Initialize(lBitPos, key, value,
                      (GetBit(key, lBitPos) ? x : q),
                      (GetBit(key, lBitPos) ? q : x));

        if (GetBit(key, p->bit))
            p->right = q;
        else
            p->left = q;

        size++;
        return q;
    }

    void KVCopy(TNode *src, TNode *dest)
    {
        if (strlen(dest->key) < strlen(src->key))
        {
            delete[] dest->key;
            dest->key = new char[strlen(src->key) + 1];
        }
        strcpy(dest->key, src->key);

        dest->value = src->value;
    }

    bool Delete(TKey *k)
    {
        // прадед удаляемого узла pp, родитель p и сам сын t (сына предстоит удалить)
        TNode *p, *t, *pp = 0;

        p = root;
        t = (p->left);

        // найдем pp, p и t
        while (p->bit < t->bit)
        {
            pp = p;
            p = t;
            t = (GetBit(k, t->bit) ? t->right : t->left);
        }

        // если ключа искомого-то и нет -- выходим
        if (!Equal(k, t->key))
            return false;

        TNode *x, *r;
        char *key;

        // если p == t, то у t есть селфпоинтер. в таком случае достаточно лишь
        // переподвесить к родителю (pp) "реальный" указатель t, который не селфпоинтер
        if (p != t)
        {
            // иначе же, кладем ключ и знач. p в t, чтобы далее удалять именно p, а не t
            KVCopy(p, t);

            key = p->key;
            r = p;
            x = (GetBit(key, p->bit) ? p->right : p->left);

            // ищем того, кто на p бекпоинтерит (будет лежать в r; а х, по сути, будет в точности равняться p)
            while (r->bit < x->bit)
            {
                r = x;
                x = (GetBit(key, x->bit) ? x->right : x->left);
            }

            // и вместо бекпоинтера на p, будем бекпоинтерить на t
            if (GetBit(key, r->bit))
                r->right = t;
            else
                r->left = t;
        }

        // остается подвесить к родителю p (pp) "реальный" указатель p, который не селфпоинтер
        TNode *ch = (GetBit(k, p->bit) ? p->left : p->right);
        if (GetBit(k, pp->bit))
            pp->right = ch;
        else
            pp->left = ch;

        // и беззаботно удалить p: больше на него никто не указывает, ведь мы избавились
        // и от бекпоинтера на него, и от родительского (pp) указателей сверху
        delete p;

        size--;

        return true;
    }

    void Save(std::ofstream &file)
    {
        // подаем размер дерева
        file.write((const char *)&(size), sizeof(int));

        // пронумеровка узлов, инициализация массива указателей
        int index = 0;
        TNode **nodes;
        try
        {
            nodes = new TNode *[size + 1];
        }
        catch (const std::bad_alloc &e)
        {
            std::cout << "ERROR: fail to allocate the requested storage space\n";
            return;
        }
        enumerate(root, nodes, index);

        // теперь просто последовательно (как при обходе в enumerate)
        // подаем всю инфу об узлах, но вместо указателей left/right подаем
        // айди узлов (каковы они были при обходе в enumerate) left/right
        TNode *node;
        for (int i = 0; i < (size + 1); ++i)
        {
            node = nodes[i];
            file.write((const char *)&(node->value), sizeof(TValue));
            file.write((const char *)&(node->bit), sizeof(int));
            int len = node->key ? strlen(node->key) : 0;
            file.write((const char *)&(len), sizeof(int));
            file.write(node->key, sizeof(char) * len);
            file.write((const char *)&(node->left->id), sizeof(int));
            file.write((const char *)&(node->right->id), sizeof(int));
        }
        delete[] nodes;
    }

    void enumerate(TNode *node, TNode **nodes, int &index)
    {
        // важно, что index передается по ссылке: айди узлов не будут повторяться
        node->id = index;
        nodes[index] = node;
        ++index;
        if (node->left->bit > node->bit)
        {
            enumerate(node->left, nodes, index);
        }
        if (node->right->bit > node->bit)
        {
            enumerate(node->right, nodes, index);
        }
    }

    void Load(std::ifstream &file)
    {
        // считываем размер
        int n;
        file.read((char *)&n, sizeof(int));
        size = n;
        // если он нуль - выходим
        if (!size)
            return;

        TNode **nodes = new TNode *[size + 1];
        // рут уже инициализировался, когда мы пишем создали new Trie()
        // незачем этого делать повторно
        nodes[0] = root;
        for (int i = 1; i < (size + 1); ++i)
            // а вот прочие узлы надо инитнуть
            nodes[i] = new TNode();

        // поля узлов, которые нам предстоит считывать
        int bit;
        int len;
        TKey *key = 0;
        TValue value;
        int idLeft, idRight;

        for (int i = 0; i < (size + 1); ++i)
        {
            file.read((char *)&(value), sizeof(TValue));
            file.read((char *)&(bit), sizeof(int));
            file.read((char *)&(len), sizeof(int));
            if (len)
            {
                key = new char[len + 1];
                key[len] = 0;
            }
            file.read(key, len);
            // поскольку считываем в том же порядке, что и писали в Load-e
            // айди узлов-сыновей будут сохранять свой порядок, и дерево соберется таким же
            file.read((char *)&(idLeft), sizeof(int));
            file.read((char *)&(idRight), sizeof(int));
            nodes[i]->Initialize(bit, key, value, nodes[idLeft], nodes[idRight]);
            delete[] key;
        }

        delete[] nodes;

        return;
    }

#define safeKey(node) (node->key ? node->key : "root")

    void PrintDefinitions(TNode *node, std::ofstream &out)
    {
        out << ' ' << safeKey(node) << "[label=\"" << safeKey(node) << ", " << node->bit << "\"];\n";
        if (node->left->bit > node->bit)
            PrintDefinitions(node->left, out);
        if (node->right->bit > node->bit)
            PrintDefinitions(node->right, out);
    }

    void PrintRelations(TNode *node, std::ofstream &out)
    {
        if (node->left->bit > node->bit)
        {
            out << ' ' << safeKey(node) << "->" << safeKey(node->left) << "[label=\"l\"];\n";
            PrintRelations(node->left, out);
        }
        else
        {
            out << ' ' << safeKey(node) << "->" << safeKey(node->left) << "[label=\"l\"];\n";
        }
        if (node->right->bit > node->bit)
        {
            out << ' ' << safeKey(node) << "->" << safeKey(node->right) << "[label=\"r\"];\n";
            PrintRelations(node->right, out);
        }
        else
        {
            out << ' ' << safeKey(node) << "->" << safeKey(node->right) << "[label=\"r\"];\n";
        }
    }
};