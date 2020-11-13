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

#ifndef __MESHSELECTION_HPP__
#define __MESHSELECTION_HPP__

#include <BaseManipulation.hpp>
#include <BasicShapes.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace mimmo{

/*!
 * \ingroup geohandlers
 * \brief Enum class for choiche of method to select sub-patch
 * of a MimmoObject tessellated mesh.
 */
enum class SelectionType{
    UNDEFINED    = 0, /**< undefined selection */
    BOX          = 1, /**< box volume intersection selection */
    CYLINDER     = 2, /**< cylinder volume intersection selection */
    SPHERE       = 3, /**< sphere volume intersection selection */
    MAPPING      = 4, /**< surface mesh driven mapping selection */
    PID          = 5, /**< part identifier driven selection */
    BOXwSCALAR   = 11 /**< box volume intersection selection + scalar field eventually attached to target mesh */
};

/*!
 * \class GenericSelection
 * \ingroup geohandlers
 * \brief Abstract Interface for selection classes
 *
 * Class/BaseManipulation Object managing selection of sub-patches of MimmoObject Data structure.
 *
 * Ports available in GenericSelection Class :
 *
 *    =========================================================
 *
     |                   Port Input       ||                               |
     |----------------|----------------------|-------------------|
     |<B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output     ||                                      |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *    =========================================================
 *
 */
class GenericSelection: public mimmo::BaseManipulation {

protected:

    SelectionType                   m_type;      /**< Type of enum class SelectionType for selection method */
    mimmo::MimmoSharedPointer<MimmoObject>    m_subpatch;  /**< Pointer to result sub-patch */
    int                             m_topo;      /**< 1 = surface (default value), 2 = volume, 3 = points cloud, 4 = 3D-Curve */
    bool                            m_dual;      /**< False selects w/ current set up, true gets its "negative". False is default. */
public:

    GenericSelection();
    virtual ~GenericSelection();
    GenericSelection(const GenericSelection & other);
    GenericSelection & operator=(const GenericSelection & other);

    void    buildPorts();

    SelectionType    whichMethod();
    virtual void     setGeometry(mimmo::MimmoSharedPointer<MimmoObject>);
    void             setDual(bool flag=false);

    const mimmo::MimmoSharedPointer<MimmoObject>  getPatch()const;
    mimmo::MimmoSharedPointer<MimmoObject>        getPatch();
    bool                isDual();

    livector1D    constrainedBoundary();

    void        execute();

    virtual void plotOptionalResults();

protected:
    /*!
     * Extract selection from target geometry
     */
    virtual livector1D extractSelection() = 0;
    void swap(GenericSelection &x) noexcept;
};


/*!
 * \class SelectionByBox
 * \ingroup geohandlers
 * \brief Selection through volume box primitive.
 *
 * Selection Object managing selection of sub-patches oof a MimmoObject Data structure.
 * Select all elements contained in a box.
 *
 * Ports available in SelectionByBox Class :
 *
 *    =========================================================

     |                   Port Input  ||                                       |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_POINT        | setOrigin           | (MC_ARRAY3, MD_FLOAT)       |
     | M_AXES         | setRefSystem        | (MC_ARR3ARR3, MD_FLOAT)     |
     | M_SPAN         | setSpan             | (MC_ARRAY3, MD_FLOAT)       |


     |             Port Output        ||                                      |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *    ===============================================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionByBox</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:

 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;
 * - <B>Origin</B>: array of 3 doubles identifying origin (space separated);
 * - <B>Span</B>: span of the box (width height  depth);
 * - <B>RefSystem</B>: reference system of the box: \n\n
 *                <tt><B>\<RefSystem\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis0\></B> 1.0 0.0 0.0 <B>\</axis0\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis1\></B> 0.0 1.0 0.0 <B>\</axis1\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis2\></B> 0.0 0.0 1.0 <B>\</axis2\></B> \n
 *                  <B>\</RefSystem\></B> </tt> \n\n
 * Geometry has to be mandatorily passed through port.
 *
 */
