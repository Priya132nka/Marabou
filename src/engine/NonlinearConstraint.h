/*********************                                                        */
/*! \file NonLinearConstraint.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Teruhiro Tagomori, Haoze (Andrew) Wu
 ** This file is part of the Marabou project.
 ** Copyright (c) 2017-2019 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved. See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** [[ Add lengthier description here ]]

**/

#ifndef __NonLinearConstraint_h__
#define __NonLinearConstraint_h__

#include "BoundManager.h"
#include "FloatUtils.h"
#include "ITableau.h"
#include "List.h"
#include "Map.h"
#include "NonlinearFunctionType.h"
#include "Queue.h"
#include "Tightening.h"

class Equation;
class IConstraintBoundTightener;
class ITableau;
class InputQuery;
class String;

class NonlinearConstraint : public ITableau::VariableWatcher
{
public:
    NonlinearConstraint();
    virtual ~NonlinearConstraint()
    {
    }

    /*
      Get the type of this constraint.
    */
    virtual NonlinearFunctionType getType() const = 0;

    /*
      Return a clone of the constraint.
    */
    virtual NonlinearConstraint *duplicateConstraint() const = 0;

    /*
      Restore the state of this constraint from the given one.
      We have this function in order to take advantage of the polymorphically
      correct assignment operator.
    */
    virtual void restoreState( const NonlinearConstraint *state ) = 0;

    /*
      Register/unregister the constraint with a talbeau.
    */
    virtual void registerAsWatcher( ITableau *tableau ) = 0;
    virtual void unregisterAsWatcher( ITableau *tableau ) = 0;

    /*
      The variable watcher notifcation callbacks, about a change in a variable's value or bounds.
    */
    virtual void notifyLowerBound( unsigned /* variable */, double /* bound */ ) {}
    virtual void notifyUpperBound( unsigned /* variable */, double /* bound */ ) {}

    /*
      Returns true iff the variable participates in this transcendental constraint.
    */
    virtual bool participatingVariable( unsigned variable ) const = 0;

    /*
      Get the list of variables participating in this constraint.
    */
    virtual List<unsigned> getParticipatingVariables() const = 0;

    /*
      Whether variables participating in the constraint can be eliminated by
      preprocessor.
      Caution: when implementing a new constraint class, this method should
      preferably not be overloaded.
    */
    virtual bool supportVariableElimination() const { return true; };

    /*
      Dump the current state of the constraint.
    */
    virtual void dump( String & ) const {}

    /*
      Preprocessing related functions, to inform that a variable has been eliminated completely
      because it was fixed to some value, or that a variable's index has changed (e.g., x4 is now
      called x2). constraintObsolete() returns true iff and the constraint has become obsolote
      as a result of variable eliminations.
    */
    virtual void eliminateVariable( unsigned variable, double fixedValue ) = 0;
    virtual void updateVariableIndex( unsigned oldIndex, unsigned newIndex ) = 0;
    virtual bool constraintObsolete() const = 0;

    /*
      Get the tightenings entailed by the constraint.
    */
    virtual void getEntailedTightenings( List<Tightening> &tightenings ) const = 0;

    void setStatistics( Statistics *statistics );

    /*
      Produce string representation of the transcendental constraint.
      This representation contains only the information necessary to reproduce it
      but does not account for state or change in state during execution. Additionally
      the first string before a comma has the contraint type identifier
      (ie. "sigmoid", "tanh", etc)
    */
    virtual String serializeToString() const = 0;

    /*
      Register a constraint bound tightener. If a tightener is registered,
      this transcendental constraint will inform the tightener whenever
      it discovers a tighter (entailed) bound.
    */
    void registerConstraintBoundTightener( IConstraintBoundTightener *tightener );


    /**********************************************************************/
    /*          Context-dependent Members Initialization and Cleanup      */
    /**********************************************************************/

    /*
      Register a bound manager. If a bound manager is registered,
      this transcendental constraint will inform the tightener whenever
      it discovers a tighter (entailed) bound.
    */
    void registerBoundManager( BoundManager *boundManager );

protected:
    Map<unsigned, double> _assignment;
    Map<unsigned, double> _lowerBounds;
    Map<unsigned, double> _upperBounds;

    BoundManager *_boundManager;

    IConstraintBoundTightener *_constraintBoundTightener;

    /*
      Statistics collection
    */
    Statistics *_statistics;

    /**********************************************************************/
    /*                         BOUND WRAPPER METHODS                      */
    /**********************************************************************/
    /* These methods prefer using BoundManager over local bound arrays.   */

    /*
       Checks whether lower bound value exists.

       If BoundManager is in use, returns true since it initializes bounds for all variables.
    */
    inline bool existsLowerBound( unsigned var ) const
    {
        return _boundManager != nullptr || _lowerBounds.exists( var );
    }

    /*
       Checks whether upper bound value exists.

       If BoundManager is in use, returns true since it initializes bounds for all variables.
    */
    inline bool existsUpperBound( unsigned var ) const
    {
        return _boundManager != nullptr || _upperBounds.exists( var );
    }

    /*
       Method obtains lower bound of *var*.
     */
    inline double getLowerBound( unsigned var ) const
    {
        return ( _boundManager != nullptr ) ? _boundManager->getLowerBound( var )
                                            : _lowerBounds[var];
    }

    /*
       Method obtains upper bound of *var*.
     */
    inline double getUpperBound( unsigned var ) const
    {
        return ( _boundManager != nullptr ) ? _boundManager->getUpperBound( var )
                                            : _upperBounds[var];
    }

    /*
       Method sets the lower bound of *var* to *value*.
     */
    inline void setLowerBound( unsigned var, double value )
    {
        ( _boundManager != nullptr ) ? _boundManager->setLowerBound( var, value )
                                     : _lowerBounds[var] = value;
    }

    /*
       Method sets the upper bound of *var* to *value*.
     */
    inline void setUpperBound( unsigned var, double value )
    {
        ( _boundManager != nullptr ) ? _boundManager->setUpperBound( var, value )
                                     : _upperBounds[var] = value;
    }
};

#endif // __NonlinearConstraint_h__
