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
\*---------------------------------------------------------------------------*/
#ifndef __STITCHGEOMETRY_HPP__
#define __STITCHGEOMETRY_HPP__

#include "BaseManipulation.hpp"

namespace mimmo{

/*!
 * \class StitchGeometry
 * \ingroup geohandlers
 * \brief StitchGeometry is an executable block class capable of
 *         stitch multiple MimmoObject geometries of the same topology
 *
 *    StitchGeometry is the object to append two or multiple MimmoObject of the same topology
 *  in a unique MimmoObject container.
 *
 * Ports available in StitchGeometry Class :
 *
 *    =========================================================

     |Port Input | | |
     |-|-|-|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_GEOM   | addGeometry                     | (MC_SCALAR, MD_MIMMO_)      |


     |Port Output | | |
     |-|-|-|
     | <B>PortType</B> | <B>variable/function</B> |<B>DataType</B>              |
     | M_GEOM    | getGeometry                        | (MC_SCALAR, MD_MIMMO_)         |

 *    =========================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.StitchGeometry</tt>
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B>: boolean 0/1 print optional results of the class.
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:
 * - <B>Topology</B>: info on admissible topology format 1-surface, 2-volume, 3-pointcloud, 4-3DCurve
 * - <B>RePID   </B>: 0-false 1-true force repidding of stitched parts. Default is 0.

 * Geometries have to be mandatorily passed through port.
 *
 */
class StitchGeometry: public BaseManipulation{

private:
    int                                     m_topo;        /**<Mark topology of your stitcher 1-surface, 2-volume, 3-pointcloud, 4-3DCurve*/
    std::unordered_map<mimmo::MimmoSharedPointer<MimmoObject>,int>    m_extgeo;    /**< pointers to external geometries*/

    mimmo::MimmoSharedPointer<MimmoObject> m_patch;    /**< resulting patch geometry */

    std::unordered_map<long, std::pair<int, long> > m_mapCellDivision; /**< division map of actual ID-cell, part Id, original ID-cell*/
    std::unordered_map<long, std::pair<int, long> > m_mapVertDivision; /**< division map of actual ID-vertex, part Id, original ID-vertex*/

    int m_geocount;                            /**<Internal geometry counter */
    bool m_repid;                              /**< force repidding of geometry */

public:
    StitchGeometry(int topo);
    StitchGeometry(const bitpit::Config::Section & rootXML);
    virtual ~StitchGeometry();

    StitchGeometry(const StitchGeometry & other);
    StitchGeometry & operator=(StitchGeometry other);

    void buildPorts();

    int                             getTopology();
    mimmo::MimmoSharedPointer<MimmoObject>    getGeometry();

    void        addGeometry(mimmo::MimmoSharedPointer<MimmoObject> geo);

    bool         isEmpty();

    void         clear();
    void         execute();
    void         forceRePID(bool flag);

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name="");
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name="");

    void     plotOptionalResults();
protected:
    void swap(StitchGeometry & x) noexcept;
};

REGISTER_PORT(M_GEOM, MC_SCALAR, MD_MIMMO_, __STITCHGEOMETRY_HPP__)


REGISTER(BaseManipulation, StitchGeometry, "mimmo.StitchGeometry")

};

#endif /* __STITCHGEOMETRY_HPP__ */
