#include <iostream>
#include <cstdio>
#include <string.h>

#include "Vector.hpp"
#include "Item.hpp"

int main()
{
    TVector<TItem> vec;
    TKey k;
    unsigned long long v;

    int sup[3] = {0}; // max value of day, month and year got in input
    while (scanf("%s %llu", k.str, &v) > 0)
    {
        sscanf(k.str, "%d.%d.%d", &k.date[0], &k.date[1], &k.date[2]); // parsing key (string) into date array
        vec.PushBack(TItem(k, v));
        sup[0] = std::max(sup[0], k.date[0]);
        sup[1] = std::max(sup[1], k.date[1]);
        sup[2] = std::max(sup[2], k.date[2]);
    }

    int n = vec.Size();
    TVector<TItem> res(n); // gonna be sorted vector

    for (int R = 0; R < 3; ++R) // R = 0 means counting sort by days, 1 - by months, 2 - by years
    {
        // cnt: number of occurrences of days or months or years (depends on R)
        TVector<int> cnt(sup[R] + 1, 0);
        for (int i = 0; i < n; ++i)
        {
            cnt[vec[i].key.date[R]]++;
        }
        // counting prefix-sums of cnt
        for (int i = 1; i < sup[R] + 1; ++i)
        {
            cnt[i] += cnt[i - 1];
        }
        // getting result sorted by days or months or years (depends on R)
        for (int i = n - 1; i >= 0; i--)
        {
            res[ cnt[vec[i].key.date[R]] - 1 ] = vec[i];
            --cnt[vec[i].key.date[R]];
        }
        for (int i = 0; i < n; i++)
        {
            vec[i] = res[i];
        }
    }

    for (int i = 0; i < n; ++i)
    {
        printf("%s\t%llu\n", vec[i].key.str, vec[i].val);
    }

    return 0;
}