class SelectionByBox: public GenericSelection, public mimmo::Cube {

public:
    SelectionByBox();
    SelectionByBox(const bitpit::Config::Section & rootXML);
    SelectionByBox(darray3E origin, darray3E span, mimmo::MimmoSharedPointer<MimmoObject> target);
    virtual ~SelectionByBox();
    SelectionByBox(const SelectionByBox & other);
    SelectionByBox & operator=(SelectionByBox other);

    void buildPorts();

    void clear();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

protected:
    livector1D extractSelection();
    void swap(SelectionByBox &) noexcept;
};

/*!
 * \class SelectionByCylinder
 * \ingroup geohandlers
 * \brief Selection through cylinder primitive.
 *
 * Selection Object managing selection of sub-patches oof a MimmoObject Data structure.
 * Select all elements contained in a cylinder.
 *
 * Ports available in SelectionByCylinder Class :
 *
 *    =========================================================

     |                   Port Input  ||                                       |
     |----------------|----------------------|----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_POINT        | setOrigin            | (MC_ARRAY3, MD_FLOAT)      |
     | M_AXES         | setRefSystem         | (MC_ARR3ARR3, MD_FLOAT)    |
     | M_SPAN         | setSpan              | (MC_ARRAY3, MD_FLOAT)      |
     | M_INFLIMITS    | setInfLimits         | (MC_ARRAY3, MD_FLOAT)      |

     |             Port Output        ||                                      |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *    =========================================================
 *
 * The xml available parameters, sections and subsections are the following :

 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionByCylinder</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:
 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;
 * - <B>Origin</B>: array of 3 doubles identifying origin of cylinder (space separated);
 * - <B>Span</B>: span of the cylinder (base_radius angular_azimuthal_width height);
 * - <B>RefSystem</B>: reference system of the cylinder (axis2 along the cylinder's height): \n\n
 *                <tt><B>\<RefSystem\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis0\></B> 1.0 0.0 0.0 <B>\</axis0\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis1\></B> 0.0 1.0 0.0 <B>\</axis1\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis2\></B> 0.0 0.0 1.0 <B>\</axis2\></B> \n
 *                  <B>\</RefSystem\></B> </tt> \n\n
 * - <B>InfLimits</B>: set starting point for each cylindrical coordinate. Useful to assign different starting angular coordinate to azimuthal width.
 *
 *
 * Geometry has to be mandatorily passed through port.
 *
 */
class SelectionByCylinder: public GenericSelection, public mimmo::Cylinder {

public:
    SelectionByCylinder();
    SelectionByCylinder(const bitpit::Config::Section & rootXML);
    SelectionByCylinder(darray3E origin, darray3E span, double infLimTheta, darray3E mainAxis, mimmo::MimmoSharedPointer<MimmoObject> target);
    virtual ~SelectionByCylinder();
    SelectionByCylinder(const SelectionByCylinder & other);
    SelectionByCylinder & operator=(SelectionByCylinder other);

    void buildPorts();

    void clear();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

protected:
    livector1D extractSelection();
    void swap(SelectionByCylinder &) noexcept;
};

