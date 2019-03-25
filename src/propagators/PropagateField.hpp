/*---------------------------------------------------------------------------*\
 * 
 *  mimmo
 *
 *  Copyright (C) 2015-2017 OPTIMAD engineering Srl
 *
 *  -------------------------------------------------------------------------
 *  License
 *  This file is part of mimmo.
 *
 *  mimmo is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License v3 (LGPL)
 *  as published by the Free Software Foundation.
 *
 *  mimmo is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with mimmo. If not, see <http://www.gnu.org/licenses/>.
 *
 \ *---------------------------------------------------------------------------*/
#ifndef __PROPAGATEFIELD_HPP__
#define __PROPAGATEFIELD_HPP__

#include "BaseManipulation.hpp"
#include "bitpit_LA.hpp"

namespace mimmo{
/*!
 * \class PropagateField
 * \ingroup propagators
 * \brief Executable block that provides the computation of a field
 * over a 3D volume mesh. The field is calculated solving a Laplacian problem over
 * the mesh with given Dirichlet boundary conditions.
 *
 * Class/BaseManipulation Object managing field defined on the boundaries of a 3D volume mesh.
 * It uses MimmoObject informations as input geometry.
 * The key to handle with constraints is an explicit calculation of the solution of a
 * Laplacian problem. 
 * In Laplacian solver the weights used to compute the stencil are of the form:
 * w(x) = (1/d(x))^n, where d(x) is the distance of the neighbour point to the
 * center point of the stencil and n is the weight factor, that can be tuned by the User.
 * Quality of the mesh during deformation is controlled by introducing an artificial diffusivity D (DUMPING)
 * in the laplacian calculation: div( D*grad(Phi)) = 0. Such diffusivity can be defined alternatively 
 * as variable with distance from a prescribed deforming surface (dumping surface), 
 * or with cell volumes distribution, modulated with distance from the dumping surface.
 * In the first case, cells more distant from dumping surfaces cells are forced to move more than nearer ones,
 * in the second case, bigger volume cell far from dumping surface are forced to move more.
 * Result field is stored in m_field member and returned as data field through ports.
 *
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B> : name of the class as <tt>mimmo.PropagateField</tt>;
 * - <B>Priority</B>  : uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B> : plot optional results in execution;
 * - <B>OutputPlot</B> : path to store optional results.
 *
 * Proper of the class:
 * - <B>Solver</B>        : 1-true use direct Laplacian solver, 0-false use iterative Smoother (default 0);
 * - <B>WeightConstant</B>  : coefficient used to get weights of the stencil points in function of distance (1.0 default);
 * - <B>Dumping</B>         : 1-true activate dumping control, 0-false deactivate it. 
 * - <B>DumpingType</B> : 0- distance control, 1-volume control.
 * - <B>DumpingInnerDistance</B> : inner limit of dumping function eta, if dumping is active;
 * - <B>DumpingOuterDistance</B> : outer limit of dumping function eta, if dumping is active;
 * - <B>DecayFactor</B>  : exponent to modulate dumping function (as power of), if dumping is active 
 * - <B>SmoothingSteps</B> : number of steps the Smoother solver need to perform (1 default);
 * - <B>Convergence</B> : convergence flag for smoothing solver;
 * - <B>Tolerance</B> : convergence tolerance for laplacian smoothing and direct solver;
 * Geometry, boundary surfaces, boundary condition values
 * for the target geometry have to be mandatorily passed through ports.
 *
 */
template<std::size_t NCOMP>
class PropagateField: public mimmo::BaseManipulation {

protected:

    bitpit::PiercedVector<int>   m_isbp;  /**< list of int to mark boundary points.
                                          int >0 marks boundary condition for type. For example 1 is a Dirichlet condition, 2 etc...*/
    
    MimmoPiercedVector<std::array<double, NCOMP> > m_bc_dir; /**< Dirichlet-type condition value of boundary nodes*/
    MimmoPiercedVector<std::array<double, NCOMP> > m_field;  /**< Resulting Propagated Field */
    
