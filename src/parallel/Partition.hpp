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
#ifndef __PARTITION_HPP__
#define __PARTITION_HPP__

#include <BaseManipulation.hpp>
#include <MimmoObject.hpp>

namespace mimmo{

/*!
 * \enum PartitionMethod
 * \ingroup geohandlers
 * \brief Methods available for partitioning geometry.
 */
enum class PartitionMethod{
    NONE = -1, /**< Not set partition method*/
            SERIALIZE = 0, /**< Communicate the whole mesh to rank 0*/
            PARTGEOM = 1, /**< Partition a serial geometry via geometric space filling curve*/
            CUSTOM = 99 /**< Partition a serial geometry via custom provided partitioning map*/
};

/*!
 * \class Partition
 * \ingroup geohandlers
 * \brief Partition is a class that partition a 3D geometry according to a chosen method.
 *
 * Partition is derived from BaseManipulation class.
 * It needs a target MimmoObject serial geometry, alongside a choice of a partitioning method over the processes.
 * It returns geometry partitioned over the processors in the same input MimmoObject.
 * To parallelize a serial input geometry, the geometry has to be stored entirely on processor with rank = 0.
 * The partition map is computed automatically by the block by set the PartitionMethod::PARTGEOM; a geometric
 * space filling curve is computed by the use of METIS library.
 * The user can set a custom partitioning structure by provide the map through the setPartition method of
 * the class; the function automatically sets the partition method to PartitionMethod::CUSTOM.
 * The default partition method is PartitionMethod::NONE; by leaving the method to the default value
 * the execution of the block is simply skipped.
 * After run the execution of the Partition block the original MimmoObject is replaced by the partitioned one.
 * The block can be used to serialize a partitioned mesh by set the PartitionMethod::SERIALIZE. The partitioned mesh after the
 * execution of the block will be owned entirely by rank = 0.
 * All the blocks linked to the input MimmoObject will link to the partitioned geometry after the execution of this object.
 * The Partition block has to be insert in an execution chain before the manipulation and the creation of fields on the geometry.
 * Partition plots as optional result the partitioned input geometry.
 *
 * Ports available in Partition Class :
 *
 *    =========================================================
 *
     |                 Port Input    ||                              |
     |----------|-------------------|-------------------------|
     | <B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_UMAPI  | setPartition      | (MC_UMAP, MD_INT)          |
     | M_GEOM   | setGeometry       | (MC_SCALAR, MD_MIMMO_)        |
     | M_GEOM2   | setBoundaryGeometry       | (MC_SCALAR, MD_MIMMO_)        |


     |            Port Output           ||                           |
     |----------|-------------------|-------------------------|
     |<B>PortType</B>   | <B>variable/function</B>  |<B>DataType</B> |
     | M_GEOM   | getGeometry   | (MC_SCALAR, MD_MIMMO_)        |

 *    =========================================================
 *
 * The xml available parameters, sections and subsections are the following :
 *
 * Inherited from BaseManipulation:
 * - <B>ClassName</B>: name of the class as <tt>mimmo.Partition</tt>;
 * - <B>Priority</B>: uint marking priority in multi-chain execution;
 * - <B>PlotInExecution</B> : boolean 0/1 print optional results of the class;
 * - <B>OutputPlot</B>: target directory for optional results writing.
 *
 * Proper of the class:
 * - <B>PartitionMethod</B>: Partition method.
 *
 * Geometry has to be mandatorily passed through port.
 *
 */
class Partition: public BaseManipulation{
private:
    std::unordered_map<long, int>   m_partition;            /**<Partition structure, i-th term is the final rank of the i-th cell after partitioning.*/
    PartitionMethod                 m_mode;                 /**<Partition method. Default 1 - Cartesian Axes Subdivision*/
    MimmoSharedPointer<MimmoObject> m_boundary;             /**<Reference to external boundary MimmoObject. */
    std::unordered_map<long, int>   m_boundarypartition;    /**<Partition structure for boundary geometry, i-th term is the final rank of the i-th cell after partitioning.*/

public:
    Partition();
    Partition(const bitpit::Config::Section & rootXML);
    ~Partition();

    Partition(const Partition & other);

    void buildPorts();

    MimmoSharedPointer<MimmoObject> getBoundaryGeometry();
    void setGeometry(MimmoSharedPointer<MimmoObject> geo);
    void setBoundaryGeometry(MimmoSharedPointer<MimmoObject> geo);
    void setPartitionMethod(PartitionMethod mode);
    void setPartitionMethod(int mode);
    void setPartition(std::unordered_map<long, int> partition);

    void execute();

    virtual void absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name = "");
    virtual void flushSectionXML(bitpit::Config::Section & slotXML, std::string name= "");

protected:
    virtual void plotOptionalResults();

private:
    void computePartition();
    void computeBoundaryPartition();
    void parmetisPartGeom();
    void serialPartition();
    void updateBoundaryVerticesID();
#if MIMMO_ENABLE_MPI
    void serialize(MimmoSharedPointer<MimmoObject> &geometry, bool isBoundary);
#endif


};

REGISTER_PORT(M_UMAPI, MC_UMAP, MD_INT,__PARTITION_HPP__)
REGISTER_PORT(M_GEOM, MC_SCALAR, MD_MIMMO_,__PARTITION_HPP__)
REGISTER_PORT(M_GEOM2, MC_SCALAR, MD_MIMMO_,__PARTITION_HPP__)

REGISTER(BaseManipulation, Partition, "mimmo.Partition")
}

#endif /* __PARTITION_HPP__ */
