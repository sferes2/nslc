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


#ifndef   	CROWD_GEN_H_
# define   	CROWD_GEN_H_

#include <sferes/parallel.hpp>

namespace sferes {
  namespace ea {
    namespace crowd_gen {
      static const float inf = 1.0e14;

      // genotype-based replacement for the crowding distance
      // in NSGA-2
      // WARNING: this will only work with vector-based genotypes (with the data() method)
      template<typename Indiv>
      class assign_crowd {
       public:
        std::vector<std::vector<Indiv> >& _fronts;
        assign_crowd(std::vector<std::vector<Indiv> >& fronts) :
          _fronts(fronts) {}
        assign_crowd(const assign_crowd& ev) : _fronts(ev._fronts) {}
        void operator() (const parallel::range_t& r) const {
          for (size_t i = r.begin(); i != r.end(); ++i)
            _assign_crowd(_fronts[i]);
        }
       protected:
        typedef typename std::vector<Indiv>::iterator it_t;
        typedef typename std::vector<Indiv>::const_iterator cit_t;

        void _assign_crowd(std::vector<Indiv>& f) const {
          for (it_t it = f.begin() ; it != f.end() ; ++it) {
            float d = 0.0f;
            for (it_t it2 = f.begin() ; it2 != f.end() ; ++it2) {
              float dist = 0;
              assert((*it)->data().size() == (*it2)->data().size());
              for (size_t i = 0; i < (*it)->data().size(); i++)
                dist += (float) fabs((*it)->data()[i] - (*it2)->data()[i]);
              d+=dist / (float)(*it)->data().size();

            }
            d /= (float)f.size();
            (*it)->set_crowd(d);
          }
        }
      };


    }
  }
}

#endif 	    /* !CROWD_H_ */
