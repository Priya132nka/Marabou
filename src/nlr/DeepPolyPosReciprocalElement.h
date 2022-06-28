/*********************                                                        */
/*! \file DeepPolyPosReciprocalElement.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Haoze Wu
 ** This file is part of the Marabou project.
 ** Copyright (c) 2017-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved. See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** [[ Add lengthier description here ]]

**/

#ifndef __DeepPolyPosReciprocalElement_h__
#define __DeepPolyPosReciprocalElement_h__

#include "DeepPolyElement.h"
#include "Layer.h"
#include "MStringf.h"
#include "NLRError.h"
#include <climits>

namespace NLR {

class DeepPolyPosReciprocalElement : public DeepPolyElement
{
public:
    DeepPolyPosReciprocalElement( Layer *layer );
    ~DeepPolyPosReciprocalElement();

    void execute( const Map<unsigned, DeepPolyElement *>
                  &deepPolyElementsBefore );

    void symbolicBoundInTermsOfPredecessor
    ( const double *symbolicLb, const double*symbolicUb, double
      *symbolicLowerBias, double *symbolicUpperBias, double
      *symbolicLbInTermsOfPredecessor, double *symbolicUbInTermsOfPredecessor,
      unsigned targetLayerSize, DeepPolyElement *predecessor );

private:

    void allocateMemory();
    void freeMemoryIfNeeded();
    void log( const String &message );

    /*
      Compute the sigmoid function.
    */
    double sigmoid( double x );

    /*
      Compute the derivative of the sigmoid function.
    */
    double sigmoid_diff( double x );
};

} // namespace NLR

#endif // __DeepPolyElement_h__
