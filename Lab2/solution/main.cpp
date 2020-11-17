#include <iostream>
#include <fstream>
#include <cstring>

#include "Additional.hpp"
#include "Trie.hpp"

int main()
{
    // оптимизация
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);

    // потоки i/o -- файлы для сериализации/десериализации, хранения/считывания конструкции патриции
    std::ofstream fout;
    std::ofstream dotout;
    std::ifstream fin;

    char input[MAXLEN];
    TValue value;

    // основное дерево trie
    TTrie *trie;
    try
    {
        trie = new TTrie();
    }
    catch (const std::bad_alloc &e)
    {
        std::cout << "ERROR: fail to allocate the requested storage space\n";
        exit(0);
    }

    TNode *node;

    while ((std::cin >> input))
    {
        if (!std::strcmp(input, "+"))
        {
            std::cin >> input;
            Lowercase(input);
            std::cin >> value;

            std::cout << (trie->Insert(input, value) ? "OK" : "Exist");
            std::cout << '\n';
        }
        else if (!std::strcmp(input, "-"))
        {
            std::cin >> input;
            Lowercase(input);

            std::cout << (trie->Delete(input) ? "OK" : "NoSuchWord");
            std::cout << '\n';
        }
        else if (!std::strcmp(input, "!"))
        {
            std::cin >> input;
            if (!std::strcmp(input, "Save"))
            {
                std::cin >> input;
                fout.open(input, std::ios::out | std::ios::binary | std::ios::trunc);
                if (!fout.is_open()) {
                    std::cout << "ERROR: can't create file\n";
                    continue;
                }

                trie->Save(fout);
                std::cout << "OK\n";

                fout.close();
            }
            else if (!std::strcmp(input, "Load"))
            {
                std::cin >> input;
                fin.open(input, std::ios::in | std::ios::binary);
                if (!fin.is_open()) {
                    std::cout << "ERROR: can't open file\n";
                    continue;
                }

                delete trie;
                trie = new TTrie();
                trie->Load(fin);

                std::cout << "OK\n";

                fin.close();
            }
        }
        else if (!std::strcmp(input, "1p"))
        {
            std::ofstream dot;
            dot.open("source.dot", std::ios::out | std::ios::trunc);

            dot << "digraph {\n";
            trie->PrintDefinitions(trie->root, dot);

            trie->PrintRelations(trie->root, dot);

            dot << "}\n";

            dot.flush(), dot.close();

            if (system("dot source.dot -Tpng -o res.png") == -1) {
                std::cout << "ERROR: fail compiling source.dot file into png\n";
                continue;
            } 
            
            if (system("xdg-open res.png") == -1) {
                std::cout << "ERROR: fail trying to open res.png\n";
                continue;
            } 
        }
        else
        {
            Lowercase(input);
            node = trie->Find(input);
            if (!node)
                std::cout << "NoSuchWord";
            else
                std::cout << "OK: " << node->value;
            std::cout << '\n';
        }
    }

    delete trie;

    return 0;
}
