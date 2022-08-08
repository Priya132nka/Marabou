/*********************                                                        */
/*! \file DeepPolyQuadraticElement.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Haoze Andrew Wu
 ** This file is part of the Marabou project.
 ** Copyright (c) 2017-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved. See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** [[ Add lengthier description here ]]

**/

#ifndef __DeepPolyQuadraticElement_h__
#define __DeepPolyQuadraticElement_h__

#include "DeepPolyElement.h"
#include "Layer.h"
#include "MStringf.h"
#include "NLRError.h"
#include <climits>

#include "Map.h"

namespace NLR {

class DeepPolyQuadraticElement : public DeepPolyElement
{
public:
    DeepPolyQuadraticElement( Layer *layer );
    ~DeepPolyQuadraticElement();

    void execute( const Map<unsigned, DeepPolyElement *>
                  &deepPolyElementsBefore );

    void symbolicBoundInTermsOfPredecessor
    ( const double *symbolicLb, const double*symbolicUb, double
      *symbolicLowerBias, double *symbolicUpperBias, double
      *symbolicLbInTermsOfPredecessor, double *symbolicUbInTermsOfPredecessor,
      unsigned targetLayerSize, DeepPolyElement *predecessor );

private:

    /*
      The index of the source variable used in the symbolic bounds.
    */
    Map<unsigned, NeuronIndex> _sourceIndexForSymbolicBounds;
    Map<unsigned, bool> _phaseFixed;

    void allocateMemory();
    void freeMemoryIfNeeded();
    void log( const String &message );
};

} // namespace NLR

#endif // __DeepPolyElement_h__
