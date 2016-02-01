// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef TOOLS_INT_H_
#define TOOLS_INT_H_


typedef unsigned char octet;

// Assumes word is a 64 bit value
#ifdef WIN32
  typedef unsigned __int64 word;
#else
  typedef unsigned long word;
#endif


inline int CEIL_LOG2(int x)
{
    int result = 0;
    x--;
    while (x > 0)
    {
        result++;
        x >>= 1;
    }
    return result;
}

inline int FLOOR_LOG2(int x)
{
    int result = 0;
    while (x > 1)
    {
        result++;
        x >>= 1;
    }
    return result;
}

// ceil(n / k)
inline int DIV_CEIL(long n, int k)
{
    return (n + k - 1)/k;
}


#endif /* TOOLS_INT_H_ */
