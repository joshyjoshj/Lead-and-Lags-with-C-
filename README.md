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
    ##                    mutate(data, returns_lag_4 = lag(returns, 4)) 3196.101
    ##  data.table(data)[, `:=`(lag4, shift(returns, 4, type = "lag"))] 1766.000
    ##                              shiftColumnDown(data, "returns", 4)  846.501
    ##        lq     mean   median       uq     max neval
    ##  4520.051 8357.367 7181.151 9939.352 53534.9   100
    ##  2650.751 6014.395 4653.002 6538.702 77026.7   100
    ##  1234.151 3271.952 2210.551 3193.151 26024.7   100

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
is calculated and added as a column. It was quite easy to modify the
previous code as I just had to add an outer loop and slightly adjust the
if statements for the assignment.

``` cpp
#include <Rcpp.h>
using namespace Rcpp;

DataFrame shiftColumnDownMulti(DataFrame df, std::string column, int n) {
  // Get the number of rows in the dataframe
  int nrow = df.nrows();

  // Get the index of the column to be shifted
  int col_index = df.findName(column);
  
  // Get the column To be shifted as a numeric vector
  NumericVector col = as<NumericVector>(df[col_index]);

  // Loop through each shift value
  for (int i = 1; i <= n; i++) {
    // Create a new column to store the shifte values
    NumericVector shifted_column(nrow);
    
    // Fill in the shifted column values
    for (int j = 0; j < nrow; j++) {
      // Check if the current row minus the shift value is greater than or equal to 0
      if (j-i >= 0) {
        shifted_column[j] = col[j-i];
      } else {
        shifted_column[j] = NA_REAL;
      }
    }
    
    
    // Add the shifted column to the dataframe
    df[column + "_" + "lag_" + std::to_string(i)] = shifted_column;
  }
  
  return df;
}
```

Microbenching:

``` r
microbenchmark(tidyAR::ar_df(data,y="returns",p=45),
                               tk_augment_lags(data,returns, .lags=1:45),
                               shiftColumnDownMulti(data,"returns",45))
```

    ## Unit: milliseconds
    ##                                          expr     min        lq     mean
    ##    tidyAR::ar_df(data, y = "returns", p = 45) 10.6527 35.838400 62.40181
    ##  tk_augment_lags(data, returns, .lags = 1:45) 11.6855 34.303551 57.72326
    ##     shiftColumnDownMulti(data, "returns", 45)  3.5628  9.700351 20.03619
    ##    median       uq      max neval
    ##  53.09185 75.11720 307.1317   100
    ##  48.37410 72.68800 218.4378   100
    ##  15.89090 23.47645 164.3693   100

As you can see my `tidyAR` package function is not very quick and I’m
working on implementing these functions in the package to speed it up.

This would return a dataframe such as:

    ##        returns       double      triple returns_lag_1 returns_lag_2
    ## 1 -0.010608477 -0.021216955 -0.03182543            NA            NA
    ## 2 -0.026592845 -0.053185690 -0.07977853  -0.010608477            NA
    ## 3 -0.001919660 -0.003839320 -0.00575898  -0.026592845  -0.010608477
    ## 4  0.003804892  0.007609785  0.01141468  -0.001919660  -0.026592845
    ## 5  0.009540750  0.019081501  0.02862225   0.003804892  -0.001919660
    ## 6  0.010264892  0.020529783  0.03079468   0.009540750   0.003804892
    ##   returns_lag_3 returns_lag_4
    ## 1            NA            NA
    ## 2            NA            NA
    ## 3            NA            NA
    ## 4   -0.01060848            NA
    ## 5   -0.02659284   -0.01060848
    ## 6   -0.00191966   -0.02659284