    int           m_np;              /**< Number of points of the cloud. */
    double        m_gamma;           /**< Coefficient used to weight the points with distance in stencil computing. */
    bool          m_laplace;         /**<Set true for laplace solver, false for smoothing solver (Laplacian solver not implemented in this version).*/
    int           m_sstep;           /**<Number of smoothing steps [default =10].*/
    bool          m_convergence;     /**<Convergence flag. If true the laplacian smoothing is solved until convergences is reached [default false].  */
    double        m_tol;             /**<Convergence tolerance. [default tol = 1.0e-05 on maximum differences (relative to maximum value on boundary conditions) of solution between two iterations].*/
    MimmoObject*  m_bsurface;        /**<Pointer to MimmoObject boundary patch identifying Dirichlet boundaries.*/
    MimmoObject*  m_dsurface;        /**<Pointer to MimmoObject with selected boundaries for dumping calculation.*/
    double        m_decayFactor;     /**<Dumping exponential factor for hybrid diffusivity region.*/
    dmpvector1D   m_dumping;         /**<Dumping field used for weights computing.*/
    double        m_radius;          /**<Outer limit distance of dumping function. At distance >= m_radius from boundary with bc != 0
                                         the stencil during the laplacian computing is the original one.*/
    double        m_plateau;        /**<Inner limit distance of dumping function. At distance <= m_plateau from boundary with bc != 0
                                         the stencil during the laplacian computing account of the maximum artificial diffusivity.*/
    bool          m_dumpingActive;  /**< true the dumping control is active, false otherwise.*/
    int           m_dumpingType;    /**< 0 distance-control, 1-volume control*/
    
    
    std::unique_ptr<bitpit::SystemSolver> m_solver; /**! linear system solver for laplace */

public:

    PropagateField();
    virtual ~PropagateField();
    PropagateField(const PropagateField & other);
    void swap(PropagateField & x) noexcept;

    //get-set methods
    void    buildPorts();
    void    setGeometry(MimmoObject * geometry_);
    void    setDirichletBoundarySurface(MimmoObject*);
    void    setDumpingBoundarySurface(MimmoObject*);
    
    void    setWeightConstant(double gamma);
    void    setSmoothingSteps(int ns);

    void    setSolver(bool solveLaplacian);
    void    setDumping(bool flag);
    void    setDumpingOuterDistance(double radius);
    void    setDumpingInnerDistance(double plateau);
    void    setDumpingType( int type=0);
    void    setDecayFactor(double decay);
    void    setConvergence(bool convergence);
    void    setTolerance(double tol);
    
    
    //XML utilities from reading writing settings to file
    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="");
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="");
    
protected:
    //cleaners and setters
    void clear();
    void setDefaults();

    virtual void computeDumpingFunction();

    void solveSmoothing(int nstep,
                        livector2D &stencils,
                        dvector2D &weights,
                        dvector1D &rhs,
                        liimap &dataInv,
                        MimmoPiercedVector<std::array<double, NCOMP> > & field);

    void solveLaplace(livector2D &stencils,
                      dvector2D &weights,
                      dvector1D &rhs,
                      liimap &dataInv,
                      MimmoPiercedVector<std::array<double, NCOMP> > & field);

    void computeStencils  (liimap &dataInv,
                           livector2D &stencils,
                           dvector2D &weights);


private:
    virtual bool checkBoundariesCoherence() = 0;

    void computeRingConnectivity(MimmoPiercedVector<livector1D> & conn);
    void computeRingWeights(MimmoPiercedVector<livector1D> & conn, MimmoPiercedVector<dvector1D> & wgt);
};

/*!
 * \class PropagateScalarField
 * \ingroup propagators
 * \brief Executable block that provides the computation of a scalar field
 * over a 3D mesh. The field is calculated solving a Laplacian problem over
 * the mesh with given Dirichlet boundary conditions.
 * 
 * Dirichlet boundary conditions are explicitly provided by the User, identifying boundaries
 * through MimmoObject patches and associating to them the value of the field as Dirichlet condition. 
 * A natural zero gradient like condition is automatically provided on unbounded borders.
 * 
 *
 * Class/BaseManipulation Object specialization of class PropagateField
 * for the propagation in a volume mesh of a scalar field.
 *
 * Ports available in PropagateScalarField Class :
 *
 *    =========================================================
 *
    |Port Input|||
    ||||
    | <B>PortType</B>| <B>variable/function</B>  |<B>DataType</B> |
    | M_GEOM         | setGeometry                           | (MC_SCALAR, MD_MIMMO_) |
    | M_GEOM2        | setDirichletBoundarySurface           | (MC_SCALAR, MD_MIMMO_) |
    | M_GEOM3        | setDumpingBoundarySurface             | (MC_SCALAR, MD_MIMMO_) |
    | M_FILTER       | setDirichletConditions                | (MC_MPVECTOR, MD_FLOAT)|

    |Port Output|||
    ||||
    | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
    | M_FILTER         | getPropagatedField                  | (MC_MPVECTOR, MD_FLOAT) |

 *    =========================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B> : name of the class as <tt>mimmo.PropagateScalarField</tt>;
 * - <B>Priority</B>  : uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B> : plot optional results in execution;
 * - <B>OutputPlot</B> : path to store optional results.
 *
 * Inherited from PropagateField:
 * - <B>Solver</B>        : 1-true use direct Laplacian solver, 0-false use iterative Smoother (default 0);
 * - <B>WeightConstant</B>  : coefficient used to get weights of the stencil points in function of distance (1.0 default);
 * - <B>Dumping</B>         : 1-true activate dumping control, 0-false deactivate it. 
 * - <B>DumpingType</B> : 0- distance control, 1-volume control.
 * - <B>DumpingInnerDistance</B> : inner limit of dumping function eta, if dumping is active;
 * - <B>DumpingOuterDistance</B> : outer limit of dumping function eta, if dumping is active;
 * - <B>DecayFactor</B>  : exponent to modulate dumping function (as power of), if dumping is active 
 * - <B>SmoothingSteps</B> : number of steps the Smoother solver need to perform (1 default);
 * - <B>Convergence</B> : convergence flag for smoothing solver;
 * - <B>Tolerance</B> : convergence tolerance for laplacian smoothing and direct solver;
 
 * Geometry, boundary surfaces, boundary condition values
 * for the target geometry have to be mandatorily passed through ports.
 *
 */
