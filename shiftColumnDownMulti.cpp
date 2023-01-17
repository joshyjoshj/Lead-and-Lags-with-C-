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