/*!
 * \class SelectionBySphere
 * \ingroup geohandlers
 * \brief Selection through sphere primitive.
 *
 * Selection Object managing selection of sub-patches of a MimmoObject Data structure.
 * Select all elements contained in a sphere.
 *
 * Ports available in SelectionBySphere Class :
 *
 *    =========================================================
 *
     |                   Port Input  ||                                       |
     |----------------|----------------------|----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_POINT        | setOrigin            | (MC_ARRAY3, MD_FLOAT)      |
     | M_AXES         | setRefSystem         | (MC_ARR3ARR3, MD_FLOAT)    |
     | M_SPAN         | setSpan              | (MC_ARRAY3, MD_FLOAT)      |
     | M_INFLIMITS    | setInfLimits         | (MC_ARRAY3, MD_FLOAT)      |

     |             Port Output    ||                                          |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |


 *    =========================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:

 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionBySphere</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:
 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;
 * - <B>Origin</B>: array of 3 doubles identifying origin of sphere (space separated);
 * - <B>Span</B>: span of the sphere (radius angular_azimuthal_width  angular_polar_width);
 * - <B>RefSystem</B>: reference system of the sphere: \n\n
 *                <tt><B>\<RefSystem\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis0\></B> 1.0 0.0 0.0 <B>\</axis0\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis1\></B> 0.0 1.0 0.0 <B>\</axis1\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis2\></B> 0.0 0.0 1.0 <B>\</axis2\></B> \n
 *                  <B>\</RefSystem\></B> </tt> \n\n
 * - <B>InfLimits</B>: set starting point for each spherical coordinate. Useful to assign different starting angular coordinate to azimuthal width/ polar width.
 *
 *
 * Geometry has to be mandatorily passed through port.

 *
 */
class SelectionBySphere: public GenericSelection, public mimmo::Sphere {

public:
    SelectionBySphere();
    SelectionBySphere(const bitpit::Config::Section & rootXML);
    SelectionBySphere(darray3E origin, darray3E span, double infLimTheta, double infLimPhi, mimmo::MimmoSharedPointer<MimmoObject> target);
    virtual ~SelectionBySphere();
    SelectionBySphere(const SelectionBySphere & other);
    SelectionBySphere & operator=(SelectionBySphere other);

    void buildPorts();

    void clear();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

protected:
    livector1D extractSelection();
    void swap(SelectionBySphere &) noexcept;
};

/*!
 *
 * \class SelectionByMapping
 * \ingroup geohandlers
 * \brief Selection mapping external surfaces/volume/3D curves on a target mesh
 * of the same topology.
 *
 * Selection Object managing selection of sub-patches of a MimmoObject
 * unstructured surface mesh, unstructured volume mesh or 3D Curve mesh.
   Extract portion of mesh in common between a target geometry and a second one
   of the same topology, provided externally. Extraction criterium
 * is based on euclidean nearness, within a prescribed tolerance.
 * Point clouds are not suitable for this selection method.
 *
 * Ports available in SelectionByMapping Class :
 *
 *    =========================================================
 *
     |                   Port Input     ||                                                     |
     |----------------|--------------------------|----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_GEOM2        | addMappingGeometry       | (MC_SCALAR, MD_MIMMO_)     |

     |             Port Output       ||                                       |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *    =========================================================
 *
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionByMapping</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:
 * - <B>Topology</B>: number indentifying topology of tesselated mesh. 1-surfaces, 2-voume. no other types are supported;
 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;
 * - <B>Tolerance</B>: proximity threshold to activate mapping;
 * - <B>Files</B>: list of external files to map on the target surface: \n\n
        <tt><B>\<Files\></B> \n
            &nbsp;&nbsp;&nbsp;<B>\<file0\></B> \n
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<B>\<fullpath></B> absolute path to your file with extension <B>\<fullpath\></B> \n
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<B>\<tag\></B> tag extension as supported format STL,NAS,STVTU,etc...<B>\<tag\></B> \n
            &nbsp;&nbsp;&nbsp;<B>\<file0\></B> \n
            &nbsp;&nbsp;&nbsp;<B>\<file1\></B> \n
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<B>\<fullpath></B> absolute path to your file with extension <B>\<fullpath\></B> \n
            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<B>\<tag\></B> tag extension as supported format STL,NAS,STVTU,etc...<B>\<tag\></B> \n
            &nbsp;&nbsp;&nbsp;<B>\<file1\></B> \n
            &nbsp;&nbsp;&nbsp;<B>...</B> \n
        <B>\</Files\></B> </tt> \n\n
 *
 * Geometry has to be mandatorily passed through port.
 */
