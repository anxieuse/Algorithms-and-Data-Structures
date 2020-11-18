#pragma once

// проверка на равенство строк
static inline bool Equal(char *a, char *b)
{
    if (a == 0 || b == 0)
        return 0;
    return (strcmp(a, b) == 0);
}

// получить bit-ый по счету бит слева
static inline int GetBit(char *key, int bit)
{
    if (bit < 0)
        bit = 0;
    return (key[bit / 8] >> (7 - (bit % 8))) & 1;
}

static inline int FirstDifBit(char *a, char *b)
{
    if (a == 0 || b == 0)
        return 0;

    int i = 0;

    // продвижение по байтам
    while (a[i] == b[i])
        i++;
    i *= 8;

    // добивка: продвижание по биту в найденном байте
    while (GetBit(a, i) == GetBit(b, i))
        i++;

    return i;
}

static inline void Lowercase(char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        str[i] = str[i] >= 'A' && str[i] <= 'Z' ? str[i] - 'A' + 'a' : str[i];
    }
}