class PropagateScalarField: public mimmo::PropagateField<1> {

public:

    PropagateScalarField();
    PropagateScalarField(const bitpit::Config::Section & rootXML);
    virtual ~PropagateScalarField();
    PropagateScalarField(const PropagateScalarField & other);
    PropagateScalarField & operator=(PropagateScalarField other);
    void swap(PropagateScalarField & x) noexcept;

    void buildPorts();

    dmpvector1D getPropagatedField();

    void    setDirichletConditions(dmpvector1D bc);

    //execute
    void        execute();

    //XML utilities from reading writing settings to file
    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="");
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="");

protected:
    //cleaners and setters
    void clear();
    
    bool checkBoundariesCoherence();

    virtual void plotOptionalResults();

private:
    //execute
    virtual void correctStencils(liimap & dataInv,
                                 livector2D &stencils,
                                 dvector2D &weights);
    
    void computeRHS       (MimmoPiercedVector<std::array<double, 1> > & bcs,
                           liimap &dataInv,
                           dvector1D &rhs);
    
};


/*!
 * \class PropagateVectorField
 * \ingroup propagators
 * \brief Executable block that provides the computation of a 3D array field
 * over a 3D mesh. The field is calculated solving a Laplacian problem over
 * the mesh with given boundary conditions.
 * 
 * Prescribed (Dirichlet) field boundary conditions are explicitly provided by the User, 
 * identifying boundaries through MimmoObject patches and associating to them the value of each component 
 * of the field as Dirichlet condition.
 * Optionally an inpermeability-like/slip condition (zero vector field normal to boundary surface) 
 * can be imposed on chosen boundary patches.
 * 
 * The block can perform multistep evaluation to relax field propagation
 *  
 * Class/BaseManipulation Object specialization of class PropagateField
 * for the propagation in a volume mesh of a 3D array field.
 *
 * Ports available in PropagateVectorField Class :
 *
 *    =========================================================
 *
    | Port Input|||
    ||||
    | <B>PortType</B> | <B>variable/function</B> |<B>DataType</B>             |
    | M_GEOM          | setGeometry                 | (MC_SCALAR, MD_MIMMO_)  |
    | M_GEOM2         | setDirichletBoundarySurface |(MC_SCALAR, MD_MIMMO_)   |
    | M_GEOM3         | setDumpingBoundarySurface   | (MC_SCALAR, MD_MIMMO_)  |
    | M_GDISPLS       | setDirichletConditions      | (MC_MPVECARR3, MD_FLOAT)|
    | M_GEOM4         | setSlipBoundarySurface      | (MC_SCALAR, MD_MIMMO_)  |

    |Port Output|||
    ||||
    | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
    | M_GDISPLS         | getPropagatedField   | (MC_MPVECARR3, MD_FLOAT) |

 *    =========================================================
 *
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B> : name of the class as <tt>mimmo.PropagateVectorField</tt>;
 * - <B>Priority</B>  : uint marking priority in multi-chain execution;
 * - <B>Apply</B> : if set to 1, apply propagated field to target geometry as a deformation field;
 * - <B>PlotInExecution</B> : plot optional results in execution;
 * - <B>OutputPlot</B> : path to store optional results.
 *
 * Inherited from PropagateField:
 * - <B>Solver</B>        : 1-true use direct Laplacian solver, 0-false use iterative Smoother (default 0);
 * - <B>WeightConstant</B>  : coefficient used to get weights of the stencil points in function of distance (1.0 default);
 * - <B>Dumping</B>         : 1-true activate dumping control, 0-false deactivate it. 
 * - <B>DumpingType</B> : 0- distance control, 1-volume control.
 * - <B>DumpingInnerDistance</B> : inner limit of dumping function eta, if dumping is active;
 * - <B>DumpingOuterDistance</B> : outer limit of dumping function eta, if dumping is active;
 * - <B>DecayFactor</B>  : exponent to modulate dumping function (as power of), if dumping is active 
 * - <B>SmoothingSteps</B> : number of steps the Smoother solver need to perform (1 default);
 * - <B>Convergence</B> : convergence flag for smoothing solver;
 * - <B>Tolerance</B> : convergence tolerance for laplacian smoothing and direct solver;
 *
 * Proper fo the class:
 * - <B>MultiStep</B> : got deformation in a finite number of substep of solution;
 * - <B>SlipNormalRatio</B> : value > 1, meant to adjust defects in vertex-normals of candidate slipsurface. if the rate between 
 *                            the maximum component of the normal and the candidate component a is greater then this value, 
 *                            the candidate component will be set to 0, and the normal will be recalculated.
 *
 * Geometry, boundary surfaces, boundary condition values
 * for the target geometry have to be mandatorily passed through ports.
 *
 */