class SelectionByMapping: public GenericSelection {

private:
    double                                              m_tolerance;     /**< tolerance for proximity detection*/
    std::unordered_map<std::string, int>                m_geolist;      /**< list of file for geometrical proximity check*/
    std::unordered_set<mimmo::MimmoSharedPointer<MimmoObject>>    m_mimmolist;    /**< list of mimmo objects for geometrical proximity check*/
    std::vector<std::set< int > >                       m_allowedType;  /**< list of FileType actually allowed for the target geometry type*/
    std::vector<std::set< int > >                       m_allowedTopology;  /**< list of mapping geometry topology allowed by current target topology*/

public:
    SelectionByMapping(int topo = 1);
    SelectionByMapping(const bitpit::Config::Section & rootXML);
    SelectionByMapping(std::unordered_map<std::string, int> & geolist, mimmo::MimmoSharedPointer<MimmoObject> target, double tolerance);
    virtual ~SelectionByMapping();
    SelectionByMapping(const SelectionByMapping & other);
    SelectionByMapping & operator=(SelectionByMapping other);

    void    buildPorts();

    double    getTolerance();
    void     setTolerance(double tol=1.e-8);

    void     setGeometry(mimmo::MimmoSharedPointer<MimmoObject> geometry);

    const std::unordered_map<std::string, int> &     getFiles() const;
    void    setFiles(std::unordered_map<std::string,int> );
    void    addFile(std::pair<std::string,int> );
    void     removeFile(std::string);
    void     removeFiles();
    void    addMappingGeometry(mimmo::MimmoSharedPointer<MimmoObject> obj);
    void    removeMappingGeometries();

    void clear();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

protected:
    livector1D extractSelection();
    void swap(SelectionByMapping &) noexcept;

private:
    livector1D getProximity(std::pair<std::string, int> val);
    livector1D getProximity(mimmo::MimmoSharedPointer<MimmoObject> obj);
    svector1D extractInfo(std::string);
};

/*!
 * \class SelectionByPID
 * \ingroup geohandlers
 * \brief Selection using target mesh Part Identifiers.
 *
 * Selection Object managing selection of sub-patches of MimmoObject data structure
 * supporting Part IDentifiers (PID). Extract portion of mesh getting its PID in
 * target geometry. Point clouds are not suitable for this selection method.
 *
 * Ports available in SelectionByPID Class :
 *
 *    =========================================================
 *
     |                   Port Input   ||                                      |
     |----------------|----------------------|----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI2    | setPID               | (MC_VECTOR, MD_LONG)      |
     | M_VALUELI      | setPID               | (MC_SCALAR, MD_LONG)      |

     |             Port Output     ||                                         |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *    =========================================================
 *
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 *
 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionByMapping</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:
 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;
 * - <B>nPID</B>: number of PID to be selected relative to target geometry;
 * - <B>PID</B>: list of PID (separated by blank spaces) to be selected relative to target geometry;
 *
 * Geometry has to be mandatorily passed through port.
 */
class SelectionByPID: public GenericSelection {

private:
    std::unordered_set<long>       m_setPID;    /**< list of pid given by the user */
    std::unordered_map<long,bool> m_activePID; /**< list of pid available in geometry, to be flagged as active or not*/

public:
    SelectionByPID();
    SelectionByPID(const bitpit::Config::Section & rootXML);
    SelectionByPID(livector1D & pidlist, mimmo::MimmoSharedPointer<MimmoObject> target);
    virtual ~SelectionByPID();
    SelectionByPID(const SelectionByPID & other);
    SelectionByPID & operator=(SelectionByPID other);

    void    buildPorts();

    livector1D getPID();
    livector1D    getActivePID(bool active= true);

    void     setGeometry(mimmo::MimmoSharedPointer<MimmoObject>);
    void     setPID(long i = -1);
    void     setPID(livector1D);

    void     removePID(long i = -1);
    void     removePID(livector1D);

    void     syncPIDList();
    void     clear();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

protected:
    livector1D extractSelection();
    void swap(SelectionByPID &) noexcept;

};

