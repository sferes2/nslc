#ifndef _NSLC_HPP_
#define _NSLC_HPP_

#include <sferes/ea/nsga2.hpp>
#include <sferes/modif/novelty.hpp>
#include "crowd_gen.hpp"
#include "modif_nslc.hpp"

namespace sferes {
  namespace ea {
    // Novelty Search with local competition
    // based on NSGA-2
    // WARNING: NSLC does not support modifiers (because it already uses a modifier)
    template<typename Phen, typename Eval, typename Stat, typename Params,
             typename Exact = stc::Itself>
    class Nslc : public GenericNsga2<Phen, Eval, Stat, modif::Nslc<Phen, Params>,
    typename crowd_gen::assign_crowd<boost::shared_ptr<crowd::Indiv<Phen> > >, Params,
      typename stc::FindExact<ea::Nslc<Phen, Eval, Stat, Params, Exact>, Exact>::ret >

    {};
  }
}


#endif