class PropagateVectorField: public mimmo::PropagateField<3> {

protected:

    MimmoObject * m_slipsurface;         /**< MimmoObject boundary patch identifying slip conditions */
    int           m_nstep;               /**! multistep solver */
    bitpit::PiercedVector<darray3E> m_vNormals; /**< temporary object to store vertex normals for slip condition */
    double m_slipratio;                         /**< ratio to correct normals of slip-surfaces*/

public:

    PropagateVectorField();
    PropagateVectorField(const bitpit::Config::Section & rootXML);
    virtual ~PropagateVectorField();
    PropagateVectorField(const PropagateVectorField & other);
    PropagateVectorField & operator=(PropagateVectorField other);
    void swap(PropagateVectorField & x) noexcept;

    void buildPorts();

    dmpvecarr3E getPropagatedField();
    
    void    setSlipBoundarySurface(MimmoObject *);
    void    setSlipNormalRatio(double thres);
    
    void    setDirichletConditions(dmpvecarr3E bc);
    
    void    setSolverMultiStep(unsigned int sstep);
    
    //execute
    void        execute();
    
    //XML utilities from reading writing settings to file
    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="");
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="");

protected:
    //cleaners and setters
    void clear();
    void setDefaults();

    bool checkBoundariesCoherence();

    void apply();

    virtual void plotOptionalResults();

    void subdivideBC();
    void restoreBC();
    void restoreGeometry(bitpit::PiercedVector<bitpit::Vertex> & vertices);
    
private:
    //execute
    virtual void correctStencils(liimap & dataInv,
                                 livector2D &stencils,
                                 dvector2D &weights);
    
    virtual void computeRHS (MimmoPiercedVector<std::array<double, 3> > & bcs,
                             liimap &dataInv,
                             dvector1D &rhs);

};

REGISTER_PORT(M_GEOM, MC_SCALAR, MD_MIMMO_,__PROPAGATEFIELD_HPP__)
REGISTER_PORT(M_GEOM2, MC_SCALAR, MD_MIMMO_,__PROPAGATEFIELD_HPP__)
REGISTER_PORT(M_GEOM3, MC_SCALAR, MD_MIMMO_,__PROPAGATEFIELD_HPP__)
REGISTER_PORT(M_GEOM4, MC_SCALAR, MD_MIMMO_,__PROPAGATEFIELD_HPP__)
REGISTER_PORT(M_FILTER, MC_MPVECTOR, MD_FLOAT,__PROPAGATEFIELD_HPP__)
REGISTER_PORT(M_GDISPLS, MC_MPVECARR3, MD_FLOAT,__PROPAGATEFIELD_HPP__)

REGISTER(BaseManipulation, PropagateScalarField, "mimmo.PropagateScalarField")
REGISTER(BaseManipulation, PropagateVectorField, "mimmo.PropagateVectorField")

};

#include "PropagateField.tpp"

#endif /* __PROPAGATEFIELD_HPP__ */