/*!
 * \class SelectionByBoxWithScalar
 * \ingroup geohandlers
 * \brief Selection through volume box primitive.
 *
 * Selection Object managing selection of MimmoObject Data Structure.
 * Select all elements contained in a box and extract
 * a scalar field if it is present.
 *
 * Ports available in SelectionByBoxWithScalar Class :
 *
 *  =========================================================
 *
     |                   Port Input      ||                                   |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_SCALARFIELD  | setField            | (MC_SCALAR, MD_MPVECFLOAT_)       |


     |             Port Output    ||                                          |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_SCALARFIELD  | getField            | (MC_VECTOR, MD_MPVECFLOAT_)       |

   Inherited from SelectionByBox

     |                   Port Input  ||                                       |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_POINT        | setOrigin           | (MC_ARRAY3, MD_FLOAT)       |
     | M_AXES         | setRefSystem        | (MC_ARR3ARR3, MD_FLOAT)     |
     | M_SPAN         | setSpan             | (MC_ARRAY3, MD_FLOAT)       |


     |             Port Output       ||                                       |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *  ===============================================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionByBoxWithScalar</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 *Inherited from SelectionByBox:

 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;
 * - <B>Origin</B>: array of 3 doubles identifying origin (space separated);
 * - <B>Span</B>: span of the box (width height depth);
 * - <B>RefSystem</B>: reference system of the box: \n
 *                <tt><B>\<RefSystem\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis0\></B> 1.0 0.0 0.0 <B>\</axis0\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis1\></B> 0.0 1.0 0.0 <B>\</axis1\></B> \n
 *                     &nbsp;&nbsp;&nbsp;<B>\<axis2\></B> 0.0 0.0 1.0 <B>\</axis2\></B> \n
 *                  <B>\</RefSystem\></B> </tt> \n\n
 * Geometry and fields have to be mandatorily passed/recovered through ports.
 */
class SelectionByBoxWithScalar: public SelectionByBox{
protected:
    dmpvector1D     m_field;          /**<Scalar field attached to the patch
                                        (related to the whole patch before execution,
                                        related to the selected patch after execution).*/

public:
    SelectionByBoxWithScalar();
    SelectionByBoxWithScalar(const bitpit::Config::Section & rootXML);
    SelectionByBoxWithScalar(darray3E origin, darray3E span, mimmo::MimmoSharedPointer<MimmoObject> target);
    virtual ~SelectionByBoxWithScalar();
    SelectionByBoxWithScalar(const SelectionByBoxWithScalar & other);
    SelectionByBoxWithScalar & operator=(SelectionByBoxWithScalar other);

    void buildPorts();

    void clear();

    void        setField(dmpvector1D *);
    dmpvector1D  * getField();

    void execute();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

    void plotOptionalResults();
protected:
    void swap(SelectionByBoxWithScalar &) noexcept;

};

/*!
 * \class SelectionByElementList
 * \ingroup geohandlers
 * \brief Selection through list of cells/vertices of the target mesh.
 *
 * Select a subpart of the original mesh providing a list of:
  - mesh cell ids
  - mesh vertex ids (in that case all cells described by those vertices will be extracted)
 *
 * Ports available in SelectionByElementList Class :
 *
 *    =========================================================

     |                   Port Input  ||                                       |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_LONGFIELD      | addAnnotatedVertexList | (MC_SCALAR, MD_MPVECLONG_)       |
     | M_LONGFIELD2       | addAnnotatedCellList   | (MC_SCALAR, MD_MPVECLONG_)       |
     | M_VECTORLI2       | addRawVertexList       | (MC_VECTOR, MD_LONG)             |
     | M_VECTORLI3        | addRawCellList         | (MC_VECTOR, MD_LONG)             |


     |             Port Output        ||                                      |
     |----------------|---------------------|-----------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |


     Inherited from GenericSelection

     |                   Port Input   ||                                   |
     |----------------|----------------------|-------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VALUEB       | setDual              | (MC_SCALAR, MD_BOOL)    |
     | M_GEOM         | setGeometry          | (MC_SCALAR, MD_MIMMO_)  |


     |             Port Output          ||                                 |
     |----------------|---------------------|--------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_VECTORLI     | constrainedBoundary | (MC_VECTOR, MD_LONG)     |
     | M_GEOM         | getPatch            | (MC_SCALAR, MD_MIMMO_)   |

 *    ===============================================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.SelectionByBox</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:

 * - <B>Dual</B>: boolean to get straight what given by selection method or its exact dual;

 * Geometry has to be mandatorily passed through port.
 *
 */
