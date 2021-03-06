// DistanceDist.h
//
// Copyright (C)  2017  Alexander Eckert
//
// This file is part of parallelDist.
//
// parallelDist is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// parallelDist is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with parallelDist. If not, see <http://www.gnu.org/licenses/>.

#ifndef DISTANCEDIST_H_
#define DISTANCEDIST_H_

#include "IDistance.h"
#include "Utility.h"
#include <limits>

//=======================
// Bhjattacharyya
//=======================
class DistanceBhjattacharyya : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // sqrt(sum_i (sqrt(x_i) - sqrt(y_i))^2))
    return sqrt(arma::accu(arma::square(arma::sqrt(A) - arma::sqrt(B))));
  }
};

//=======================
// Bray
//=======================
class DistanceBray : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // sum_i |x_i - y_i| / sum_i (x_i + y_i)
    return arma::accu(arma::abs(A - B)) / arma::accu(A + B);
  }
};

//=======================
// Canberra distance
//=======================
class DistanceCanberra : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    arma::mat denominator = arma::abs(A + B);
    arma::mat ratio = arma::abs(A - B) / denominator;

    unsigned int notNanCount = 0;
    for(arma::mat::iterator it = ratio.begin(); it != ratio.end(); ++it) {
      if (std::isnan(*it)) {
        (*it) = double(0);
      } else {
        ++notNanCount;
      }
    }

    if (ratio.size() - notNanCount > 0) {
      return ((notNanCount + 1)/ (double)notNanCount) * arma::accu(ratio);
    } else {
      return arma::accu(ratio);
    }
  }
};

//=======================
// Canberra distance
//=======================
// class DistanceCanberraOrg : public IDistance {
// public:
//   DistanceCanberraOrg () {}
//   ~DistanceCanberraOrg () {}
//   double calcDistance(const arma::mat &A, const arma::mat &B) {
//     arma::mat denominator = arma::abs(A) + arma::abs(B);
//     std::cout << "denominator"<< std::endl;
//     denominator.print(std::cout);
//     arma::mat ratio = arma::abs(A - B) / denominator;
//     std::cout << "ratio"<< std::endl;
//     ratio.print(std::cout);
//     ratio.transform( [](double val) {
//       return (std::isnan(val) ? double(1) : val);
//     } );
//     std::cout << "ratio after nan replacement"<< std::endl;
//     ratio.print(std::cout);
//     return arma::accu(ratio);
//   }
// };

//=======================
// Chord
//=======================
class DistanceChord : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // sqrt(2 * (1 - xy / sqrt(xx * yy)))
    return sqrt(2 * (1 - arma::dot(A.row(0), B.row(0)) / sqrt(arma::dot(A.row(0), A.row(0)) * arma::dot(B.row(0), B.row(0)))));
  }
};

//=======================
// Divergence
//=======================
class DistanceDivergence : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // sum_i (x_i - y_i)^2 / (x_i + y_i)^2
    arma::mat tmp = arma::square(A - B) / arma::square(A + B);
    if (tmp.has_nan()) {
      tmp.transform( [](double val) {
        return (std::isnan(val) ? 0 : val);
      } );
    }
    return arma::accu(tmp);
  }
};

//=======================
// Euclidean distance
//=======================
class DistanceEuclidean : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    return sqrt(arma::accu(arma::square(A - B)));
  }
};

//=======================
// FJaccard
//=======================
class DistanceFJaccard : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    //sum_i (min{x_i, y_i} / max{x_i, y_i})
    arma::mat tmp = arma::join_cols(A, B);
    return utility::similarityToDistance(arma::accu(colwise_min_idx(tmp)) / arma::accu(colwise_max_idx(tmp)));
  }
};

//=======================
// Geodesic
//=======================
class DistanceGeodesic : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // arccos(xy / sqrt(xx * yy))
    return acos(arma::dot(A.row(0), B.row(0)) / sqrt(arma::dot(A.row(0), A.row(0)) * arma::dot(B.row(0), B.row(0))));
  }
};

//=======================
// Hellinger
//=======================
class DistanceHellinger : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    //sqrt(sum_i (sqrt(x_i / sum_i x) - sqrt(y_i / sum_i y)) ^ 2)
    return sqrt(arma::accu(arma::square(arma::sqrt(A / arma::accu(A)) - arma::sqrt(B /  arma::accu(B)))));
  }
};

//=======================
// Kullback, Leibler
//=======================
class DistanceKullback : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // sum_i [x_i * log((x_i / sum_j x_j) / (y_i / sum_j y_j)) / sum_j x_j)]
    arma::mat p = A / arma::accu(A);
    arma::mat q = B / arma::accu(B);
    double result = arma::accu(p * arma::log(p / q).t());
    // return same results as dist
    return std::isinf(result) ? std::numeric_limits<double>::quiet_NaN() : result;
  }
};

