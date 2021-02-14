#include "SuffixTree.h"

int main(int argc, char *argv[])
{
    ios::sync_with_stdio(0);
    cout.tie(0), cin.tie(0);

    string text, pattern;
    cin >> text;

    SuffixTree suffixTree(text);
    /* suffixTree.DisplayTree();
    cout << endl; */

    int patternNumber = 0;
    while (cin >> pattern)
        suffixTree.Search(pattern, ++patternNumber);
    
    return 0;
}