class SelectionByElementList: public GenericSelection {

public:
    SelectionByElementList();
    SelectionByElementList(const bitpit::Config::Section & rootXML);
    virtual ~SelectionByElementList();

    void buildPorts();
    void addAnnotatedCellList(MimmoPiercedVector<long> * celldata);
    void addAnnotatedVertexList(MimmoPiercedVector<long> * vertexdata);
    void addRawCellList(std::vector<long> celldata);
    void addRawVertexList(std::vector<long> vertexdata);

    void clear();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="" );
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="" );

protected:

    std::vector<MimmoPiercedVector<long> *> m_annotatedcells;
    std::vector<MimmoPiercedVector<long> *> m_annotatedvertices;
    std::vector<std::vector<long>> m_rawcells;
    std::vector<std::vector<long>> m_rawvertices;

    livector1D extractSelection();
    void swap(SelectionByElementList &) noexcept;
};






REGISTER_PORT(M_GEOM, MC_SCALAR, MD_MIMMO_, __MESHSELECTION_HPP__)
REGISTER_PORT(M_VALUEB, MC_SCALAR, MD_BOOL, __MESHSELECTION_HPP__)
REGISTER_PORT(M_VECTORLI, MC_VECTOR, MD_LONG, __MESHSELECTION_HPP__)
REGISTER_PORT(M_VECTORLI2, MC_VECTOR, MD_LONG, __MESHSELECTION_HPP__)
REGISTER_PORT(M_VECTORLI3, MC_VECTOR, MD_LONG, __MESHSELECTION_HPP__)
REGISTER_PORT(M_POINT, MC_ARRAY3, MD_FLOAT, __MESHSELECTION_HPP__)
REGISTER_PORT(M_AXES, MC_ARR3ARR3, MD_FLOAT, __MESHSELECTION_HPP__)
REGISTER_PORT(M_SPAN, MC_ARRAY3, MD_FLOAT, __MESHSELECTION_HPP__)
REGISTER_PORT(M_INFLIMITS, MC_ARRAY3, MD_FLOAT, __MESHSELECTION_HPP__)
REGISTER_PORT(M_GEOM2, MC_SCALAR, MD_MIMMO_, __MESHSELECTION_HPP__)
REGISTER_PORT(M_VALUELI, MC_SCALAR, MD_LONG, __MESHSELECTION_HPP__)
REGISTER_PORT(M_SCALARFIELD, MC_SCALAR, MD_MPVECFLOAT_, __MESHSELECTION_HPP__)
REGISTER_PORT(M_LONGFIELD, MC_SCALAR, MD_MPVECLONG_, __MESHSELECTION_HPP__)
REGISTER_PORT(M_LONGFIELD2, MC_SCALAR, MD_MPVECLONG_, __MESHSELECTION_HPP__)


REGISTER(BaseManipulation, SelectionByBox,"mimmo.SelectionByBox")
REGISTER(BaseManipulation, SelectionByCylinder, "mimmo.SelectionByCylinder")
REGISTER(BaseManipulation, SelectionBySphere,"mimmo.SelectionBySphere")
REGISTER(BaseManipulation, SelectionByMapping, "mimmo.SelectionByMapping")
REGISTER(BaseManipulation, SelectionByPID, "mimmo.SelectionByPID")
REGISTER(BaseManipulation, SelectionByBoxWithScalar, "mimmo.SelectionByBoxWithScalar")
REGISTER(BaseManipulation, SelectionByElementList, "mimmo.SelectionByElementList")

};


#endif /* __MESHSELECTION_HPP__ */
