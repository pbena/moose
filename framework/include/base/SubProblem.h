/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H

#include "ParallelUniqueId.h"
#include "Problem.h"
#include "DiracKernelInfo.h"
#include "Assembly.h"
#include "GeometricSearchData.h"

// libMesh include
#include "equation_systems.h"
#include "transient_system.h"
#include "nonlinear_implicit_system.h"
#include "numeric_vector.h"
#include "sparse_matrix.h"

class MooseMesh;
class SubProblem;

template<>
InputParameters validParams<SubProblem>();

/**
 * Generic class for solving transient nonlinear problems
 *
 */
class SubProblem : public Problem
{
public:
  SubProblem(const std::string & name, InputParameters parameters);
  virtual ~SubProblem();

  virtual EquationSystems & es() = 0;
  virtual MooseMesh & mesh() = 0;

  /**
   * Whether or not this problem should utilize FE shape function caching.
   *
   * @param fe_cache True for using the cache false for not.
   */
  virtual void useFECache(bool fe_cache) = 0;

  virtual void solve() = 0;
  virtual bool converged() = 0;

  virtual void onTimestepBegin() = 0;
  virtual void onTimestepEnd() = 0;

  virtual bool isTransient() = 0;

  virtual Order getQuadratureOrder() = 0;

  // Variables /////
  virtual bool hasVariable(const std::string & var_name) = 0;
  virtual MooseVariable & getVariable(THREAD_ID tid, const std::string & var_name) = 0;
  virtual bool hasScalarVariable(const std::string & var_name) = 0;
  virtual MooseVariableScalar & getScalarVariable(THREAD_ID tid, const std::string & var_name) = 0;

  /**
   * Set the MOOSE variables to be reinited on each element.
   * @param moose_vars A set of variables that need to be reinited each time reinit() is called.
   *
   * @param tid The thread id
   */
  void setActiveElementalMooseVariables(const std::set<MooseVariable *> & moose_vars, THREAD_ID tid);

  /**
   * Get the MOOSE variables to be reinited on each element.
   *
   * @param tid The thread id
   */
  const std::set<MooseVariable *> & getActiveElementalMooseVariables(THREAD_ID tid);

  /**
   * Whether or not a list of active elemental moose variables has been set.
   *
   * @return True if there has been a list of active elemental moose variables set, False otherwise
   */
  bool hasActiveElementalMooseVariables(THREAD_ID tid);

  /**
   * Clear the active elmental MooseVariable.  If there are no active variables then they will all be reinited.
   * Call this after finishing the computation that was using a restricted set of MooseVariables
   *
   * @param tid The thread id
   */
  void clearActiveElementalMooseVariables(THREAD_ID tid);

  virtual Assembly & assembly(THREAD_ID tid) = 0;
  virtual void prepareShapes(unsigned int var, THREAD_ID tid) = 0;
  virtual void prepareFaceShapes(unsigned int var, THREAD_ID tid) = 0;
  virtual void prepareNeighborShapes(unsigned int var, THREAD_ID tid) = 0;

  virtual Moose::CoordinateSystemType getCoordSystem(SubdomainID sid) = 0;
  virtual const Moose::CoordinateSystemType & coordSystem(THREAD_ID tid) = 0;
  virtual QBase * & qRule(THREAD_ID tid) = 0;
  virtual const MooseArray<Point> & points(THREAD_ID tid) = 0;
  virtual const MooseArray<Point> & physicalPoints(THREAD_ID tid) = 0;
  virtual const MooseArray<Real> & JxW(THREAD_ID tid) = 0;
  virtual const Real & elemVolume(THREAD_ID tid) = 0;
  virtual const MooseArray<Real> & coords(THREAD_ID tid) = 0;
  virtual QBase * & qRuleFace(THREAD_ID tid) = 0;
  virtual const MooseArray<Point> & pointsFace(THREAD_ID tid) = 0;
  virtual const MooseArray<Real> & JxWFace(THREAD_ID tid) = 0;
  virtual const Real & sideElemVolume(THREAD_ID tid) = 0;
  virtual const Elem * & elem(THREAD_ID tid) = 0;
  virtual unsigned int & side(THREAD_ID tid) = 0;
  virtual const Elem * & sideElem(THREAD_ID tid) = 0;
  virtual const Node * & node(THREAD_ID tid) = 0;
  virtual const Node * & nodeNeighbor(THREAD_ID tid) = 0;
  virtual DiracKernelInfo & diracKernelInfo();
  virtual Real finalNonlinearResidual();
  virtual unsigned int nNonlinearIterations();
  virtual unsigned int nLinearIterations();

  virtual void addResidual(NumericVector<Number> & residual, THREAD_ID tid) = 0;
  virtual void addResidualNeighbor(NumericVector<Number> & residual, THREAD_ID tid) = 0;

  virtual void cacheResidual(THREAD_ID tid) = 0;
  virtual void cacheResidualNeighbor(THREAD_ID tid) = 0;
  virtual void addCachedResidual(NumericVector<Number> & residual, THREAD_ID tid) = 0;

  virtual void setResidual(NumericVector<Number> & residual, THREAD_ID tid) = 0;
  virtual void setResidualNeighbor(NumericVector<Number> & residual, THREAD_ID tid) = 0;

