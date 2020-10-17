#pragma once

struct TKey
{
    int date[3] = {-1};    // days, months, years separately
    char str[11] = {'\0'}; // DD.MM.YYYY
};

struct TItem
{
    TKey key;                   // key
    unsigned long long val = 0; // value

    TItem() {}
    TItem(const TKey &k, const unsigned long long &v)
    {
        val = v;
        memcpy(key.date, k.date, 3 * sizeof(int));
        strcpy(key.str, k.str);
    }
};