//=======================
// Mahalanobis
//=======================
class DistanceMahalanobis : public IDistance {
private:
  arma::mat invertedCov;
public:
  explicit DistanceMahalanobis(const arma::mat &invertedCov) {
    this->invertedCov = invertedCov;
  }
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    arma::mat C = A - B;
    return sqrt(arma::accu(C * this->invertedCov % C));
  }
};

//=======================
// Manhattan distance
//=======================
class DistanceManhattan : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    return arma::accu(arma::abs(A - B));
  }
};

//=======================
// Maximum distance
//=======================
class DistanceMaximum : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    return arma::abs(A - B).max();
  }
};

//=======================
// Minkowski distance
//=======================
class DistanceMinkowski : public IDistance {
private:
    unsigned int p;
public:
    explicit DistanceMinkowski (unsigned int p) {
        this->p = p;
    }
    ~DistanceMinkowski() {}
    double calcDistance(const arma::mat &A, const arma::mat &B) {
        return pow(arma::accu(arma::pow(arma::abs(A - B), this->p)), 1.0 / this->p);
    }
};

//=======================
// Podani
//=======================
class DistancePodani : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    unsigned long n = A.n_cols;
    unsigned long a, b, c, d;
    a = b = c = d = 0;
    for (unsigned int i = 0; i < n; i++) {
      for (unsigned int j = i + 1; j < n; j++) {
        if ((A(0, i) < A(0, j) && B(0, i) < B(0, j)) || (A(0, i) > A(0, j) && B(0, i) > B(0, j))) {
          a++;
        }
        if ((A(0, i) < A(0, j) && B(0, i) > B(0, j)) || (A(0, i) > A(0, j) && B(0, i) < B(0, j))) {
          b++;
        }
        if (A(0, i) == A(0, j) && B(0, i) == B(0, j) && ((A(0, i) == 0 &&  B(0, i) == 0) || (A(0, i) > 0 &&  B(0, i) > 0))) {
          c++;
        }

        unsigned int z = 0;
        if (A(0, i) == 0) ++z;
        if (A(0, j) == 0) ++z;
        if (B(0, i) == 0) ++z;
        if (B(0, j) == 0) ++z;

        if ((A(0, i) == A(0, j) || B(0, i) == B(0, j)) && z > 0 && z < 4) {
          d++;
        }
      }
    }
    return 1 - 2 * ((double) a - b + c - d) / (n * (n - 1));
  }
};

//=======================
// Soergel
//=======================
class DistanceSoergel : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    // sum_i |x_i - y_i| / sum_i max{x_i, y_i}
    arma::mat tmp = arma::join_cols(A, B);
    return arma::accu(arma::abs(A - B)) / arma::accu(colwise_max_idx(tmp));
  }
};

//=======================
// Wave
// See Comprehensive Survey on Distance/Similarity Measures between Probability Density Functions
// Sung-Hyuk Cha
//=======================
class DistanceWave : public IDistance {
public:
    double calcDistance(const arma::mat &A, const arma::mat &B) {
        //sum_i (1 - min(x_i, y_i) / max(x_i, y_i))
        arma::mat tmp = arma::join_cols(A, B);
        arma::mat substr = arma::abs(A - B);
        return arma::accu(substr / repmat(colwise_max_idx(tmp), substr.n_rows, 1));
    }
};

//=======================
// Whittaker
//=======================
class DistanceWhittaker : public IDistance {
public:
    double calcDistance(const arma::mat &A, const arma::mat &B) {
        // sum_i |x_i / sum_i x - y_i / sum_i y| / 2
        return arma::accu(arma::abs(A / arma::accu(A) - B / arma::accu(B))) / 2.0;
    }
};

//=======================
// Cosine
//=======================
class DistanceCosine : public IDistance {
public:
  double calcDistance(const arma::mat &A, const arma::mat &B) {
    return utility::similarityToDistance(arma::as_scalar(arma::dot(A, B))/(arma::as_scalar(arma::norm(A))*arma::as_scalar(arma::norm(B))));
  }
};

//=======================
// Custom distance
//=======================
class DistanceCustom : public IDistance {
private:
    funcPtr func;
public:
    explicit DistanceCustom (funcPtr function) : func(function) {
      this->func = function;
    };
    ~DistanceCustom () {}
    double calcDistance(const arma::mat &A, const arma::mat &B) {
        return func(A, B);
    }
};

#endif
