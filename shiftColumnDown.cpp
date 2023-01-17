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