#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

void BadCharacter(const std::vector<std::string> &pattern, std::map<std::string, int> &badCharacter)
{
    // цикл до предпосл. слова, ибо последнее слово не учитывается: для него определяем предпоследнюю позицию
    for (int i = 0; i < (int)pattern.size() - 1; ++i)
        badCharacter[pattern[i]] = i;
}

void GoodSuffix(const std::vector<std::string> &pattern, std::vector<int> &goodSuffix)
{
    int n = (int)pattern.size();
    int left = 0;
    int right = 0;
    // z-функция для перевернутого паттерна
    std::vector<int> zFunction(n, 0);
    for (int i = 1; i < n; ++i) {
        if (i <= right)
            zFunction[i] = std::min(right - i + 1, zFunction[i - left]);

        while (i + zFunction[i] < n && (pattern[n - 1 - zFunction[i]] == pattern[n - 1 - (i + zFunction[i])]))
            zFunction[i]++;

        if (i + zFunction[i] - 1 > right) {
            left = i;
            right = i + zFunction[i] - 1;
        }
    }

    // N-функция -- перевернутая z-функция для перевернутого паттерна
    std::vector<int> N(zFunction.rbegin(), zFunction.rend());
    // L-функция: если произошло несовпадение в i, то L[i + 1]
    // определяет правую границу подстроки (притом самой правой подстроки), равной суффиксу pat[i + 1 ... n - 1] (далее зову всю ее ПГПРС)
    // сдвиг, очевидно, при этом будет равен (n - 1) - L[i + 1]
    std::vector<int> L(n + 1, n);

    int j;
    for (int i = 0; i < n - 1; ++i) {
        // проходя слева направо, определяем, для кого мы будем являться ПГПРС (см. 38 строка)
        // для j, очевидно: то есть при несовпадении в j - 1, на нас, L[j], будут ссылаться за помощью! (за сдвигом)
        j = n - N[i];
        L[j] = i;
    }

    // но если подстроки, равной суффиксу pat[i + 1 ... n - 1] в образце нет,
    // будем сдвигать на минимальное число так, чтобы некотрый префикс образца
    // "наезжал" на суффикс
    std::vector<int> l(n + 1, n);
    for (int i = n - 1; i >= 0; i--) {
        // длина суффикса
        j = n - i;
        if (N[j - 1] == j)
            // если N[j-1] == j, значит, префикс 0...j-1 совпадает с суффиксом i...0
            // ну и правая граница этого префикса определит наш сдвиг (при несовп. в i он будет равен (n - 1) - l[i + 1])
            l[i] = (j - 1);
        else
            // иначе, положим l[i] = l[i + 1], тогда при сдвиге сопадет все, кроме несольких первых символов префикса 0...j-1
            // а именно, что-то в роде (j - 1) - n[j - 1] первых символов не совпадут
            l[i] = l[i + 1];
    }

    // если L[i] == n, значит, нет соответствующей суффиксу подстроки: воспользуемся l-функцией
    for (int i = 0; i < n + 1; ++i)
        if (L[i] == n)
            L[i] = l[i];

    // теперь, чтобы не заморачиваться, писать всюду shift = n - 1 - L[i+1], переопределим значения L функции,
    // чтобы сразу писать далее shift = L[i + 1]
    for (auto &x : L)
        if (x != n)
            x = n - 1 - x;

    goodSuffix = L;
}

void Lowercase(std::string &str)
{
    for (int i = 0; i < (int)str.size(); ++i)
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 'a' - 'A';
}

void Read(std::vector<std::string> &pattern, std::vector<std::string> &txt, std::vector<std::pair<int, int>> &rowCol)
{
    // reading pattern
    std::string curLine;
    getline(std::cin, curLine, '\n');
    // splitting pattern-line into words
    std::istringstream stringStream(curLine);
    std::string curWord;
    while (stringStream >> curWord) {
        Lowercase(curWord);
        pattern.push_back(curWord);
    }
    // reading, parsing text into words
    int lineNum = 1;
    int wordNum = 0;
    int lineLen;
    int left;
    std::pair<int, int> lineNumWordNum;
    while (getline(std::cin, curLine, '\n')) {
        wordNum = 1;
        lineLen = curLine.size();
        for (int curPos = 0; curPos < lineLen;) {
            if (isalpha(curLine[curPos])) {
                left = curPos;
                while (isalpha(curLine[curPos]) && curPos < lineLen)
                    ++curPos;

                lineNumWordNum.first = lineNum;
                lineNumWordNum.second = wordNum++;
                rowCol.push_back(lineNumWordNum);

                curWord = curLine.substr(left, curPos - left);
                Lowercase(curWord);
                txt.push_back(curWord);
            }
            ++curPos;
        }
        ++lineNum;
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);

    std::vector<std::string> pattern;
    std::vector<std::string> txt;
    std::vector<std::pair<int, int>> rowCol; // rowcol[i].first - строка i-го слова, rowcol[i].second - номер этого слова в строке
    Read(pattern, txt, rowCol);

    // bC[i] определяет крайнее правое вхождение каждого символа паттерна (за искл. последнего симола паттерна: считается его предпосл. позиция)
    std::map<std::string, int> badCharacter;
    BadCharacter(pattern, badCharacter);

    // gS[i] определяет, насколько вправо двигать шаблон при несовпадении символа на поз. i - 1
    std::vector<int> goodSuffix(pattern.size() + 1, pattern.size());
    GoodSuffix(pattern, goodSuffix);

    std::vector<int> occurPos;
    int j, bound = 0, shift = 0;
    for (int i = 0; i < 1 + (int)txt.size() - (int)pattern.size();) {
        for (j = pattern.size() - 1; j >= bound; j--) {
            if (pattern[j] != txt[i + j]) {
                break;
            }
        }
        if (j < bound) {
            occurPos.push_back(i);
            bound = pattern.size() - goodSuffix[0];
            j = -1;
        }
        else {
            bound = 0;
        }
        if (j < bound) {
            shift = goodSuffix[j + 1];
        }
        else {
            shift = std::max({1, goodSuffix[j + 1], j - badCharacter[txt[i + j]]});
        }
        i += shift;
    }

    for (const auto &oc : occurPos) 
        std::cout << rowCol[oc].first << ", " << rowCol[oc].second << '\n';

    return 0;
}
