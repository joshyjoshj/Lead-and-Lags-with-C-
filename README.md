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

Microbenching against ’dplyr::lag\`:

``` r
microbenchmark(mutate(data,returns_lag_4 = lag(returns,4)),
                               shiftColumnDown(data,"returns",4))
```

    ## Unit: milliseconds
    ##                                           expr      min       lq     mean
    ##  mutate(data, returns_lag_4 = lag(returns, 4)) 6.299601 8.159600 9.506509
    ##            shiftColumnDown(data, "returns", 4) 1.489300 2.239251 2.889154
    ##    median        uq       max neval
    ##  9.061751 10.926901 14.302401   100
    ##  2.856751  3.242252  9.150001   100