  virtual void addJacobian(SparseMatrix<Number> & jacobian, THREAD_ID tid) = 0;
  virtual void addJacobianNeighbor(SparseMatrix<Number> & jacobian, THREAD_ID tid) = 0;
  virtual void addJacobianBlock(SparseMatrix<Number> & jacobian, unsigned int ivar, unsigned int jvar, const DofMap & dof_map, std::vector<unsigned int> & dof_indices, THREAD_ID tid) = 0;
  virtual void addJacobianNeighbor(SparseMatrix<Number> & jacobian, unsigned int ivar, unsigned int jvar, const DofMap & dof_map, std::vector<unsigned int> & dof_indices, std::vector<unsigned int> & neighbor_dof_indices, THREAD_ID tid) = 0;

  virtual void cacheJacobian(THREAD_ID tid) = 0;
  virtual void cacheJacobianNeighbor(THREAD_ID tid) = 0;
  virtual void addCachedJacobian(SparseMatrix<Number> & jacobian, THREAD_ID tid) = 0;

  virtual void prepare(const Elem * elem, THREAD_ID tid) = 0;
  virtual void prepareFace(const Elem * elem, THREAD_ID tid) = 0;
  virtual void prepare(const Elem * elem, unsigned int ivar, unsigned int jvar, const std::vector<unsigned int> & dof_indices, THREAD_ID tid) = 0;
  virtual void prepareAssembly(THREAD_ID tid) = 0;

  virtual void reinitElem(const Elem * elem, THREAD_ID tid) = 0;
  virtual void reinitElemFace(const Elem * elem, unsigned int side, BoundaryID bnd_id, THREAD_ID tid) = 0;
  virtual void reinitNode(const Node * node, THREAD_ID tid) = 0;
  virtual void reinitNodeFace(const Node * node, BoundaryID bnd_id, THREAD_ID tid) = 0;
  virtual void reinitNodes(const std::vector<unsigned int> & nodes, THREAD_ID tid) = 0;
  virtual void reinitNeighbor(const Elem * elem, unsigned int side, THREAD_ID tid) = 0;
  virtual void reinitNeighborPhys(const Elem * neighbor, unsigned int neighbor_side, const std::vector<Point> & physical_points, THREAD_ID tid) = 0;
  virtual void reinitNodeNeighbor(const Node * node, THREAD_ID tid) = 0;
  virtual void reinitScalars(THREAD_ID tid) = 0;

  /**
   * Returns true if the Problem has Dirac kernels it needs to compute on elem.
   */
  virtual bool reinitDirac(const Elem * elem, THREAD_ID tid) = 0;
  /**
   * Fills "elems" with the elements that should be looped over for Dirac Kernels
   */
  virtual void getDiracElements(std::set<const Elem *> & elems) = 0;
  /**
   * Gets called before Dirac Kernels are asked to add the points they are supposed to be evaluated in
   */
  virtual void clearDiracInfo() = 0;

  // Geom Search
  virtual void updateGeomSearch() = 0;
  virtual GeometricSearchData & geomSearchData() = 0;

  virtual void meshChanged();

  virtual void storeMatPropName(SubdomainID block_id, const std::string & name);
  virtual void checkMatProp(SubdomainID block_id, const std::string & name);

  /**
   * Will make sure that all dofs connected to elem_id are ghosted to this processor
   */
  virtual void addGhostedElem(unsigned int elem_id) = 0;

  /**
   * Will make sure that all necessary elements from boundary_id are ghosted to this processor
   */
  virtual void addGhostedBoundary(BoundaryID boundary_id) = 0;

  /**
   * Get a vector containing the block ids the material property is defined on.
   */
  virtual std::vector<SubdomainID> getMaterialPropertyBlocks(const std::string prop_name);

  /**
   * Get a vector of block id equivalences that the material property is defined on.
   */
  virtual std::vector<SubdomainName> getMaterialPropertyBlockNames(const std::string prop_name);

  /**
   * Returns true if the problem is in the process of computing it's initial residual.
   * @return Whether or not the problem is currently computing the initial residual.
   */
  virtual bool computingInitialResidual() = 0;

public:
  /**
   * Convenience zeros
   */
  MooseArray<Real> _real_zero;
  MooseArray<MooseArray<Real> > _zero;
  MooseArray<MooseArray<RealGradient> > _grad_zero;
  MooseArray<MooseArray<RealTensor> > _second_zero;

protected:
  /// Type of coordinate system per subdomain
  std::map<SubdomainID, Moose::CoordinateSystemType> _coord_sys;

  DiracKernelInfo _dirac_kernel_info;

  /// the map of material properties (block_id -> list of properties)
  std::map<unsigned int, std::set<std::string> > _map_material_props;

  /// This is the set of MooseVariables that will actually get reinited by a call to reinit(elem)
  std::vector<std::set<MooseVariable *> > _active_elemental_moose_variables;

  /// Whether or not there is currently a list of active elemental moose variables
  std::vector<bool> _has_active_elemental_moose_variables;

};


namespace Moose
{

void initial_condition(EquationSystems & es, const std::string & system_name);

} // namespace Moose


#endif /* SUBPROBLEM_H */
