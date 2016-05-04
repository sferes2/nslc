//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//|
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.




//#define NO_PARALLEL
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE nsga2_novelty


#include <boost/test/unit_test.hpp>
#include <cmath>
#include <iostream>
#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/ea/nsga2.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/pareto_front.hpp>
#include <sferes/eval/parallel.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/stat/archive.hpp>
#include <sferes/stat/best_archive_fit.hpp>
#include <Eigen/Core>
#include "ea_nslc.hpp"

using namespace sferes;
using namespace sferes::gen::evo_float;

struct Params {
  struct evo_float {
    SFERES_CONST float cross_rate = 0.5f;
    SFERES_CONST float mutation_rate = 0.5f;
    SFERES_CONST float eta_m = 15.0f;
    SFERES_CONST float eta_c = 10.0f;
    SFERES_CONST mutation_t mutation_type = polynomial;
    SFERES_CONST cross_over_t cross_over_type = sbx;
  };
  struct pop {
    SFERES_CONST unsigned size = 100;
    SFERES_CONST unsigned nb_gen = 500;
    SFERES_CONST float initial_aleat = 2.0f;
    SFERES_CONST int dump_period = 100;
  };
  struct parameters {
    SFERES_CONST float min = 0.0f;
    SFERES_CONST float max = 1.0f;
  };
  struct nslc {
    SFERES_CONST float rho_min_init = 0.1;
    SFERES_CONST size_t k = 15;
    SFERES_CONST size_t stalled_tresh = 2500;
    SFERES_CONST size_t adding_tresh = 4;
    SFERES_CONST float add_to_archive_prob = 0;
  };
};


// Rastrigin
SFERES_FITNESS(Rastrigin, sferes::fit::Fitness) {
  public:
  template <typename Indiv>
  void eval(Indiv & ind) {
    this->_objs.resize(2);
    float f = 10 * ind.size();
    for (size_t i = 0; i < ind.size(); ++i)
    f += ind.data(i) * ind.data(i) - 10 * cos(2 * M_PI * ind.data(i));
    this->_value = -f;

    _desc.clear();
    _desc.push_back(ind.gen().data(0));
    _desc.push_back(ind.gen().data(1));
  }
  template<typename Indiv>
  double dist(const Indiv& ind) {
    // Euclidean distance
    float x1 = _desc[0] - ind.fit()._desc[0];
    float x2 = _desc[1] - ind.fit()._desc[1];
    return x1 * x1 + x2 * x2;
  }
protected:
  std::vector<double> _desc;
};



BOOST_AUTO_TEST_CASE(test_nslc) {
  srand(time(0));

  typedef gen::EvoFloat<30, Params> gen_t;
  typedef phen::Parameters<gen_t, Rastrigin<Params>, Params> phen_t;
  typedef eval::Parallel<Params> eval_t;
  typedef boost::fusion::vector<stat::BestArchiveFit<phen_t, Params>,
                                stat::Archive<phen_t, Params> >  stat_t;
  typedef boost::fusion::vector<modif::Dummy<> > modifier_t;
  typedef ea::Nslc<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
  ea_t ea;

  ea.run();

}
