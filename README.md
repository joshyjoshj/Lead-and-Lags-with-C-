Leads and Lags with C++
================
Josh Bale
2023-01-17

## Introduction

A collection of functions for working with lags in a data frame coded in
C++ and used in r via `rcpp`. This is my first attempt coding with C++
so I’m sure these aren’t the most efficient code (they’re just loops). I
will include benchmarking to any equivalent functions.

I will try and update these when I create a new function

## Lags

``` rcpp
#include <Rcpp.h>
using namespace Rcpp;

DataFrame shiftColumnDown(DataFrame df, std::string column, int n) {
  // Get the number of rows in the dataframe
  int nrow = df.nrows();

  // Get the index of the column to be shifted
  int col_index = df.findName(column);
  
  // Create a new column to store the shifted values
  NumericVector shifted_column(nrow);
  
  // Get the column to be shifted as a numeric vector
  NumericVector col = as<NumericVector>(df[col_index]);

  // Fill in the shifted column values
  for (int i = 0; i < nrow; i++) {
    // Check if the current row is less than "n"
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
