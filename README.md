Leads and Lags with C++
================
Josh Bale

## Introduction

A collection of functions for working with lags in a data frame coded in
C++ and used in r via `rcpp`. This is my first attempt coding with C++
so I’m sure these aren’t the most efficient code (they’re just loops). I
will include benchmarking to any equivalent functions.

I will try and update these when I create a new function

## Lags

This function lags a given column by n. I have called it
‘shiftColumnDown’ as this is how I thought about it when coding it.

``` cpp
#include <Rcpp.h>
using namespace Rcpp;

DataFrame shiftColumnDown(DataFrame df, std::string column, int n) {
  // Get the number of rows in the dataframe
  int nrow = df.nrows();

  // Get the index of the columnn to be shifted
  int col_index = df.findName(column);
  
  // Create a new column to store the shifted values
  NumericVector shifted_column(nrow);
  
  // Get the column to be shifted as a numeric vector
  NumericVector col = as<NumericVector>(df[col_index]);

  // fill in the shifted column values
  for (int i = 0; i < nrow; i++) {
    // Check if the current row is less than n
    if (i < n) {
      shifted_column[i] = NA_REAL;
    } else {
      shifted_column[i] = col[i-n];
    }
  }

  
  // Add the shifted column to the dataframe
  df[column + "_" + "lag_" + std::to_string(n)] = shifted_column;
  
  return df;
}
```

Microbenching:

``` r
microbenchmark(mutate(data,returns_lag_4 = lag(returns,4)),
                      data.table(data)[, lag4 := shift(returns,4,type = "lag")],
                      shiftColumnDown(data,"returns",4))
```

    ## Unit: microseconds
    ##                                                             expr      min
    ##                    mutate(data, returns_lag_4 = lag(returns, 4)) 1916.600
    ##  data.table(data)[, `:=`(lag4, shift(returns, 4, type = "lag"))] 1252.301
    ##                              shiftColumnDown(data, "returns", 4)  531.500
    ##         lq     mean   median       uq       max neval
    ##  3602.5505 4519.982 4450.651 5030.651 16668.201   100
    ##  1617.3510 2586.011 2472.001 2782.651 26494.102   100
    ##   722.9505 1187.538 1208.050 1422.351  5351.301   100

This would return a dataframe such as:

    ##        returns       double      triple returns_lag_4
    ## 1 -0.010608477 -0.021216955 -0.03182543            NA
    ## 2 -0.026592845 -0.053185690 -0.07977853            NA
    ## 3 -0.001919660 -0.003839320 -0.00575898            NA
    ## 4  0.003804892  0.007609785  0.01141468            NA
    ## 5  0.009540750  0.019081501  0.02862225   -0.01060848
    ## 6  0.010264892  0.020529783  0.03079468   -0.02659284

## Creating Multiple Lags

This function is the natural extension of the previous lag function.
This time instead of one lag calculated for a given n each lag up to n
is calculated and added as a column.

\`\`\`
