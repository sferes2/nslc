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

#ifndef MODIFIER_NSLC_HPP
#define MODIFIER_NSLC_HPP

#include <Eigen/Core>
#include <sferes/parallel.hpp>

namespace sferes {
  namespace modif {

    namespace nslc {
      template<typename T>
      struct _compare_dist_p {
        _compare_dist_p(const T& v) : _v(v) {}
        const T& _v;
        template<typename T1, typename T2>
        bool operator()(const T1& v1, const T2& v2) const {
          return _v->fit().dist(*v1) < _v->fit().dist(*v2);
        }
      };
    }

    template<typename Phen, typename Params, typename Exact = stc::Itself>
    class Nslc {
     public:
      typedef boost::shared_ptr<Phen> phen_t;
      typedef std::vector<phen_t> pop_t;

      Nslc() : _rho_min(Params::nslc::rho_min_init), _not_added(0) {}

      template<typename Ea>
      void apply(Ea& ea) {
        size_t k = Params::nslc::k;
        // merge the population and the archive in a single archive
        pop_t archive = _archive;
        archive.insert(archive.end(), ea.pop().begin(), ea.pop().end());

        // compute the sparseness of each individual of the population
        // and potentially add some of them to the archive
        int added = 0;
        for (size_t i = 0; i < ea.pop().size(); ++i) {
          size_t nb_objs = ea.pop()[i]->fit().objs().size();
          assert(nb_objs >= 2);

          // sort
          parallel::sort(archive.begin(), archive.end(),
                         nslc::_compare_dist_p<phen_t>(ea.pop()[i]));

          // local competition score
          int rank = k;
          for (size_t j = 0; j < k; ++j)
            if (ea.pop()[i]->fit().value() < archive[j]->fit().value())
              rank--;
          ea.pop()[i]->fit().set_obj(0, rank);

          // novelty
          double n = 0.0;
          for (size_t j = 0; j < k; ++j)
            n += ea.pop()[i]->fit().dist(*archive[j]);

          ea.pop()[i]->fit().set_obj(1, n);

          // add to the archive
          if (n > _rho_min
              || misc::rand<float>() < Params::novelty::add_to_archive_prob) {
            _archive.push_back(ea.pop()[i]);
            _not_added = 0;
            ++added;
          } else {
            ++_not_added;
          }
        } // end for all individuals

        // update rho_min
        if (_not_added > Params::novelty::stalled_tresh) { //2500
          _rho_min *= 0.95;
          _not_added = 0;
        }
        if (_archive.size() > Params::novelty::k
            && added > Params::novelty::adding_tresh) {//4
          _rho_min *= 1.05f;
        }
        std::cout<<"archive size:"<<_archive.size()<<std::endl;
      }
      const pop_t& archive() const {
        return _archive;
      }
     protected:
      pop_t _archive;
      float _rho_min;
      size_t _not_added;
    };
  } // modif
} // sferes

#endif
