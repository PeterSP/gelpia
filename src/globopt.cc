#include <boost/numeric/interval.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <functional>
#include <cassert>
#include "helpers.h"
using boost::numeric::interval;
using std::vector;
using std::function;

typedef interval<double> interval_t;
typedef vector<interval_t>  box_t;
typedef unsigned int uint;

/*
 * Global maximum solver
 * Arguments:
 *          X_0      - given starting box
 *          x_tol    -
 *          f_tol    -
 *          max_iter - maximum iterations
 *          F        - function to fin maximum of
 */
double serial_solver(const box_t & X_0, double x_tol, double f_tol, int max_iter,
		     const function<interval<double>(const box_t &)> & F)
{
  std::queue<box_t> Q;
  Q.push(X_0);

  double f_best_low = -INFINITY, f_best_high = -INFINITY;
  int iter_count = 0;

  while(!Q.empty()) {
    // grab new work item
    box_t X = Q.front();
    Q.pop();


    interval<double> f = F(X);
    double w = width(X);
    double fw = width(f);

    if(f.upper() < f_best_low
       || w <= x_tol
       || fw <= f_tol
       || iter_count > max_iter) {
      // found new maximum
      f_best_high = fmax(f_best_high, f.upper());
      continue;
    } else {
      iter_count++;
      vector<box_t> X_12 = split_box(X);
      for(auto Xi : X_12) {
	interval<double> e = F(midpoint(Xi));
	if(e.lower() > f_best_low) {
	  f_best_low = e.lower();
	}
	Q.push(Xi);
      }
    }
  }
  return f_best_high;
}
