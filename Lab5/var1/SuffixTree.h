#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// уникальный символ, добавление которого гарантирует построение true suffix tree
// т.е. никакой суффикс не будет являться префиксом какого-либо другого суффикса
#define TERMINATION_SYMBOL '$'

using namespace std;

class SuffixTree;

class Node
{
public:
    friend SuffixTree;

    // конструктор ноды без индекса: используется при создании новых внутренних вершин
    Node(Node *link, int start, int *end) : Node(link, start, end, -1)
    {
    }

    // конструктор с индексом: исп-ся при создании листьев
    // ибо при их создании можно доподлинно определить их индекс
    Node(Node *link, int start, int *end, int ind) : suffix_link(link),
                                                     start(start),
                                                     end(end),
                                                     suffix_index(ind)
    {
    }

private:
    map<char, Node *> children; // "массив" детей
    Node *suffix_link;          // суффиксная ссылка
    int start;                  // номер первого символа вершины в тексте
    int *end;                   // номер последнего символа вершины в тексте
    int suffix_index;           // номер суффикса
};

class SuffixTree
{
public:
    SuffixTree(string &text);             // конструктор по строке
    void BuildSuffixTree();               // само построение дерева
    void DisplayTree();                   // печать дерева
    void Search(string &pat, int patNum); // поиск паттерна
    ~SuffixTree()                         // деструктор
    {
        DeleteSuffixTree(root);
    }

private:
    void ExtendSuffixTree(int pos);   // расширение дерева
    void DeleteSuffixTree(Node *node) // удаление дерева
    {
        for (auto it : node->children)
            DeleteSuffixTree(it.second);
        if (node->suffix_index == -1)
            delete node->end;
        delete node;
    }
    void CountIndex(Node *node, vector<int> &vec); // сбор индексов листьев в вектор, содержащихся в поддереве node
    int EdgeLength(Node *node);                    // подсчет длины подстроки расположенной на дуге
    void Display(Node *node);                      // печать поддерева

    Node *root = new Node(nullptr, -1, new int(-1)); // корень
    Node *lastCreatedInternalNode = nullptr;         // последняя созданная внутрення вершина

    string text; // текст, по которому строится суфтри

    Node *activeNode = nullptr;   // то, откуда начнется расширение на следующей фазе
    int activeEdge = -1;          // индекс символа, который задает движение из текущей ноды
    int activeLength = 0;         // на сколько символов ступаем в направлении activeEdge
    int remainingSuffixCount = 0; // сколько суффиксов осталось создать. по сути, сколько суффиксов на прошлом шаге мы не создали.
    int leafEnd = -1;             // глобальная переменная, определяет содержимое листьев: её инкремент как бы приписывает новый символ к ним
};