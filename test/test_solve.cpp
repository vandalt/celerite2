#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "helpers.hpp"

#include <Eigen/Dense>
#include <celerite2/core.hpp>

TEMPLATE_LIST_TEST_CASE("check the results of solve", "[solve]", TestKernels) {
  auto kernel = TestType::get_kernel();

  Vector x, diag;
  Matrix Y, Z;
  std::tie(x, diag, Y) = get_data();
  const int N          = x.rows();

  typename decltype(kernel)::Vector a;
  typename decltype(kernel)::LowRank U, V, P;
  std::tie(a, U, V, P) = kernel.get_celerite_matrices(x, diag);

  Matrix K, S, F, G;
  celerite::core::to_dense(a, U, V, P, K);

  // Do the solve using celerite
  int flag = celerite::core::factor(U, P, a, V, S);
  REQUIRE(flag == 0);
  Z = Y;
  celerite::core::solve(U, P, a, V, Z, F, G);

  // Brute force the solve
  Eigen::LDLT<Matrix> LDLT(K);
  Matrix expect = LDLT.solve(Y);

  // Check the result
  double resid = (expect - Z).array().abs().maxCoeff();
  REQUIRE(resid < 1e-12);
}