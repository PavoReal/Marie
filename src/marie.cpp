
//
// https://en.wikipedia.org/wiki/Levenshtein_distance
//
int 
LevenshteinDistance(char *s, int len_s, char *t, int len_t)
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
GetBestSrcInstrMatch(SrcInstruction *instrs, uint32 instrCount, char *name)
{
    int nameLength = StringLength(name);

    int minIndex;
    int minMin;

    minIndex = 0;
    minMin   = LevenshteinDistance(instrs[0].name, StringLength(instrs[0].name), name, nameLength);

    for (uint32 i = 1; (i < instrCount) && (minMin != 0); ++i)
    {
        int min = LevenshteinDistance(instrs[i].name, StringLength(instrs[i].name), name, nameLength);

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

