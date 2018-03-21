#include "marie.h"
#include "list.h"
#include "math.h"
#include "strings.h"

//
// https://en.wikipedia.org/wiki/Levenshtein_distance
//
u32 
LevenshteinDistance(char *s, u32 len_s, char *t, u32 len_t)
{ 
  int cost;

  /* base case: empty strings */
  if (len_s == 0)
  {
      return len_t;
  }

  if (len_t == 0)
  {
      return len_s;
  }

  /* test if last characters of the strings match */
  if (s[len_s - 1] == t[len_t - 1])
  {
      cost = 0;
  }
  else
  {
      cost = 1;
  }

  /* return minimum of delete char from s, delete char from t, and delete char from both */
  return Min(LevenshteinDistance(s, len_s - 1, t, len_t    ) + 1,
         Min(LevenshteinDistance(s, len_s    , t, len_t - 1) + 1,
             LevenshteinDistance(s, len_s - 1, t, len_t - 1) + cost));
}

INTERNAL MatchResult
GetBestSrcInstrMatch(SrcInstruction *instrs, u32 instrCount, char *name)
{
    int nameLength = StringLength(name);

    u32 minIndex = 0;
    u32 minMin   = LevenshteinDistance(instrs[0].name, StringLength(instrs[0].name), name, nameLength);

    for (u32 i = 1; (i < instrCount) && (minMin != 0); ++i)
    {
        u32 min = LevenshteinDistance(instrs[i].name, StringLength(instrs[i].name), name, nameLength);

        if (min < minMin)
        {
            minMin = min;
            minIndex = i;
        }
    }

    MatchResult result;
    result.weight = static_cast<float>(minMin);
    result.match  = instrs + minIndex;

    return result;
}

INTERNAL MatchResult
GetBestSymbolMatch(SymbolList *list, char *name)
{
    int nameLength = StringLength(name);

    Symbol *symbolZero = PeakAt(list, 0);

    u32 minIndex = 0;
    u32 minMin   = LevenshteinDistance(symbolZero->name, StringLength(symbolZero->name), name, nameLength);

    for (u32 i = 1; (i < list->count) && (minMin != 0); ++i)
    {
        Symbol *symbol = PeakAt(list, i);

        u32 min = LevenshteinDistance(symbol->name, StringLength(symbol->name), name, nameLength);

        if (min < minMin)
        {
            minMin = min;
            minIndex = i;
        }
    }

    MatchResult result;
    result.weight = static_cast<float>(minMin);
    result.match  = PeakAt(list, minIndex);

    return result;
}

