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
#include "MimmoObject.hpp"
#include "MimmoNamespace.hpp"
#include "SkdTreeUtils.hpp"
#if MIMMO_ENABLE_MPI
#include "communications.hpp"
#endif
#include <Operators.hpp>
#include <set>
#include <cassert>

namespace mimmo{

/*!
 * MimmoSurfUnstructured default constructor
 */
MimmoSurfUnstructured::MimmoSurfUnstructured():
#if MIMMO_ENABLE_MPI
            bitpit::SurfUnstructured(MPI_COMM_NULL){}
#else
            bitpit::SurfUnstructured(){}
#endif

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] dimension dimensionality of elements (2D-triangles/quads/polygons, 1D-lines)
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(int dimension):
#if MIMMO_ENABLE_MPI
                MimmoSurfUnstructured(dimension, MPI_COMM_NULL){}
#else
                bitpit::SurfUnstructured(dimension, 3){}
#endif

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] id custom identification label of the mesh
 * \param[in] dimension dimensionality of elements (2D-triangles/quads/polygons, 1D-lines)
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(int id, int dimension):
#if MIMMO_ENABLE_MPI
                MimmoSurfUnstructured(id, dimension, MPI_COMM_NULL){}
#else
                bitpit::SurfUnstructured(id, dimension, 3){}
#endif

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] stream input stream where reading from
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(std::istream & stream):
#if MIMMO_ENABLE_MPI
                MimmoSurfUnstructured(stream, MPI_COMM_NULL){}
#else
                bitpit::SurfUnstructured(stream){}
#endif

#if MIMMO_ENABLE_MPI
/*!
 * MimmoSurfUnstructured default constructor
 * \param communicator MPI communicator assigned to the patch
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(MPI_Comm communicator):
        bitpit::SurfUnstructured(communicator){}

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] dimension dimensionality of elements (2D-triangles/quads/polygons, 1D-lines)
 * \param communicator MPI communicator assigned to the patch
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(int dimension, MPI_Comm communicator):
            bitpit::SurfUnstructured(dimension, 3, communicator){}

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] id custom identification label of the mesh
 * \param[in] dimension dimensionality of elements (2D-triangles/quads/polygons, 1D-lines)
 * \param communicator MPI communicator assigned to the patch
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(int id, int dimension, MPI_Comm communicator):
            bitpit::SurfUnstructured(id, dimension, 3, communicator){}

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] stream input stream where reading from
 * \param communicator MPI communicator assigned to the patch
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(std::istream & stream, MPI_Comm communicator):
            bitpit::SurfUnstructured(stream, communicator){}

#endif

/*!
 * Basic Destructor
 */
MimmoSurfUnstructured::~MimmoSurfUnstructured(){}

/*!
 * Cloning a MimmoSurfUnstructured in an independent object of base type
 * bitpit::PatchKernel
 */
std::unique_ptr<bitpit::PatchKernel>
MimmoSurfUnstructured::clone() const{
	return bitpit::SurfUnstructured::clone();
}

/*!
 * MimmoVolUnstructured default constructor
 * \param[in] dimension dimensionality of elements (3- 3D tetrahedra/hexahedra ..., 2- 2D triangles/quads/polygons)
 */
MimmoVolUnstructured::MimmoVolUnstructured(int dimension):
#if MIMMO_ENABLE_MPI
                MimmoVolUnstructured(dimension, MPI_COMM_NULL){}
#else
				bitpit::VolUnstructured(dimension){}
#endif

/*!
 * MimmoVolUnstructured custom constructor
 * \param[in] id custom identification label of the mesh
 * \param[in] dimension dimensionality of elements (3- 3D tetrahedra/hexahedra ..., 2- 2D triangles/quads/polygons)
 */
MimmoVolUnstructured::MimmoVolUnstructured(int id, int dimension):
#if MIMMO_ENABLE_MPI
                MimmoVolUnstructured(id, dimension, MPI_COMM_NULL){}
#else
                bitpit::VolUnstructured(id, dimension){}
#endif

#if MIMMO_ENABLE_MPI

/*!
 * MimmoVolUnstructured custom constructor
 * \param[in] dimension dimensionality of elements (3- 3D tetrahedra/hexahedra ..., 2- 2D triangles/quads/polygons)
 * \param communicator MPI communicator assigned to the patch
 */
MimmoVolUnstructured::MimmoVolUnstructured(int dimension, MPI_Comm communicator):
            bitpit::VolUnstructured(dimension, communicator){}

/*!
 * MimmoVolUnstructured custom constructor
 * \param[in] id custom identification label of the mesh
 * \param[in] dimension dimensionality of elements (3- 3D tetrahedra/hexahedra ..., 2- 2D triangles/quads/polygons)
 * \param communicator MPI communicator assigned to the patch
 */
MimmoVolUnstructured::MimmoVolUnstructured(int id, int dimension, MPI_Comm communicator):
            bitpit::VolUnstructured(id, dimension, communicator){}

#endif

/*!
 * Basic Destructor
 */
MimmoVolUnstructured::~MimmoVolUnstructured(){}

/*!
 * Cloning a MimmoVolUnstructured in an independent object of base type
 * bitpit::PatchKernel
 */
std::unique_ptr<bitpit::PatchKernel>
MimmoVolUnstructured::clone() const{
	return bitpit::VolUnstructured::clone();
}

/*!
 * MimmoPointCloud basic constructor
 */
MimmoPointCloud::MimmoPointCloud():
#if MIMMO_ENABLE_MPI
                MimmoPointCloud(MPI_COMM_NULL){}
#else
                bitpit::SurfUnstructured(2, 3){}
#endif

/*!
 * MimmoPointCloud custom constructor
 * \param[in] id custom identification label of the mesh
 */
MimmoPointCloud::MimmoPointCloud(int id):
#if MIMMO_ENABLE_MPI
                MimmoPointCloud(id, MPI_COMM_NULL){}
#else
                bitpit::SurfUnstructured(id, 2, 3){}
#endif

#if MIMMO_ENABLE_MPI
/*!
 * MimmoPointCloud default constructor
 * \param communicator MPI communicator assigned to the patch
 */
MimmoPointCloud::MimmoPointCloud(MPI_Comm communicator):
        bitpit::SurfUnstructured(2, 3, communicator){}

/*!
 * MimmoPointCloud custom constructor
 * \param[in] id custom identification label of the mesh
 * \param communicator MPI communicator assigned to the patch
 */
MimmoPointCloud::MimmoPointCloud(int id, MPI_Comm communicator):
            bitpit::SurfUnstructured(id, 2, 3, communicator){}

#endif

/*!
 * Basic Destructor
 */
MimmoPointCloud::~MimmoPointCloud(){}

/*!
 * Cloning a MimmoPointCloud in an independent object of base type
 * bitpit::PatchKernel
 */
std::unique_ptr<bitpit::PatchKernel>
MimmoPointCloud::clone() const{
	return bitpit::SurfUnstructured::clone();
}


/*!
 * Default constructor of MimmoObject.
 * It requires a int flag identifying the type of mesh meant to be created:
 *  - surface unstructured mesh = 1
 *  - volume unstructured mesh  = 2
 *  - 3D Cloud Point            = 3
 *  - 3D tessellated Curve      = 4
 *
 * \param[in] type type of mesh
 * \param[in] isParallel if true construct a parallel MimmoObject (if MPI not enabled forced to false)
 */
MimmoObject::MimmoObject(int type, bool isParallel):m_patch(nullptr),m_extpatch(nullptr){

    // Initialize parallel data
    m_isParallel = isParallel && MIMMO_ENABLE_MPI;
#if MIMMO_ENABLE_MPI
    initializeMPI();
    MPI_Comm_dup(MPI_COMM_WORLD, &m_communicator);
    initializeCommunicator(m_isParallel);
#else
    m_rank = 0;
    m_nprocs = 1;
#endif

	m_type = std::max(type,1);
	switch(m_type){
	case 1:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(2, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(2)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoVolUnstructured(3, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoVolUnstructured(3)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::VolumeSkdTree(dynamic_cast<bitpit::VolumeKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoPointCloud(getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoPointCloud()));
#endif
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(1, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(1)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
        (*m_log)<<"Error MimmoObject: unrecognized data structure type in class construction."<<std::endl;
        throw std::runtime_error ("MimmoObject : unrecognized mesh type in class construction");
		break;
	}

	m_internalPatch = true;

    initializeLogger();

    // Set skdTreeSync and kdTreeSync to none
    m_skdTreeSync = SyncStatus::NONE;
    m_kdTreeSync = SyncStatus::NONE;

    // Set skdTreeSync to unsupported for point cloud
    if (m_type == 3){
        m_skdTreeSync = SyncStatus::NOTSUPPORTED;
    }

    // Set AdjSync and IntSync to none
    m_AdjSync = SyncStatus::NONE;
	m_IntSync = SyncStatus::NONE;

	m_patchInfo.setPatch(m_patch.get());
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;
    m_boundingBoxSync = SyncStatus::NONE;
	m_pointConnectivitySync = SyncStatus::NONE;

	setTolerance(1.0e-06);
}

/*!
 * Custom constructor of MimmoObject.
 * This constructor builds a generic mesh from given vertex list and its related
 * local connectivity. Mesh-element details are desumed by local cell-vertices connectivity size.
 * Connectivities supported can be of any kind. Mixed elements are allowed.
 * Cell element types supported are all those of bitpit::ElementType list. In particular:
 *  - any 2D cell for surface meshes
 *  - any 3D cell for volume meshes
 *  - VERTEX only for 3D point clouds
 *  - LINE cells only for 3D curves
 *
 * Cloud points are always supported (no connectivity field must be provided)
 * Type of meshes supported are described in the default MimmoObject constructor documentation.
 * Please note, despite type argument, if null connectivity structure is provided, MimmoObject
 * will be builtas a standard cloud point of vertices provided by vertex.
 * Connectivity for each standard cell is simply defined as a list of vertex indices
 * which compose it (indices are meant as positions in the provided vertex list argument).
 * Polygonal and Polyhedral cells requires a special convention to define their connectivity:
 * - polygons: require on top of the list the total number of vertices
 *              which compose it,followed by the indices in the vertex
                list (ex. polygon with 5 vertices: 5 i1 i2 i3 i4 i5).
 * - polyhedra: require on top the total number of faces, followed by
                the number of vertices which composes the local face +
                the vertex indices which compose the faces, and so on
                for all the faces which compose the polyhedron
                (ex. polyhedron with 3 faces, first face is a triangle,
                second is a quad etc...  3 3 i1 i2 i3 4 i2 i3 i4 i5 ...)

 * \param[in] type type of meshes.
 * \param[in] vertex Coordinates of geometry vertices.
 * \param[in] connectivity pointer to mesh connectivity list (optional).
 * \param[in] isParallel if true construct a parallel MimmoObject (if MPI not enabled forced to false)
 */
MimmoObject::MimmoObject(int type, dvecarr3E & vertex, livector2D * connectivity, bool isParallel):m_patch(nullptr),m_extpatch(nullptr){

    // Initialize parallel data
    m_isParallel = isParallel && MIMMO_ENABLE_MPI;
#if MIMMO_ENABLE_MPI
    initializeMPI();
    initializeCommunicator(m_isParallel);
#else
    m_rank = 0;
    m_nprocs = 1;
#endif

	m_type = std::max(type,1);
	switch(m_type){
	case 1:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(2, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(2)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoVolUnstructured(3, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoVolUnstructured(3)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::VolumeSkdTree(dynamic_cast<bitpit::VolumeKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoPointCloud(getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoPointCloud()));
#endif
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(1, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(1)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
        (*m_log)<<"Error MimmoObject: unrecognized data structure type in class construction."<<std::endl;
        throw std::runtime_error ("MimmoObject : unrecognized mesh type in class construction");
		break;
	}

	m_internalPatch = true;

    initializeLogger();

    // Set skdTreeSync and kdTreeSync to none
    m_skdTreeSync = SyncStatus::NONE;
    m_kdTreeSync = SyncStatus::NONE;

    // Set skdTreeSync to unsupported for point cloud
    if (m_type != 3){
        m_skdTreeSync = SyncStatus::NOTSUPPORTED;
    }

    // Set AdjSync and IntSync to none
    m_AdjSync = SyncStatus::NONE;
    m_IntSync = SyncStatus::NONE;

	bitpit::ElementType eltype;
	std::size_t sizeVert = vertex.size();

	m_patch->reserveVertices(sizeVert);

	for(const auto & vv : vertex)   addVertex(vv);

	m_log->setPriority(bitpit::log::Priority::DEBUG);

	livector1D temp;
	std::size_t sizeCell = connectivity->size();
	m_patch->reserveCells(sizeCell);
	for(auto const & cc : *connectivity){
		eltype = desumeElement(cc);
		if(eltype != bitpit::ElementType::UNDEFINED){
			addConnectedCell(cc, eltype); //if MPI, is adding cell with rank =-1, that is is using local m_rank.
		}else{
			(*m_log)<<"warning: in MimmoObject custom constructor. Undefined cell type detected and skipped."<<std::endl;
		}
	}

	m_pidsType.insert(0);
	m_pidsTypeWNames.insert(std::make_pair(0,""));

	m_log->setPriority(bitpit::log::Priority::NORMAL);

	m_patchInfo.setPatch(m_patch.get());
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;
    m_boundingBoxSync = SyncStatus::NONE;
	m_pointConnectivitySync = SyncStatus::NONE;

    setTolerance(1.0e-06);

};

/*!
 * Custom constructor of MimmoObject.
 * This constructor builds a geometry data structure soft linking to an external
   bitpit::PatchKernel object, i.e. it does not own the geometry data structure,
   but simple access it, while it is instantiated elsewhere.
 * If a null or empty or not coherent geometry patch is linked, throw an error exception.

 * \param[in] type type of mesh
 * \param[in] geometry pointer to a geometry of class PatchKernel to be linked.
 */
MimmoObject::MimmoObject(int type, bitpit::PatchKernel* geometry):m_patch(nullptr),m_extpatch(nullptr){

    m_type = std::max(type,1);
    if (m_type > 4 || geometry == nullptr){
        throw std::runtime_error ("MimmoObject : unrecognized mesh type or nullptr argument in class construction");
    }

    m_internalPatch = false;
    m_extpatch = geometry;

    // Initialize parallel data
    m_isParallel = false;
#if MIMMO_ENABLE_MPI
    m_isParallel = geometry->getCommunicator() != MPI_COMM_NULL;
    initializeMPI();
    initializeCommunicator(m_isParallel);
#else
    m_rank = 0;
    m_nprocs = 1;
#endif

    initializeLogger();

	//check among elements if they are coherent with the type currently hold by the linked mesh.
	std::unordered_set<int> mapEle = elementsMap(*geometry);
	switch(m_type){
	case 1:
		if( mapEle.count((int)bitpit::ElementType::VERTEX) > 0      ||
				mapEle.count((int)bitpit::ElementType::LINE) > 0        ||
				mapEle.count((int)bitpit::ElementType::TETRA) > 0       ||
				mapEle.count((int)bitpit::ElementType::HEXAHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::VOXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::WEDGE) > 0       ||
				mapEle.count((int)bitpit::ElementType::PYRAMID) > 0     ||
				mapEle.count((int)bitpit::ElementType::POLYHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported Elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported Elements for required type in linked mesh.");
		}
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(geometry))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		if( mapEle.count((int)bitpit::ElementType::VERTEX) > 0      ||
				mapEle.count((int)bitpit::ElementType::LINE) > 0        ||
				mapEle.count((int)bitpit::ElementType::TRIANGLE) > 0    ||
				mapEle.count((int)bitpit::ElementType::QUAD) > 0        ||
				mapEle.count((int)bitpit::ElementType::PIXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::POLYGON) > 0     ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported Elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported Elements for required type in linked mesh.");
		}
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::VolumeSkdTree(dynamic_cast<bitpit::VolumeKernel*>(geometry))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		if( mapEle.count((int)bitpit::ElementType::LINE) > 0        ||
				mapEle.count((int)bitpit::ElementType::TRIANGLE) > 0    ||
				mapEle.count((int)bitpit::ElementType::QUAD) > 0        ||
				mapEle.count((int)bitpit::ElementType::PIXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::POLYGON) > 0     ||
				mapEle.count((int)bitpit::ElementType::TETRA) > 0       ||
				mapEle.count((int)bitpit::ElementType::HEXAHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::VOXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::WEDGE) > 0       ||
				mapEle.count((int)bitpit::ElementType::PYRAMID) > 0     ||
				mapEle.count((int)bitpit::ElementType::POLYHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported elements for required type in linked mesh.");
		}
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		if( mapEle.count((int)bitpit::ElementType::VERTEX) > 0      ||
				mapEle.count((int)bitpit::ElementType::TRIANGLE) > 0    ||
				mapEle.count((int)bitpit::ElementType::QUAD) > 0        ||
				mapEle.count((int)bitpit::ElementType::PIXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::POLYGON) > 0     ||
				mapEle.count((int)bitpit::ElementType::TETRA) > 0       ||
				mapEle.count((int)bitpit::ElementType::HEXAHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::VOXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::WEDGE) > 0       ||
				mapEle.count((int)bitpit::ElementType::PYRAMID) > 0     ||
				mapEle.count((int)bitpit::ElementType::POLYHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported elements for required type in linked mesh.");
		}
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(geometry))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

    // Set skdTreeSync and kdTreeSync to none
    m_skdTreeSync = SyncStatus::NONE;
    m_kdTreeSync = SyncStatus::NONE;

    // Set skdTreeSync to unsupported for point cloud
    if (m_type == 3){
        m_skdTreeSync = SyncStatus::NOTSUPPORTED;
    }

    // Check if adjacencies and interfaces are built.(Patch called it BuildStrategy -- NONE is unbuilt)
    if (geometry->getAdjacenciesBuildStrategy() == bitpit::PatchKernel::AdjacenciesBuildStrategy::ADJACENCIES_NONE){
        m_AdjSync = SyncStatus::NONE;
    }
    else{
        // Set to UNSYNC to safeness
        m_AdjSync = SyncStatus::UNSYNC;
    }

    if (geometry->getInterfacesBuildStrategy() == bitpit::PatchKernel::InterfacesBuildStrategy::INTERFACES_NONE){
        m_IntSync = SyncStatus::NONE;
    }
    else{
        // Set to UNSYNC to safeness
        m_IntSync = SyncStatus::UNSYNC;
    }

	//recover cell PID
	for(const auto &cell : getPatch()->getCells()){
		auto PID = cell.getPID();
		m_pidsType.insert((long)PID);
		m_pidsTypeWNames.insert(std::make_pair( (long)PID , "") );
	}

	m_patchInfo.setPatch(m_extpatch);
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;
    m_boundingBoxSync = SyncStatus::UNSYNC;
	m_pointConnectivitySync = SyncStatus::NONE;

    setTolerance(1.0e-06);

}

/*!
 * Custom constructor of MimmoObject.
 * This constructor builds a geometry data structure owning an external
   bitpit::PatchKernel object, i.e.it takes the ownership of the geometry data
   structure treating it as an internal structure, and it will be responsible
   of its own destruction.
 * If a null or empty or not coherent geometry patch is linked, throw an error exception.

 * \param[in] type type of mesh
 * \param[in] geometry unique pointer to a geometry of class PatchKernel to be linked.
 */
MimmoObject::MimmoObject(int type, std::unique_ptr<bitpit::PatchKernel> & geometry):m_patch(nullptr),m_extpatch(nullptr){

	if (!geometry){
		throw std::runtime_error ("MimmoObject : nullptr argument in class construction");
	}

    m_internalPatch = true;
    m_patch = std::move(geometry);

    // Initialize parallel data
    m_isParallel = false;
#if MIMMO_ENABLE_MPI
    m_isParallel = getPatch()->getCommunicator() != MPI_COMM_NULL;
    initializeMPI();
    initializeCommunicator(m_isParallel);
#else
    m_rank = 0;
    m_nprocs = 1;
#endif

	//check among elements if they are coherent with the type currently hold by the linked mesh.
    m_type = std::max(type,1);
	std::unordered_set<int> mapEle = elementsMap(*(getPatch()));
	switch(m_type){
	case 1:
		if(mapEle.count((int)bitpit::ElementType::VERTEX) > 0      ||
				mapEle.count((int)bitpit::ElementType::LINE) > 0        ||
				mapEle.count((int)bitpit::ElementType::TETRA) > 0       ||
				mapEle.count((int)bitpit::ElementType::HEXAHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::VOXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::WEDGE) > 0       ||
				mapEle.count((int)bitpit::ElementType::PYRAMID) > 0     ||
				mapEle.count((int)bitpit::ElementType::POLYHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported Elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported Elements for required type in linked mesh.");
		}
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(getPatch()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		if( mapEle.count((int)bitpit::ElementType::VERTEX) > 0      ||
				mapEle.count((int)bitpit::ElementType::LINE) > 0        ||
				mapEle.count((int)bitpit::ElementType::TRIANGLE) > 0    ||
				mapEle.count((int)bitpit::ElementType::QUAD) > 0        ||
				mapEle.count((int)bitpit::ElementType::PIXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::POLYGON) > 0     ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported Elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported Elements for required type in linked mesh.");
		}
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::VolumeSkdTree(dynamic_cast<bitpit::VolumeKernel*>(getPatch()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		if( mapEle.count((int)bitpit::ElementType::LINE) > 0      ||
				mapEle.count((int)bitpit::ElementType::TRIANGLE) > 0    ||
				mapEle.count((int)bitpit::ElementType::QUAD) > 0        ||
				mapEle.count((int)bitpit::ElementType::PIXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::POLYGON) > 0     ||
				mapEle.count((int)bitpit::ElementType::TETRA) > 0       ||
				mapEle.count((int)bitpit::ElementType::HEXAHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::VOXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::WEDGE) > 0       ||
				mapEle.count((int)bitpit::ElementType::PYRAMID) > 0     ||
				mapEle.count((int)bitpit::ElementType::POLYHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported elements for required type in linked mesh.");
		}		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		if( mapEle.count((int)bitpit::ElementType::VERTEX) > 0      ||
				mapEle.count((int)bitpit::ElementType::TRIANGLE) > 0    ||
				mapEle.count((int)bitpit::ElementType::QUAD) > 0        ||
				mapEle.count((int)bitpit::ElementType::PIXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::POLYGON) > 0     ||
				mapEle.count((int)bitpit::ElementType::TETRA) > 0       ||
				mapEle.count((int)bitpit::ElementType::HEXAHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::VOXEL) > 0       ||
				mapEle.count((int)bitpit::ElementType::WEDGE) > 0       ||
				mapEle.count((int)bitpit::ElementType::PYRAMID) > 0     ||
				mapEle.count((int)bitpit::ElementType::POLYHEDRON) > 0  ||
				mapEle.count((int)bitpit::ElementType::UNDEFINED) > 0    )
		{
			(*m_log)<<"Error MimmoObject: unsupported elements for required type in linked mesh."<<std::endl;
			throw std::runtime_error ("MimmoObject :unsupported elements for required type in linked mesh.");
		}
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(getPatch()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
        throw std::runtime_error ("MimmoObject : unrecognized mesh type in class construction");
		break;
	}

    initializeLogger();

    // Set skdTreeSync and kdTreeSync to none
    m_skdTreeSync = SyncStatus::NONE;
    m_kdTreeSync = SyncStatus::NONE;

    // Set skdTreeSync to unsupported for point cloud
    if (m_type == 3){
        m_skdTreeSync = SyncStatus::NOTSUPPORTED;
    }

    // Check if adjacencies and interfaces are built.(Patch called it BuildStrategy -- NONE is unbuilt)
    if (getPatch()->getAdjacenciesBuildStrategy() == bitpit::PatchKernel::AdjacenciesBuildStrategy::ADJACENCIES_NONE){
        m_AdjSync = SyncStatus::NONE;
    }
    else{
        // Set to UNSYNC to safeness
        m_AdjSync = SyncStatus::UNSYNC;
    }

    if (getPatch()->getInterfacesBuildStrategy() == bitpit::PatchKernel::InterfacesBuildStrategy::INTERFACES_NONE){
        m_IntSync = SyncStatus::NONE;
    }
    else{
        // Set to UNSYNC to safeness
        m_IntSync = SyncStatus::UNSYNC;
    }

	//recover cell PID
	for(const auto &cell : getPatch()->getCells()){
		auto PID = cell.getPID();
		m_pidsType.insert((long)PID);
		m_pidsTypeWNames.insert(std::make_pair( (long)PID , "") );
	}

	m_patchInfo.setPatch(m_patch.get());
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;
    m_boundingBoxSync = SyncStatus::UNSYNC;
	m_pointConnectivitySync = SyncStatus::NONE;

    setTolerance(1.0e-06);

}

/*!
 * Default destructor
 */
MimmoObject::~MimmoObject(){};

/*!
 * Copy constructor of MimmoObject.
 * Internal allocated PatchKernel is copied from the argument object as a soft link
 * (copied by its pointer only, geometry data structure is treated as external in the new copied class).
 * Search trees are instantiated, but their containts (if any) are not copied by default.
 * \param[in] other class to be copied
 */
MimmoObject::MimmoObject(const MimmoObject & other){

	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);

	m_isParallel = other.m_isParallel;

	m_extpatch = other.m_extpatch;
	if(other.m_internalPatch){
		m_extpatch      = other.m_patch.get();
	}
	m_internalPatch = false;

	m_type              = other.m_type;
	m_pidsType          = other.m_pidsType;
	m_pidsTypeWNames    = other.m_pidsTypeWNames;
	m_AdjSync          = other.m_AdjSync;
	m_IntSync          = other.m_IntSync;
    m_boundingBoxSync  = other.m_boundingBoxSync;

    m_patchInfo.setPatch(m_extpatch);
    m_patchInfo.update();
    m_infoSync = SyncStatus::SYNC;

	m_skdTreeSync   = SyncStatus::NONE;
	m_kdTreeSync    = SyncStatus::NONE;

	//instantiate empty trees:
	switch(m_type){
	case 1:
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_extpatch))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::VolumeSkdTree(dynamic_cast<bitpit::VolumeKernel*>(m_extpatch))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_extpatch))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

#if MIMMO_ENABLE_MPI
	m_rank = other.m_rank;
	m_nprocs = other.m_nprocs;
	MPI_Comm_dup(other.m_communicator, &m_communicator);
	m_nglobalvertices = other.m_nglobalvertices;
	m_pointGhostExchangeInfoSync = SyncStatus::NONE;
#else
	m_rank = 0;
	m_nprocs=1;
#endif

	m_pointConnectivitySync = SyncStatus::NONE;

	m_tolerance = other.m_tolerance;

};

/*!
 * Assignement operator of MimmoObject.
 * Please be careful when using it, because internal bitpit::PatchKernel structure
 * is only copied by their pointer (soft copy). If you destroy the original MimmoObject
 * the copied class will have an internal patch pointing to nullptr.
 * Search trees are instantiated, but their containts (if any) are not copied by default.
 * \param[in] other reference to another MimmoObject
 */
MimmoObject & MimmoObject::operator=(MimmoObject other){
	swap(other);
	return *this;
};

/*!
 * Swap function. Swap data between current object and one of the same type.
 * \param[in] x object to be swapped.
 */
void MimmoObject::swap(MimmoObject & x) noexcept
{
    std::swap(m_patch, x.m_patch);
	std::swap(m_extpatch, x.m_extpatch);
	std::swap(m_internalPatch, x.m_internalPatch);
	std::swap(m_type, x.m_type);
	std::swap(m_pidsType, x.m_pidsType);
	std::swap(m_pidsTypeWNames, x.m_pidsTypeWNames);
	std::swap(m_AdjSync, x.m_AdjSync);
	std::swap(m_IntSync, x.m_IntSync);
	std::swap(m_skdTree, x.m_skdTree);
	std::swap(m_kdTree, x.m_kdTree);
	std::swap(m_skdTreeSync, x.m_skdTreeSync);
	std::swap(m_kdTreeSync, x.m_kdTreeSync);
    std::swap(m_boundingBoxSync, x.m_boundingBoxSync);

    m_patchInfo.setPatch(getPatch());
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;

#if MIMMO_ENABLE_MPI
    std::swap(m_isParallel, x.m_isParallel);
    MPI_Comm dummy_communicator = MPI_COMM_NULL;
    MPI_Comm_dup(m_communicator, &dummy_communicator);
    MPI_Comm_dup(x.m_communicator, &m_communicator);
    MPI_Comm_dup(dummy_communicator, &x.m_communicator);
    MPI_Comm_free(&dummy_communicator);
	std::swap(m_rank, x.m_rank);
	std::swap(m_nprocs, x.m_nprocs);
	std::swap(m_nglobalvertices, x.m_nglobalvertices);
	m_pointGhostExchangeInfoSync = SyncStatus::NONE;
#endif
	m_pointConnectivitySync = SyncStatus::NONE; //point connectivity is not copied

	std::swap(m_tolerance, x.m_tolerance);

}

/*!
 * Initialize the logger.
 */
void
MimmoObject::initializeLogger(){

    bool logexists  = bitpit::log::manager().exists(MIMMO_LOG_FILE);

    if (!logexists){
#if MIMMO_ENABLE_MPI
        // MPI variables have to be already initialized
        bitpit::log::manager().initialize(bitpit::log::Mode::COMBINED, MIMMO_LOG_FILE, true, MIMMO_LOG_DIR, m_nprocs, m_rank);
#else
        bitpit::log::manager().initialize(bitpit::log::Mode::COMBINED, MIMMO_LOG_FILE, true, MIMMO_LOG_DIR);
#endif
    }
    m_log = &bitpit::log::cout(MIMMO_LOG_FILE);
    bitpit::log::setVisibility((*m_log), bitpit::log::Visibility::MASTER);
    bitpit::log::setConsoleVerbosity((*m_log), bitpit::log::NORMAL);
    bitpit::log::setFileVerbosity((*m_log), bitpit::log::NORMAL);

}

#if MIMMO_ENABLE_MPI
/*!
 * Initialize MPI if not already initialized.
 */
void
MimmoObject::initializeMPI(){

	int initialized;
	MPI_Initialized(&initialized);
	if (!initialized)
		MPI_Init(nullptr, nullptr);

}

/*!
 * Initialize communicator.
 * \param[in] isParallel if true/false MPI communicator is set to MPI_COMM_WORLD/MPI_COMM_NULL
 */
void
MimmoObject::initializeCommunicator(bool isParallel){

    // If patch not yet set initialize the communicator with MPI_COMM_WORLD
    // This will be used to initialize an internal patch
    // Otherwise use a duplicate of the communicator of the patch
    if (!getPatch()){

        if (isParallel){
            MPI_Comm_dup(MPI_COMM_WORLD, &m_communicator);
            MPI_Comm_rank(m_communicator, &m_rank);
            MPI_Comm_size(m_communicator, &m_nprocs);
        }else{
            m_communicator = MPI_COMM_NULL;
            m_rank = 0;
            m_nprocs = 1;
        }

    }else{

        // Recover communicator of the patch (always defined as parallel if I'm here)
        MPI_Comm_dup(getPatch()->getCommunicator(), &m_communicator);
        m_rank = getPatch()->getRank();
        m_nprocs = getPatch()->getProcessorCount();

    }

	m_pointGhostExchangeInfoSync = SyncStatus::NONE;

}
#endif

/*!
    \return reference to internal logger
 */
bitpit::Logger&
MimmoObject::getLog(){
	return (*m_log);
}

/*!
 * Is the object empty?
 * \return True/False if the local geometry data structure is empty.
 */
bool
MimmoObject::isEmpty(){

	bool check = getNVertices() == 0;
	check = check || (getNCells() == 0);

	return check;
};


/*!
 * Is the skdTree ordering supported w/ the current geometry?
 * \return true for connectivity-based meshes, false por point clouds
 */
bool
MimmoObject::isSkdTreeSupported(){
	return (m_skdTreeSync != SyncStatus::NOTSUPPORTED);
};

/*!
 * Return the type of mesh currently hold by the class
 * (for type of mesh allowed see MimmoObject(int type) documentation).
 * \return integer flag for mesh type
 */
int
MimmoObject::getType(){
	return m_type;
};

/*!
 * Return the total number of local vertices within the data structure.
   For parallel versions: ghost vertices are considered in the count.
 * \return number of mesh vertices
 */
long
MimmoObject::getNVertices(){
	const auto p = getPatch();
	return p->getVertexCount();
};

/*!
 * Return the total number of local cells within the data structure.
   For parallel versions: ghost cells are considered in the count.
 * \return number of mesh cells.
 */
long
MimmoObject::getNCells(){
	const auto p = getPatch();
	return p->getCellCount();
};

/*!
 * Return the total number of local vertices within the data structure.
   For parallel versions: ghost vertices are considered in the count.
 * \return number of mesh vertices
 */
long
MimmoObject::getNVertices() const {
    const auto p = getPatch();
    return p->getVertexCount();
};

/*!
 * Return the total number of local cells within the data structure.
   For parallel versions: ghost cells are considered in the count.
 * \return number of mesh cells.
 */
long
MimmoObject::getNCells() const {
    const auto p = getPatch();
    return p->getCellCount();
};

/*!
 * Return the total number of local internal cells (no ghosts) within the data structure
 * \return number of mesh cells.
 */
long
MimmoObject::getNInternals() const {
	const auto p = getPatch();
	return p->getInternalCellCount();
};

/*!
 * Return the number of interior vertices (no ghosts) within the data structure.
 * \return number of interior mesh vertices
 */
long
MimmoObject::getNInternalVertices(){
#if MIMMO_ENABLE_MPI
	if (!isDistributed())
#endif
		return getNVertices();
#if MIMMO_ENABLE_MPI
	return m_ninteriorvertices;
#endif
};

#if MIMMO_ENABLE_MPI
/*!
 * Return the total number of global vertices within the data structure
   throughout partitions.
 * \return number of mesh vertices
 */
long
MimmoObject::getNGlobalVertices(){
	if (!isParallel()){
		const auto p = getPatch();
		return p->getVertexCount();
	}

	return m_nglobalvertices;
};

/*!
 * Return the total number of global cells within the data structure,
   throughout partitions.
 * \return number of mesh cells.
 */
long
MimmoObject::getNGlobalCells() {
	if (!isParallel())
		return getNCells();

	return getPatchInfo()->getCellGlobalCount();
};

/*!
 * Return the partition offset for nodes consecutive index.
   The parallel structures have to be already updated (not internal sync).
 * \return points partition offset
 */
long
MimmoObject::getPointGlobalCountOffset(){
	if (!isParallel())
		return 0;

	return m_globaloffset;
};

#endif

/*!
 * Return the compact list of local vertices hold by the class.
   Ghost cells are considered to fill the structure.
 * \return coordinates of mesh vertices
 * \param[out] mapDataInv pointer to inverse of Map of vertex ids,
               for aligning external vertex data to bitpit::Patch ordering.
 */
dvecarr3E
MimmoObject::getVerticesCoords(lilimap* mapDataInv){
	dvecarr3E result(getNVertices());
	int  i = 0;

	auto pvert = getVertices();

	if (mapDataInv != nullptr){
		for (auto const & vertex : pvert){
			result[i] = vertex.getCoords();
			(*mapDataInv)[vertex.getId()] = i;
			++i;
		}
	}
	else{
		for (auto const & vertex : pvert){
			result[i] = vertex.getCoords();
			++i;
		}
	}
	return result;
};

/*!
 * Gets the vertex coordinates marked with a unique label i (bitpit::PatchKernel id);
 * If i is not found, return a default value (max double limit).
 * \param[in] i bitpit::PatchKernel id of the vertex in the mesh.
 * \return Coordinates of the i-th vertex of geometry mesh.
 */
const darray3E &
MimmoObject::getVertexCoords(long i) const
{
    assert(getVertices().exists(i));
    return 	getPatch()->getVertexCoords(i);
};

/*!
 * Return reference to the PiercedVector structure of local vertices hold
 * by bitpit::PatchKernel class member. Ghost cells vertices are considered.
 * \return  Pierced Vector structure of vertices
 */
bitpit::PiercedVector<bitpit::Vertex> &
MimmoObject::getVertices(){
	return getPatch()->getVertices();
}

/*!
 * Return const reference to the PiercedVector structure of local vertices hold
 * by bitpit::PatchKernel class member. Ghost cells vertices are considered.
 * \return  const Pierced Vector structure of vertices
 */
const bitpit::PiercedVector<bitpit::Vertex> &
MimmoObject::getVertices() const {
	const auto p = getPatch();
	return p->getVertices();
}

/*!
 * Get the local geometry compact connectivity.
 * "Compact" means that in the connectivity matrix the vertex indexing is referred to
 * the local, compact and sequential numbering of vertices, as it
 * gets them from the internal method getVertexCoords().
 * Special connectivity is returned for polygons and polyhedra support. See getCellConnectivity
 * doxy for further info.
 * Ghost cells are visited to fill the structure.
 * \return local connectivity list
 * \param[in] mapDataInv inverse of Map of vertex ids actually set, for aligning external vertex data to bitpit::Patch ordering
 */
livector2D
MimmoObject::getCompactConnectivity(lilimap & mapDataInv){

	livector2D connecti(getNCells());
	int np, counter =0;

	for(auto const & cell : getCells()){
		np = cell.getConnectSize();
		const long * conn_ = cell.getConnect();
		connecti[counter].resize(np);
		bitpit::ElementType eltype = cell.getType();

		if(eltype == bitpit::ElementType::POLYGON){
			connecti[counter][0] = conn_[0];
			for (int i=1; i<np; ++i){
				connecti[counter][i] = mapDataInv[conn_[i]];
			}
		}else if(eltype == bitpit::ElementType::POLYHEDRON){
			connecti[counter][0] = conn_[0];
			for(int nF = 0; nF < conn_[0]-1; ++nF){
				int facePos = cell.getFaceStreamPosition(nF);
				int beginVertexPos = facePos + 1;
				int endVertexPos   = facePos + 1 + conn_[facePos];
				connecti[counter][facePos] = conn_[facePos];
				for (int i=beginVertexPos; i<endVertexPos; ++i){
					connecti[counter][i] = mapDataInv[conn_[i]];
				}
			}
		}else{
			for (int i=0; i<np; ++i){
				connecti[counter][i] = mapDataInv[conn_[i]];
			}
		}
		++counter;
	}
	return connecti;
}

/*!
 * It gets the connectivity of the local cells of the linked geometry. Index of vertices in
 * connectivity matrix are returned according to bitpit::PatchKernel unique indexing (ids).
 * Special connectivity is returned for polygons and polyhedra support. See getCellConnectivity
 * doxy for further info.
 * Ghost cells are visited to fill the structure.
 * \return cell-vertices connectivity
 */
livector2D
MimmoObject::getConnectivity(){

	livector2D connecti(getNCells());
	int np, counter =0;

	for(auto const & cell : getCells()){
		np = cell.getConnectSize();
		const long * conn_ = cell.getConnect();
		connecti[counter].resize(np);
		for (int i=0; i<np; ++i){
			connecti[counter][i] = conn_[i];
		}
		++counter;
	}

	return connecti;
};

/*!
 * It gets the connectivity of a cell, with vertex id's in bitpit::PatchKernel unique indexing.
 * Connectivity of polygons is returned as (nV,V1,V2,V3,V4,...) where nV is the number of vertices
 * defining the polygon and V1,V2,V3... are indices of vertices.
 * Connectivity of polyhedrons  is returned as (nF,nF1V, V1, V2, V3,..., nF2V, V2,V4,V5,V6,...,....)
 * where nF is the total number of faces of polyhedros, nF1V is the number of vertices composing the face 1,
 * followed by the indices of vertices which draws it. Face 2,3,..,n are defined in the same way.
 * Any other standard cell element is uniquely defined by its list of vertex indices.
 * if i cell does not exists, return an empty list.
 * \param[in] i bitpit::PatchKernel ID of the cell.
 * \return i-th cell-vertices connectivity, in bitpit::PatchKernel vertex indexing.
 */
livector1D
MimmoObject::getCellConnectivity(long i) const {
	if (!(getCells().exists(i)))    return livector1D(0);

	const bitpit::Cell & cell = getPatch()->getCell(i);
	int np = cell.getConnectSize();
	const long * conn_ = cell.getConnect();
	livector1D connecti(np);

	for (int j=0; j<np; j++){
		connecti[j] = conn_[j];
	}
	return connecti;
};

/*!
 * Return reference to the PiercedVector structure of local cells hold
 * by bitpit::PatchKernel class member. Ghost cells are considered.
 * \return  Pierced Vector structure of cells
 */
bitpit::PiercedVector<bitpit::Cell> &
MimmoObject::getCells(){
	return getPatch()->getCells();
}

/*!
 * Return const reference to the PiercedVector structure of local cells hold
 * by bitpit::PatchKernel class member. Ghost cells are considered.
 * \return  const Pierced Vector structure of cells
 */
const bitpit::PiercedVector<bitpit::Cell> &
MimmoObject::getCells()  const{
	const auto p = getPatch();
	return p->getCells();
}

/*!
 * Return reference to the PiercedVector structure of local interfaces hold
 * by bitpit::PatchKernel class member. Interface between internal and ghost cells are considered.
 * \return  Pierced Vector structure of interfaces
 */
bitpit::PiercedVector<bitpit::Interface> &
MimmoObject::getInterfaces(){
	return getPatch()->getInterfaces();
}

/*!
 * Return const reference to the PiercedVector structure of local interfaces hold
 * by bitpit::PatchKernel class member. Interfaces between internal and ghost cells are considered.
 * \return  const Pierced Vector structure of interfaces
 */
const bitpit::PiercedVector<bitpit::Interface> &
MimmoObject::getInterfaces()  const{
	const auto p = getPatch();
	return p->getInterfaces();
}

/*!
 * Return a compact vector with the Ids of local cells given by
 * bitpit::PatchKernel unique-labeled indexing.
   \param[in] internalsOnly FOR MPI ONLY: if true method accounts for internal cells only,
   if false it includes also ghosts. Serial comp ignore this parameter.
 * \return ids of cells
 */
livector1D
MimmoObject::getCellsIds(bool internalsOnly){

    std::vector<long> ids;

#if MIMMO_ENABLE_MPI
    //MPI version
    if(internalsOnly){
        ids.reserve(getPatch()->getInternalCellCount());
        for(bitpit::PatchKernel::CellIterator it = getPatch()->internalCellBegin(); it!= getPatch()->internalCellEnd(); ++it){
            ids.push_back(it.getId());
        }
    }else{
        ids = getPatch()->getCells().getIds();
    }
#else
    //SERIAL version
    BITPIT_UNUSED(internalsOnly);
    ids = getPatch()->getCells().getIds();
#endif

    return ids;
};

/*!
 * Return a compact vector with the Ids of local vertices given by
 * bitpit::PatchKernel unique-labeled indexing.
   \param[in] internalsOnly if true method accounts for internal vertices only,
   if false it includes also ghosts.
 * \return ids of vertices
 */
livector1D
MimmoObject::getVerticesIds(bool internalsOnly){

    livector1D ids;

    if(internalsOnly){
        ids.reserve(getNInternalVertices());
    }
    else{
        ids.reserve(getNVertices());
    }

    for(const bitpit::Vertex & vertex : getVertices()){
        long vertexId = vertex.getId();
        if(!internalsOnly || isPointInterior(vertexId)){
            ids.push_back(vertexId);
        }
    }

    return ids;
};


/*!
 * \return pointer to bitpit::PatchKernel structure hold by the class.
 */
bitpit::PatchKernel*
MimmoObject::getPatch(){
	if(!m_internalPatch) return m_extpatch;
	else return m_patch.get();
};

/*!
 * \return pointer to bitpit::PatchKernel structure hold by the class.
 */
const bitpit::PatchKernel*
MimmoObject::getPatch() const{
	if(!m_internalPatch) return m_extpatch;
	else return m_patch.get();
};

/*!
 * Return the local indexing vertex map, to pass from local, compact indexing
 * to bitpit::PatchKernel unique-labeled indexing.
 * \param[in] withghosts If true the structure is filled with ghosts (meaningful only in parallel versions)
 * \return local/unique-id map
 */
lilimap
MimmoObject::getMapData(bool withghosts){
	lilimap mapData;
	lilimap mapDataInv = getMapDataInv(withghosts);
	for (auto const & vertex : getVertices()){
		long id = vertex.getId();
		if (mapDataInv.count(id))
			mapData[mapDataInv[id]] = id;
	}
	return mapData;
};

/*!
 * Return the local indexing vertex map, to pass from bitpit::PatchKernel unique-labeled indexing
 * to local, compact indexing. Note, ghost vertices are not considered in the consecutive map.
 * \param[in] withghosts If true the structure is filled with ghosts (meaningful only in parallel versions)
 * \return unique-id/local map
 */
lilimap
MimmoObject::getMapDataInv(bool withghosts){

    lilimap mapDataInv;
#if MIMMO_ENABLE_MPI
    if (isDistributed()){
        for (auto val : m_pointConsecutiveId){
            if (!withghosts && !isPointInterior(val.first)){
                continue;
            }
            mapDataInv.insert(val);
        }
        return mapDataInv;
    }
#else
	BITPIT_UNUSED(withghosts);
#endif
	int i = 0;
	for (auto const & vertex : getVertices()){
		mapDataInv[vertex.getId()] = i;
		++i;
	}
	return mapDataInv;
};

/*!
 * Return the map of consecutive global indexing of the local cells; to pass
   from consecutive global index of a local cell to bitpit::PatchKernel
   unique-labeled indexing. Ghost cells are considered.
 * \param[in] withghosts If true the structure is filled with ghosts (meaningful only in parallel versions)
 * \return global consecutive / local unique id map
 */
lilimap
MimmoObject::getMapCell(bool withghosts){
	lilimap mapCell;
	for (auto const & cell : getCells()){
		long id = cell.getId();
		if (!withghosts && !cell.isInterior()){
			continue;
		}
		long i = getPatchInfo()->getCellConsecutiveId(id);
		mapCell[i] = id;
	}
	return mapCell;
};

/*!
 * Return the map of consecutive global indexing of the local cells;
   to pass from bitpit::PatchKernel unique-labeled indexing of a local cell
 * to consecutive global index. Ghost cells are considered.
 * \param[in] withghosts If true the structure is filled with ghosts (meaningful only in parallel versions)
 * \return local unique id / global consecutive map
 */
lilimap
MimmoObject::getMapCellInv(bool withghosts){
	lilimap mapCellInv = getPatchInfo()->getCellConsecutiveMap();
	if (!withghosts){
		std::vector<long> todelete;
		for (auto & val : mapCellInv){
			if (!getPatch()->getCell(val.second).isInterior()){
				todelete.push_back(val.first);
			}
		}
		for (int id : todelete){
			mapCellInv.erase(id);
		}
	}
	return mapCellInv;
};

/*!
 * \return the list of PID types actually present in your geometry.
 * If empty list is returned, pidding is actually not supported for this geometry
 */
std::unordered_set<long> 	&
MimmoObject::getPIDTypeList(){
	return m_pidsType;
};

/*!
 * \return the list of PID types actually present in your geometry with its custom names attached.
 * If empty list is returned, pidding is actually not supported for this geometry
 */
std::unordered_map<long, std::string> &
MimmoObject::getPIDTypeListWNames(){
	return m_pidsTypeWNames;
};

/*!
 * \return the list of PID types actually present in your geometry.
 * If empty list is returned, pidding is actually not supported for this geometry
 */
const std::unordered_set<long>    &
MimmoObject::getPIDTypeList() const {
    return m_pidsType;
};

/*!
 * \return the list of PID types actually present in your geometry with its custom names attached.
 * If empty list is returned, pidding is actually not supported for this geometry
 */
const std::unordered_map<long, std::string> &
MimmoObject::getPIDTypeListWNames() const {
    return m_pidsTypeWNames;
};

/*!
 * \return the list of PID associated to each cell of tessellation in compact
 * sequential ordering. Ghost cells are considered.
 * If empty list is returned, pidding is not supported for this geometry.
 */
livector1D
MimmoObject::getCompactPID() {
	if(m_pidsType.empty())	return livector1D(0);
	livector1D result(getNCells());
	int counter=0;
	for(auto const & cell : getCells()){
		result[counter] = (long)cell.getPID();
		++counter;
	}
	return(result);
};

/*!
 * \return the map of the PID (argument) associated to each cell local unique-id (key) in tessellation.
 * If empty map is returned, pidding is not supported for this geometry.
 * Ghost cells are considered.
 */
std::unordered_map<long,long>
MimmoObject::getPID() {
	if(m_pidsType.empty())	return std::unordered_map<long,long>();
	std::unordered_map<long,long> 	result;
	for(auto const & cell : getCells()){
		result[cell.getId()] = (long) cell.getPID();
	}
	return(result);
};

/*!
 * Return the synchronization status of the skdTree ordering structure for cells
 * \return synchronization status of the skdTree with your current geometry.
 */
SyncStatus
MimmoObject::getSkdTreeSyncStatus(){
	return m_skdTreeSync;
}

/*!
 * Return the synchronization status of the patch numbering info structure for cells
 * \return synchronization status of the patch numbering info with your current geometry.
 */
SyncStatus
MimmoObject::getInfoSyncStatus(){
	return m_infoSync;
}

/*!
 * Return the synchronization status of the patch bounding box
 * \return synchronization status of the patch bounding box with your current geometry.
 */
SyncStatus
MimmoObject::getBoundingBoxSyncStatus(){
    return m_boundingBoxSync;
}

/*!
 * \return pointer to geometry skdTree search structure
 */
bitpit::PatchSkdTree*
MimmoObject::getSkdTree(){
	return m_skdTree.get();
}

/*!
 * \return pointer to geometry patch numbering info structure
 */
bitpit::PatchNumberingInfo*
MimmoObject::getPatchInfo(){
	return &m_patchInfo;
}

/*!
 * Return the synchronization status of the kdTree ordering structure for cells
 * \return synchronization status of the kdTree with your current geometry.
 */
SyncStatus
MimmoObject::getKdTreeSyncStatus(){
	return m_kdTreeSync;
}

/*!
 * \return pointer to geometry KdTree internal structure
 */
bitpit::KdTree<3, bitpit::Vertex, long >*
MimmoObject::getKdTree(){
	return m_kdTree.get();
}

/*!
 * Get if a vertex is local or not. The structures have to be previously updated (not internal sync).
 * \param[in] id Vertex id
 * Note: if not mpi support enabled or patch not partitioned return true (no check if node exists).
 */
bool
MimmoObject::isPointInterior(long id)
{
#if MIMMO_ENABLE_MPI
	return getPatch()->getVertex(id).isInterior();
#else
	return true;
#endif
}

/*!
 * Get the geometric tolerance used to perform geometric operations on the geometry.
 * param[out] Geometric tolerance
 */
double
MimmoObject::getTolerance(){
	return m_tolerance;
}

/*!
	Gets the MPI rank associated to the object.
    For serial version return always 0.
	\return The MPI rank associated to the object.
 */
int MimmoObject::getRank() const
{
	return m_rank;
}

/*!
	Gets the MPI processors in the communicator associated to the object.
    For serial version return always 1
	\return The MPI processors in the communicator associated to the object
 */
int MimmoObject::getProcessorCount() const
{
	return m_nprocs;
}


#if MIMMO_ENABLE_MPI

/*!
	Gets the MPI communicator associated to the object

	\return The MPI communicator associated to the object.
 */
const MPI_Comm & MimmoObject::getCommunicator() const
{
	return m_communicator;
}

/*!
	Gets a constant reference to the ghost targets needed for point data exchange.

	\return a constant reference to the ghost targets needed for point data
	exchange.
 */
const std::unordered_map<int, std::vector<long>> & MimmoObject::getPointGhostExchangeTargets() const
{
    return getPatch()->getGhostVertexExchangeTargets();
}

/*!
	Gets a constant reference to the ghost sources needed for point data exchange.

	\return A constant reference to the ghost sources needed for point data
	exchange.
 */
const std::unordered_map<int, std::vector<long>> & MimmoObject::getPointGhostExchangeSources() const
{
    return getPatch()->getGhostVertexExchangeSources();
}


/*!
	Get the ghost exchange information synchronization status with the geometry.
	\return  ghost exchange information synchronization status.
 */
SyncStatus
MimmoObject::getPointGhostExchangeInfoSyncStatus()
{
	MPI_Barrier(m_communicator);
	MPI_Allreduce(MPI_IN_PLACE, &m_pointGhostExchangeInfoSync, 1, MPI_LONG, MPI_MIN, m_communicator);
	return m_pointGhostExchangeInfoSync;
}

/*!
	Update the information needed for ghost point data exchange as well as
	the shared points.
 */
void MimmoObject::updatePointGhostExchangeInfo()
{

    //Update n interior vertices
    m_ninteriorvertices = 0;
    for (long vertexId : getVerticesIds()){
        if (isPointInterior(vertexId)){
            m_ninteriorvertices++;
        }
    }

    //Update n global vertices
    m_nglobalvertices = 0;
    MPI_Allreduce(&m_ninteriorvertices, &m_nglobalvertices, 1, MPI_LONG, MPI_SUM, m_communicator);

    //Update n interior vertices for procs
    m_rankinteriorvertices.clear();
    m_rankinteriorvertices.resize(m_nprocs);
    m_rankinteriorvertices[m_rank] = m_ninteriorvertices;
    long rankinteriorvertices = m_rankinteriorvertices[m_rank];
    MPI_Allgather(&rankinteriorvertices, 1, MPI_LONG, m_rankinteriorvertices.data(), 1, MPI_LONG, m_communicator);

    //Update global offset
    m_globaloffset = 0;
    for (int i=0; i<m_rank; i++){
        m_globaloffset += m_rankinteriorvertices[i];
    }

    //---
    //Create consecutive map for vertices and fill locals
    //---
    m_pointConsecutiveId.clear();
    long consecutiveId = m_globaloffset;
    //Insert owned vertices
    for (const long & id : getVerticesIds()){
        if (isPointInterior(id)){
            m_pointConsecutiveId[id] = consecutiveId;
            consecutiveId++;
        }
    }


    //---
    //Communicate consecutive ids for ghost points
    //---
    {
        size_t exchangeDataSize = sizeof(consecutiveId);
        std::unique_ptr<bitpit::DataCommunicator> dataCommunicator;
        dataCommunicator = std::unique_ptr<bitpit::DataCommunicator>(new bitpit::DataCommunicator(getCommunicator()));

        // Set and start the sends
        for (const auto entry : getPointGhostExchangeSources()) {
            const int rank = entry.first;
            auto &list = entry.second;
            dataCommunicator->setSend(rank, list.size() * exchangeDataSize);
            bitpit::SendBuffer &buffer = dataCommunicator->getSendBuffer(rank);
            for (long id : list) {
                if (m_pointConsecutiveId.count(id)){
                    buffer << m_pointConsecutiveId.at(id);
                }else{
                    buffer << long(-1);
                }
            }
            dataCommunicator->startSend(rank);
        }

        // Discover & start all the receives
        dataCommunicator->discoverRecvs();
        dataCommunicator->startAllRecvs();

        // Receive the consecutive ids of the ghosts
        int nCompletedRecvs = 0;
        while (nCompletedRecvs < dataCommunicator->getRecvCount()) {
            int rank = dataCommunicator->waitAnyRecv();
            const auto &list = getPointGhostExchangeTargets().at(rank);
            bitpit::RecvBuffer &buffer = dataCommunicator->getRecvBuffer(rank);
            for (long id : list) {
                buffer >> consecutiveId;
                if (consecutiveId > -1){
                    m_pointConsecutiveId[id] = consecutiveId;
                }
            }
            ++nCompletedRecvs;
        }
        // Wait for the sends to finish
        dataCommunicator->waitAllSends();
    }

	// Exchange info are now updated
	m_pointGhostExchangeInfoSync = SyncStatus::SYNC;
}

/*!
	Reset any information for ghost point data exchange.
	Shared points included.
 */
void MimmoObject::resetPointGhostExchangeInfo()
{
	//Update n interior vertices
	m_ninteriorvertices = 0;

	//Update n global vertices
	m_nglobalvertices = 0;

	//Update n interior vertices for procs
	m_rankinteriorvertices.clear();

	//Update global offset
	m_globaloffset = 0;

	//Reset consecutive map for vertices
	m_pointConsecutiveId.clear();

	if(m_pointGhostExchangeInfoSync == SyncStatus::SYNC){
        m_pointGhostExchangeInfoSync = SyncStatus::UNSYNC;
    }
}

/*!
    General checker for SkdTree status throughout all procs. If at least one partition
    has not-syncronized SkdTree set all the other procs to not syncronized and free the structure eventually.
    \return synchronization status of skd tree.
 */
SyncStatus
MimmoObject::cleanParallelSkdTreeSync(){

	MPI_Allreduce(MPI_IN_PLACE, &m_skdTreeSync, 1, MPI_LONG, MPI_MIN, m_communicator);

	if(m_skdTreeSync != SyncStatus::SYNC){
		cleanSkdTree();
	}

	return m_skdTreeSync;
}

/*!
    General checker for KdTree status throughout all procs. If at least one partition
    has not-syncronized KdTree set all the other procs to not syncronized and free the structure eventually.
    \return synchronization status of kdtree
 */
SyncStatus
MimmoObject::cleanParallelKdTreeSync(){

	MPI_Allreduce(MPI_IN_PLACE, &m_kdTreeSync, 1, MPI_LONG, MPI_MIN, m_communicator);

	if(m_kdTreeSync != SyncStatus::SYNC){
		cleanKdTree();
	}

	return (m_kdTreeSync);
}
/*!
    General checker for Adjacencies status throughout all procs. If at least one partition
    has not-syncronized Adj set all the other procs to not syncronized.
    If one partition has the adjacencies not build free the structure.
    \return Synchronization status of adjacencies
 */
SyncStatus
MimmoObject::cleanParallelAdjacenciesSync(){

    // Reduce by using the minimum sync status
	MPI_Allreduce(MPI_IN_PLACE, &m_AdjSync, 1, MPI_LONG, MPI_MIN, m_communicator);

	//if status is none destroy adjacencies
	if(m_AdjSync == SyncStatus::NONE){
		destroyAdjacencies();
	}

	return (m_AdjSync);
}

/*!
    General checker for Interfaces status throughout all procs. If at least one partition
    has not-syncronized Interfaces set all the other procs to not syncronized.
    If one partition has the Interfaces not build free the structure.
    \return synchronization status of interfaces.
 */
SyncStatus
MimmoObject::cleanParallelInterfacesSync(){

	// Reduce by using the minimum sync status
	MPI_Allreduce(MPI_IN_PLACE, &m_IntSync, 1, MPI_LONG, MPI_MIN, m_communicator);

    //if status is none destroy interfaces
	if(m_IntSync == SyncStatus::NONE){
		destroyInterfaces();
	}

	return (m_IntSync);
}

/*!
    General checker for PointConnectivity status throughout all procs. If at least one partition
    has not-syncronized PointConnectivity set all the other procs to not syncronized.
    If one partition has the PointConnectivity not build free the structure.
    \return Synchronization status of point connectivity
 */
SyncStatus
MimmoObject::cleanParallelPointConnectivitySync(){

    // Reduce by using the minimum sync status
	MPI_Allreduce(MPI_IN_PLACE, &m_pointConnectivitySync, 1, MPI_LONG, MPI_MIN, m_communicator);

    //if status is not sync destroy
	if(getPointConnectivitySyncStatus() != SyncStatus::SYNC){
		cleanPointConnectivity();
	}

	return (m_pointConnectivitySync);
}
/*!
    General checker for patch numbering info status throughout all procs. If at least one partition
    has not-syncronized patch numbering info set all the other procs to not syncronized.
    If one partition has the patch numbering info not build free the structure.
    \return Synchronization status of patch info.
 */
SyncStatus
MimmoObject::cleanParallelInfoSync(){

    // Reduce by using the minimum sync status
	MPI_Allreduce(MPI_IN_PLACE, &m_infoSync, 1, MPI_LONG, MPI_MIN, m_communicator);

    //if status is not sync destroy
	if(m_infoSync != SyncStatus::SYNC){
		m_patchInfo.reset();
	}

	return (m_infoSync);
}
/*!
    General checker for patch bounding box status throughout all procs. If at least one partition
    has not-syncronized patch bounding box set all the other procs to not syncronized.
    If one partition has the patch numbering info not build free the structure.
    \return Synchronization status of patch bounding box.
 */
SyncStatus
MimmoObject::cleanParallelBoundingBoxSync(){

    // Reduce by using the minimum sync status
    MPI_Allreduce(MPI_IN_PLACE, &m_boundingBoxSync, 1, MPI_LONG, MPI_MIN, m_communicator);

    //if status is not sync destroy
    if(m_boundingBoxSync != SyncStatus::SYNC){
//        getPatch()->clearBoundingBox();
    }

    return (m_boundingBoxSync);
}

/*!
    General checker for point ghost exchange info status throughout all procs. If at least one partition
    has not-syncronized point ghost exchange info set all the other procs to not syncronized.
    If one partition has the point ghost exchange info not build free the structure.
    \return Synchronization status of point exchange info.
 */
SyncStatus
MimmoObject::cleanParallelPointGhostExchangeInfoSync(){

    // Reduce by using the minimum sync status
	MPI_Allreduce(MPI_IN_PLACE, &m_pointGhostExchangeInfoSync, 1, MPI_LONG, MPI_MIN, m_communicator);

    //if status is not sync destroy
	if(m_pointGhostExchangeInfoSync != SyncStatus::SYNC){
		resetPointGhostExchangeInfo();
	}

	return (m_pointGhostExchangeInfoSync);
}

/*!
    General checker for all InfoSync status throughout all procs. If at least one partition
    has one not-syncronized InfoSync set all the other procs to not syncronized for this InfoSync
    and free the structure eventually.
*/
void
MimmoObject::cleanAllParallelSync(){
    cleanParallelSkdTreeSync();
    cleanParallelKdTreeSync();
    cleanParallelAdjacenciesSync();
    cleanParallelInterfacesSync();
    cleanParallelPointConnectivitySync();
    cleanParallelInfoSync();
    cleanParallelBoundingBoxSync();
    cleanParallelPointGhostExchangeInfoSync();
}

/*!
 * Give if the patch is really partitioned, i.e. not owned entirely by the single master process.
 * Note. In mimmo the master process is considered rank = 0.
 */
bool
MimmoObject::isDistributed()
{
    if (getPatch() == nullptr){
        return false;
    }

    return getPatch()->isDistributed();
}

/*!
    Track all ghost cells present in the current object which are not
    adjacent to at least one internal cell and erase them.
    WARNING Orphan vertices may be created after this method call.

 */
void MimmoObject::deleteOrphanGhostCells(){

	//build temporarely adjacencies to get orphans.
	bool checkResetAdjacencies = false;
	if(m_AdjSync != SyncStatus::SYNC){
		updateAdjacencies();
		checkResetAdjacencies = true;
	}
	std::vector<long> markToDelete;

	//Track orphan ghosts, that is ghost cells which have no neighbour cell marked as internal.
	bitpit::PiercedVector<bitpit::Cell> & patchCells = getCells();
	for(auto it = getPatch()->ghostCellBegin(); it != getPatch()->ghostCellEnd(); ++it){
		std::vector<long> neighs = getPatch()->findCellNeighs(it.getId());
		bool check = false;
		std::vector<long>::iterator itN = neighs.begin();
		while(itN != neighs.end() && !check){
			check = check || patchCells[*itN].isInterior();
			++itN;
		}

		//if check is still false, send this ghost cell to deletion
		if(!check){
			markToDelete.push_back(it.getId());
		}
	}

	//clean marked ghosts;
	if(!markToDelete.empty()){
		getPatch()->deleteCells(markToDelete);
	}
	//erase temporarely adjacencies
	if(checkResetAdjacencies){
		destroyAdjacencies();
	}

}

#endif

/*!
 * Give if the patch is a parallel patch, i.e. it has a communicator set and it can
 * be distributed over the processes (see isDistributed() method).
 */
bool
MimmoObject::isParallel()
{
    if (getPatch() == nullptr){
        return false;
    }

    return m_isParallel;
}

/*!
 * Set geometric tolerance used to perform geometric operations on the patch.
 * It set directly even the tolerance of the patch if not null.
 * param[in] tol input geometric tolerance
 */
void
MimmoObject::setTolerance(double tol)
{
	m_tolerance = std::max(1.0e-15, tol);
    getPatch()->setTol(m_tolerance);
}


/*!
 *It adds one vertex to the mesh.
 * If unique-id is specified for the vertex, assign it, otherwise provide itself
 * to get a unique-id for the added vertex. The latter option is the default.
 * If the unique-id is already assigned, return with unsuccessful insertion.
 * Vertices of ghost cells have to be considered.
 *
 * \param[in] vertex vertex coordinates to be added
 * \param[in/out] idtag  unique id associated to the vertex
 * \return id of the inserted vertex. return bitpit::Vertex::NULL_ID if failed.
 */
long
MimmoObject::addVertex(const darray3E & vertex, const long idtag){

	if(idtag != bitpit::Vertex::NULL_ID && getVertices().exists(idtag))    return bitpit::Vertex::NULL_ID;

	bitpit::PatchKernel::VertexIterator it;
	auto patch = getPatch();
	if(idtag == bitpit::Vertex::NULL_ID){
		it = patch->addVertex(vertex);
	}else{
		it = patch->addVertex(vertex, idtag);
	}

	long id = it->getId();

	m_kdTreeSync = std::min(m_kdTreeSync, SyncStatus::UNSYNC);
    m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
    m_boundingBoxSync = std::min(m_boundingBoxSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
	m_pointConnectivitySync = std::min(m_pointConnectivitySync, SyncStatus::UNSYNC);
	return id;
};

/*!
 *It adds one vertex to the mesh.
 * If unique-id is specified for the vertex, assign it, otherwise provide itself
 * to get a unique-id for the added vertex. The latter option is the default.
 * If the unique-id is already assigned, return with unsuccessful insertion.
 *
 *
 * \param[in] vertex vertex to be added
 * \param[in] idtag  unique id associated to the vertex
 * \return id of the inserted vertex. return bitpit::Vertex::NULL_ID if failed.
 */
long
MimmoObject::addVertex(const bitpit::Vertex & vertex, const long idtag){

	if(idtag != bitpit::Vertex::NULL_ID && getVertices().exists(idtag))    return bitpit::Vertex::NULL_ID;

	bitpit::PatchKernel::VertexIterator it;
	auto patch = getPatch();
	if(idtag == bitpit::Vertex::NULL_ID){
		it = patch->addVertex(vertex);
	}else{
		it = patch->addVertex(vertex, idtag);
	}

	long id = it->getId();

	m_kdTreeSync = std::min(m_kdTreeSync, SyncStatus::UNSYNC);
	m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
    m_boundingBoxSync = std::min(m_boundingBoxSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
	m_pointConnectivitySync = std::min(m_pointConnectivitySync, SyncStatus::UNSYNC);
	return id;
};


/*!
 * It modifies the coordinates of a pre-existent vertex.
 * \param[in] vertex new vertex coordinates
 * \param[in] id unique-id of the vertex meant to be modified
 * \return false if no geometry is present or vertex id does not exist.
 */
bool
MimmoObject::modifyVertex(const darray3E & vertex, const long & id){

	if(!(getVertices().exists(id))) return false;
	bitpit::Vertex &vert = getPatch()->getVertex(id);
	vert.setCoords(vertex);
	m_skdTreeSync = std::min(m_skdTreeSync, SyncStatus::UNSYNC);
	m_kdTreeSync = std::min(m_kdTreeSync, SyncStatus::UNSYNC);
	m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
    m_boundingBoxSync = std::min(m_boundingBoxSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
    m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
	return true;
};

/*!
 * See method addConnectedCell(const livector1D & conn, bitpit::ElementType type, long PID, long idtag, int rank) doxy.
 * The only difference is the automatic assignment to PID= 0 for the current element and the automatic assignment of ID.
 * \return id of the inserted cell. return bitpit::Cell::NULL_ID if failed.
 */
long
MimmoObject::addConnectedCell(const livector1D & conn, bitpit::ElementType type, int rank){
	return addConnectedCell(conn, type, 0, bitpit::Cell::NULL_ID, rank);
};

/*!
 * See method addConnectedCell(const livector1D & conn, bitpit::ElementType type, long PID, long idtag, int rank) doxy.
 * The only difference is the automatic assignment to PID= 0 for the current element.
 * \return id of the inserted cell. return bitpit::Cell::NULL_ID if failed.
 */
long
MimmoObject::addConnectedCell(const livector1D & conn, bitpit::ElementType type, long idtag, int rank){
	return addConnectedCell(conn, type, 0, idtag, rank);
};

/*!
 * It adds one cell with its vertex-connectivity (vertex in bitpit::PatchKernel unique id's), the type of cell to
 * be added and its own unique id. If no id is specified, teh method assigns it automatically.
 * Cell type and connectivity dimension of the cell are cross-checked with the mesh type of the class: if mismatch, the method
 * does not add the cell and return false.
 * If class type is a pointcloud one (type 3) the method creates cell of type bitpit::Vertex
 * A part identifier PID mark can be associated to the cell.
 *
 * As a reminder for connectivity conn argument:
 *  - Connectivity of polygons must be defined as (nV,V1,V2,V3,V4,...) where nV is the number of vertices
 * defining the polygon and V1,V2,V3... are indices of vertices.
 *  - Connectivity of polyhedrons must be defined as (nF,nF1V, V1, V2, V3,..., nF2V, V2,V4,V5,V6,...,....)
 * where nF is the total number of faces of polyhedros, nF1V is the number of vertices composing the face 1,
 * followed by the indices of vertices which draws it. Face 2,3,..,n are defined in the same way.
 *  - Any other standard cell element is uniquely defined by its list of vertex indices.
 *
 * FOR MPI version: the method add cell with the custom rank provided. If rank < 0, use local m_rank of current MimmoObject.
 *
 * \param[in] conn  connectivity of target cell of geometry mesh.
 * \param[in] type  type of element to be added, according to bitpit ElementInfo enum.
 * \param[in] PID   part identifier
 * \param[in] idtag id of the cell
 * \param[in] rank  belonging proc rank of the Cell (for MPI version only)
 * \return id of the inserted cell. return bitpit::Cell::NULL_ID if failed.
 */
long
MimmoObject::addConnectedCell(const livector1D & conn, bitpit::ElementType type, long PID, long idtag, int rank){

#if MIMMO_ENABLE_MPI==0
	BITPIT_UNUSED(rank);
#endif

	if (conn.empty()) return bitpit::Cell::NULL_ID;
	if(idtag != bitpit::Cell::NULL_ID && getCells().exists(idtag)) return bitpit::Cell::NULL_ID;

	if(!checkCellConnCoherence(type, conn))  return bitpit::Cell::NULL_ID;

	auto patch = getPatch();

	bitpit::PatchKernel::CellIterator it;
	long checkedID;
#if MIMMO_ENABLE_MPI
	if(rank < 0)    rank = m_rank;
	it = patch->addCell(type, conn, rank, idtag);
#else
	it = patch->addCell(type, conn, idtag);
#endif
	checkedID = it->getId();

	setPIDCell(checkedID, PID);

	m_skdTreeSync = std::min(m_skdTreeSync, SyncStatus::UNSYNC);
	m_AdjSync = std::min(m_AdjSync, SyncStatus::UNSYNC);
	m_IntSync = std::min(m_IntSync, SyncStatus::UNSYNC);
	m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
	m_pointConnectivitySync = std::min(m_pointConnectivitySync, SyncStatus::UNSYNC);
	return checkedID;
};

/*!
 * Same as MimmoObject::addCell(bitpit::Cell & cell, const long idtag, int rank), but
 * using id automatic assignment.
 * \return id of the inserted cell. return bitpit::Cell::NULL_ID if failed.
 */
long
MimmoObject::addCell(bitpit::Cell & cell, int rank){
	return addCell(cell, bitpit::Cell::NULL_ID, rank);
}

/*!
 *It adds one cell to the mesh.
 * If unique-id is specified for the cell, assign it, otherwise provide itself
 * to get a unique-id for the added cell. The latter option is the default.
 * If the unique-id is already assigned, return with unsuccessful insertion.
 * PAY ATTENTION if the cell have adjacencies and interfaces built, they are not copied
 * to not mess with local PatchKernel and MimmoObject Adjacencies and Interfaces syncronization.
 *
 * FOR MPI version: the method add cell with the custom rank provided. If rank < 0, use local m_rank of MimmoObject.
 *
 * \param[in] cell cell to be added
 * \param[in] idtag  unique id associated to the cell
 * \param[in] rank  belonging proc rank of the Cell (for MPI version only)
 * \return id of the inserted cell. return bitpit::Cell::NULL_ID if failed.
 */
long
MimmoObject::addCell(bitpit::Cell & cell, const long idtag, int rank){
#if MIMMO_ENABLE_MPI==0
	BITPIT_UNUSED(rank);
#endif

	if(idtag != bitpit::Cell::NULL_ID && getCells().exists(idtag))    return bitpit::Cell::NULL_ID;
	auto patch = getPatch();

	//patchkernel methods addCell(const & Cell... ) copy everything from the target cell, adjacencies and interfaces included.
	//and move them into the new patch.
	// But pay attention  these information are useless for you, since you considered interfaces and
	// adjacencies invalidated every time you add a cell.
	//So practically keep using an add cell method passing connectivity, type, PID and rank.

	int connectSize = cell.getConnectSize();
	long *conn = cell.getConnect();
	std::unique_ptr<long[]> connectStorage = std::unique_ptr<long[]>(new long[connectSize]);
	std::copy(conn, conn+connectSize, connectStorage.get());
	bitpit::PatchKernel::CellIterator itcell;
#if MIMMO_ENABLE_MPI==1
	if(rank < 0)    rank = m_rank;
	itcell = patch->addCell(cell.getType(), std::move(connectStorage), rank, idtag);
#else
	itcell = patch->addCell(cell.getType(), std::move(connectStorage), idtag);
#endif

	long checkedID = itcell->getId();

	setPIDCell(checkedID, cell.getPID());

    m_skdTreeSync = std::min(m_skdTreeSync, SyncStatus::UNSYNC);
    m_AdjSync = std::min(m_AdjSync, SyncStatus::UNSYNC);
    m_IntSync = std::min(m_IntSync, SyncStatus::UNSYNC);
    m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
    m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
    m_pointConnectivitySync = std::min(m_pointConnectivitySync, SyncStatus::UNSYNC);
	return checkedID;
};

/*!
 * Set PIDs for all geometry cells available.
 * Any previous PID subdivion with label name associated will be erased.
 * The PID list must be referred to the compact local/indexing of the cells in the class.
 * \param[in] pids PID list.
 */
void
MimmoObject::setPID(livector1D pids){
	if((int)pids.size() != getNCells()) return;

	m_pidsType.clear();
	m_pidsTypeWNames.clear();
	int counter = 0;
	for(auto & cell: getCells()){
		m_pidsType.insert(pids[counter]);
		cell.setPID(pids[counter]);
		++counter;
	}

	for(const auto & pid : m_pidsType){
		m_pidsTypeWNames.insert(std::make_pair( pid, ""));
	}
};

/*!
 * Set PIDs for all geometry cells available.
 * Any previous PID subdivion with label name associated will be erased.
 * The PID must be provided as a map with cell unique-id as key and pid associated to it as argument.
 * \param[in] pidsMap PID amp.
 */
void
MimmoObject::setPID(std::unordered_map<long, long> pidsMap){
	if(getNCells() == 0) return;

	m_pidsType.clear();
	m_pidsTypeWNames.clear();
	auto & cells = getCells();
	for(auto const & val: pidsMap){
		if(cells.exists(val.first)){
			cells[val.first].setPID(val.second);
			m_pidsType.insert(val.second);
		}
	}

	for(const auto & pid : m_pidsType){
		m_pidsTypeWNames.insert(std::make_pair( pid, ""));
	}

};

/*!
 * Set the PID name, if PID exists
 * \param[in] id unique-id of the cell
 * \param[in] pid PID to assign on cell
 */
void
MimmoObject::setPIDCell(long id, long pid){
	auto & cells = getCells();
	if(cells.exists(id)){
		cells[id].setPID((int)pid);
		m_pidsType.insert(pid);
		m_pidsTypeWNames.insert(std::make_pair( pid, "") );
	}
};

/*!
 * Set the PID of a target cell
 * \param[in] pid PID to assign on cell
 * \param[in] name name to be assigned to the pid
 *
 * \return true if name is assigned, false if not.
 */
bool
MimmoObject::setPIDName(long pid, const std::string & name){
	if(! bool(m_pidsTypeWNames.count(pid))) return false;
	m_pidsTypeWNames[pid] = name;
	return true;
}

/*!
 * Update class member map m_pidsType with PIDs effectively contained.
 * Beware any label name previously stored for pid will be lost.
 * in the internal/linked geometry patch.
 */
void
MimmoObject::resyncPID(){
	m_pidsType.clear();
	std::unordered_map<long, std::string> copynames = m_pidsTypeWNames;
	m_pidsTypeWNames.clear();
	for(const bitpit::Cell & cell : getCells()){
		m_pidsType.insert( (long)cell.getPID() );
	}
	std::string work;
	for(const auto & pid : m_pidsType){
		if (copynames.count(pid) > 0){
			work =  copynames[pid];
		}else{
			work = "";
		}
		m_pidsTypeWNames.insert(std::make_pair( pid, work));
	}
};

/*!
 * Clone your MimmoObject in a new indipendent MimmoObject.
   All data of the current class will be "hard" copied in a new MimmoObject class.
   Hard means that the geometry data structure will be allocated internally into
   the new object as an exact and stand-alone copy of the geometry data structure
   of the current class, indipendently on how the current class owns or links it.
 * \return MimmoSharedPointer to cloned MimmoObject.
 */
MimmoSharedPointer<MimmoObject> MimmoObject::clone() const {

	//first step clone the internal bitpit patch.
	std::unique_ptr<bitpit::PatchKernel> clonedPatch = getPatch()->clone();

	//build the cloned mimmoObject using the custom constructor
	MimmoSharedPointer<MimmoObject> result (new MimmoObject(m_type, clonedPatch));

	//--> this constructor checks adjacencies and interfaces, initialize parallel
	// and update the infoSync (cell parallel structures also)
	//now what missing here?
	// 1) MimmoObject sync'ed PID structured and get eventually local PID names.
	result->resyncPID();
	for(auto & touple: m_pidsTypeWNames){
		result->setPIDName(touple.first, touple.second);
	}

//	result->update();

	// 2) check if trees are built here locally and force build to result eventually;
	if(m_kdTreeSync == SyncStatus::SYNC)    result->buildKdTree();
	if(m_skdTreeSync == SyncStatus::SYNC)   result->buildSkdTree();

//
#if MIMMO_ENABLE_MPI
	//rank, nprocs and communicator managed inside initializeParallel call  of the MimmoObject constructor
	//3) check if PointGhost are synchronized, if they are, update point ghost of result
	if(m_pointGhostExchangeInfoSync == SyncStatus::SYNC)    result->updatePointGhostExchangeInfo();
#endif

	//4) check if pointConnectivity is built here locally, if it is, force build to result.
	if(m_pointConnectivitySync == SyncStatus::SYNC)    result->buildPointConnectivity();

	return result;
};

/*!
 * It finds and cleans duplicated vertices and, in case of connected tessellations,
   all orphan/isolated vertices.
   In case of active cleaning, unsync all MimmoObject structures.
   In MPI version this call is collective among all ranks.
 * \return false if no activity cleaning is performed, true otherwise.
 */
bool
MimmoObject::cleanGeometry(){
	auto patch = getPatch();
    int nvold = patch->getVertexCount();
	patch->deleteCoincidentVertices();
    bool checkSync = (nvold == patch->getVertexCount());

#if MIMMO_ENABLE_MPI
    // Delete orphan ghosts cells
    deleteOrphanGhostCells();
    bool checkCleanOrphan = (getPatch()->countOrphanVertices() == 0);
    if(!checkCleanOrphan){
        getPatch()->deleteOrphanVertices();
    }
    checkSync = checkSync && checkCleanOrphan;
    MPI_Allreduce(MPI_IN_PLACE, &checkSync,1, MPI_C_BOOL, MPI_LAND, m_communicator);
#endif

    if(!checkSync) setUnsyncAll();

return !checkSync;

}
/*!
 * Unsynchronizes all MimmoObject patch structures, namely adjacencies, interfaces,
   skdtree, kdtree, internal patch numbering info, bounding box, and for MPI version ghost
   structures.
   If any of the structures has a SyncStatus flag lesser then UNSYNC, preserve it.
   This is a useful call when intense mesh modifications (insertion, deletion of vertices and cells) are
   done externally, and all structures needed to be re-synchronized as consequence. In that case
   use this method before calling the MimmoObject::update();
 */
void
MimmoObject::setUnsyncAll(){
    m_AdjSync = std::min(m_AdjSync,SyncStatus::UNSYNC);
    m_IntSync = std::min(m_IntSync, SyncStatus::UNSYNC);
    m_kdTreeSync = std::min(m_kdTreeSync, SyncStatus::UNSYNC);
    m_skdTreeSync = std::min(m_skdTreeSync, SyncStatus::UNSYNC);
    m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
    m_boundingBoxSync = SyncStatus::UNSYNC;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
	cleanPointConnectivity(); //forcefully destroy point connectivity.
};

/*!
 * Extract vertex list from an ensamble of geometry cells.
 *\param[in] cellList list of bitpit::PatchKernel ids identifying cells.
 *\return the list of bitpit::PatchKernel ids of involved vertices.
 */
livector1D MimmoObject::getVertexFromCellList(const livector1D &cellList){
    if(getType() == 3)   return livector1D(0);

    livector1D result;
    std::unordered_set<long int> ordV;
    bitpit::PiercedVector<bitpit::Cell> & cells = getCells();
    ordV.reserve(getPatch()->getVertexCount());
    //get conn from each cell of the list
    for(const long & id : cellList){
        if(cells.exists(id)){
            bitpit::ConstProxyVector<long> ids = cells.at(id).getVertexIds();
            for(const long & val: ids){
                ordV.insert(val);
            }
        }
    }
    result.reserve(ordV.size());
    result.insert(result.end(), ordV.begin(), ordV.end());
    return  result;
}

/*!
 * Extract interfaces list from an ensamble of geometry cells. Build Interfaces
 * if the target mesh has none.
 *\param[in] cellList list of bitpit::PatchKernel ids identifying cells.
 *\return the list of bitpit::PatchKernel ids of involved interfaces.
 *\param[in] all extract all interfaces af listed cells. Otherwise (if false) extract only the interfaces shared by the listed cells.
 */
livector1D MimmoObject::getInterfaceFromCellList(const livector1D &cellList, bool all){
	if(getType() == 3)   return livector1D(0);

	updateInterfaces();
	livector1D result;
	std::unordered_set<long int> ordV;
	auto patch = getPatch();
	ordV.reserve(patch->getInterfaceCount());

	if (all){
		bitpit::PiercedVector<bitpit::Cell> & cells = getCells();
		//get conn from each cell of the list
		for(const auto id : cellList){
			if(cells.exists(id)){
				bitpit::Cell & cell = cells.at(id);
				long * interf =cell.getInterfaces();
				int nIloc = cell.getInterfaceCount();
				for(int i=0; i<nIloc; ++i){
					if(interf[i] < 0) continue;
					ordV.insert(interf[i]);
				}
			}
		}
	}
	else{
		std::unordered_set<long> targetCells(cellList.begin(), cellList.end());
		for(const auto & interf : getInterfaces()){
			long idowner = interf.getOwner();
			long idneigh = interf.getNeigh();
			if (targetCells.count(idowner) && (targetCells.count(idneigh) || idneigh == bitpit::Element::NULL_ID)){
				ordV.insert(interf.getId());
			}
		}
	}
	result.reserve(ordV.size());
	result.insert(result.end(), ordV.begin(), ordV.end());
	return  result;
}


/*!
 * Extract cell list from an ensamble of geometry vertices. Ids of all those cells whose vertex are
 * defined inside the selection will be returned.
 * \param[in] vertexList list of bitpit::PatchKernel IDs identifying vertices.
 * \param[in] strict boolean true to restrict search for cells having all vertices in the list,
                     false to include all cells having at least 1 vertex in the list.
 * \return list of bitpit::PatchKernel IDs of involved 1-Ring cells.
 */
livector1D MimmoObject::getCellFromVertexList(const livector1D & vertexList, bool strict){
	if(getType() == 3)   return livector1D(0);

	livector1D result;
	std::unordered_set<long int> ordV, ordC;
	ordV.insert(vertexList.begin(), vertexList.end());
	ordC.reserve(getPatch()->getCellCount());
	//get conn from each cell of the list
	for(auto it = getPatch()->cellBegin(); it != getPatch()->cellEnd(); ++it){
		bitpit::ConstProxyVector<long> vIds= it->getVertexIds();
		bool check = false;
		for(const auto & id : vIds){
			check = (ordV.count(id) > 0);
			if(!check && strict) {
				break ;
			}
			if(check && !strict) {
				break ;
			}
		}
		if(check) ordC.insert(it.getId());
	}
	result.reserve(ordC.size());
	result.insert(result.end(), ordC.begin(), ordC.end());
	return  result;
}

/*!
 * Extract interfaces list from an ensamble of geometry vertices.
 * Ids of all those interfaces whose vertices are defined inside the
 * selection will be returned. Interfaces are automatically built, if the mesh has none.
   "strict" boolean, if true, controls that interfaces have all connectivity points inside the vertex list,
    if false insert all interfaces having at least one point inside such list.
    "border" boolean, if true, limits the search to interface lying on the mesh border. Please, be aware,
    in case of MPI distributed mesh, all borders of partition are considered, physical and rank borders.
 * \param[in] vertexList list of bitpit::PatchKernel IDs identifying vertices.
 * \param[in] strict boolean true to restrict search for cells having all vertices in the list,
                     false to include all cells having at least 1 vertex in the list.
 * \param[in] border boolean true to restrict the search to border interfaces, false to include all interfaces

 * \return list of bitpit::PatchKernel IDs of involved interfaces
 */

livector1D MimmoObject::getInterfaceFromVertexList(const livector1D & vertexList, bool strict, bool border){
	if(getType() == 3)   return livector1D(0);

	updateInterfaces();

	livector1D result;
	std::unordered_set<long int> ordV, ordI;
	ordV.insert(vertexList.begin(), vertexList.end());
	ordI.reserve(getPatch()->getInterfaceCount());
	//get conn from each cell of the list
	for(auto it = getPatch()->interfaceBegin(); it != getPatch()->interfaceEnd(); ++it){
		if(border && !it->isBorder()) continue;
		bitpit::ConstProxyVector<long> vIds= it->getVertexIds();
		bool check = false;
		for(const auto & id : vIds){
			check = (ordV.count(id) > 0);
			if(!check && strict) {
				break ;
			}
			if(check && !strict) {
				break ;
			}
		}
		if(check) ordI.insert(it.getId());
	}

	result.reserve(ordI.size());
	result.insert(result.end(), ordI.begin(), ordI.end());
	return  result;
}


/*!
 * Extract cells  who have one face at the mesh boundaries at least, if any.
 * Return the list of the local faces per cell, which lie exactly on the boundary.
 * The method is meant for connected mesh only, return empty list otherwise.
 * In MPI versions, if mesh is distributed and ghost search is activated, returns
   all cells (internal and ghost) that have a face properly lying on a physical boundary: faces
   lying on a rank border are excluded through an internal communication.
 *  \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
 * \return map of boundary cell unique-ids, with local boundary faces list.
 */
std::unordered_map<long, std::set<int> >
MimmoObject::extractBoundaryFaceCellID(bool ghost){

	std::unordered_map<long, std::set<int> > result;
	if(m_type ==3)   return result;

#if MIMMO_ENABLE_MPI == 0 //serial part only
    BITPIT_UNUSED(ghost);
#endif
    if(getAdjacenciesSyncStatus() != SyncStatus::SYNC)  updateAdjacencies(); //forcing adjacencies building
    update();

    //loop on internal cells
    int size;
    for (bitpit::PatchKernel::CellIterator it = getPatch()->internalCellBegin(); it != getPatch()->internalCellEnd(); ++it){
        size= it->getFaceCount();
        for(int face=0; face<size; ++face){
            if(it->isFaceBorder(face)){
                result[it.getId()].insert(face);
            }//endif
        }// end loop on face
    }

#if MIMMO_ENABLE_MPI
    //here, if you want ghost face physical borders, you need to do a communication
    // from local rank to neighbour ranks: send borderfaces of local rank internal sources,
    //and recover border faces on local ghosts from other ranks.

    if(ghost){

        size_t dataSize = sizeof(int);
        std::unique_ptr<bitpit::DataCommunicator> dataCommunicator(new bitpit::DataCommunicator(getCommunicator()));

        // Set and start the sends
        for (const auto & entry : getPatch()->getGhostCellExchangeSources()) {
            const int rank = entry.first;
            auto &list = entry.second;
            // Recover number of vertices
            int dataCount(0);
            for (long cellId : list){
                if(result.count(cellId) > 0) {
                    dataCount += int(result[cellId].size());
                }
            }

            dataCommunicator->setSend(rank, ( dataCount * dataSize + int(list.size()) * dataSize ) );
            bitpit::SendBuffer &buffer = dataCommunicator->getSendBuffer(rank);
            // Fill the buffer with borderfaces information on sources.
            for (long cellId : list){
                if(result.count(cellId) > 0){
                    buffer<<int(result[cellId].size());
                    for(int face : result[cellId]){
                        buffer<<face;
                    }
                }else{
                    buffer<<int(0);
                }
            }
            dataCommunicator->startSend(rank);
        }

        // Discover & start all the receives
        dataCommunicator->discoverRecvs();
        dataCommunicator->startAllRecvs();

        // Receive ghosts data from owners on other ranks
        int nCompletedRecvs = 0;

        while (nCompletedRecvs < dataCommunicator->getRecvCount()) {
            int rank = dataCommunicator->waitAnyRecv();
            const auto &list = getPatch()->getGhostCellExchangeTargets(rank);
            bitpit::RecvBuffer &buffer = dataCommunicator->getRecvBuffer(rank);
            // visit the list, all cellId in the list is a ghost for my local rank.
            //Collect all face information they had.
            int nface, face;
            for (long cellId : list){
                buffer >> nface;
                for(int i=0; i<nface; ++i){
                    buffer >> face;
                    result[cellId].insert(face);
                }
            }
            ++nCompletedRecvs;
        }
        // Wait for the sends to finish
        dataCommunicator->waitAllSends();

    } //end of if ghost

    //all ghost face borders are already appended to result
#endif

    return result;
};


/*!
 * Extract boundary cells who have one face at the mesh boundaries at least, if any.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with extractBoundaryFaceCellID
 * \return list of cell ids.
 */
livector1D
MimmoObject::extractBoundaryCellID(std::unordered_map<long, std::set<int> > & map){
    livector1D result;
    result.reserve(map.size());
    for(auto & touple: map){
        result.push_back(touple.first);
    }
    return result;
};

/*!
 * Extract cells who have one face at the mesh boundaries at least, if any.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
 * \return list of cell ids.
 */
livector1D
MimmoObject::extractBoundaryCellID(bool ghost){
    std::unordered_map<long, std::set<int> > map = extractBoundaryFaceCellID(ghost);
    return extractBoundaryCellID(map);
};

/*!
 * Extract vertices at the mesh boundaries.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with extractBoundaryFaceCellID
 * \return list of vertex ids.
 */
livector1D
MimmoObject::extractBoundaryVertexID(std::unordered_map<long, std::set<int> > &map){

    std::unordered_set<long> container;
    container.reserve(getPatch()->getVertexCount());

    for (auto & touple : map){
        bitpit::Cell & cell = getPatch()->getCell(touple.first);
        for(int face : touple.second){
            bitpit::ConstProxyVector<long> list = cell.getFaceVertexIds(face);
            for(long id : list ){
                container.insert(id);
            }
        }// end loop on face
    }

    livector1D result;
    result.reserve(container.size());
    result.insert(result.end(), container.begin(), container.end());

    return result;
};

/*!
 * Extract vertices at the mesh boundaries, if any. The method is meant for connected mesh only,
 * return empty list otherwise.
 * \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
 * \return list of vertex ids.
 */
livector1D
MimmoObject::extractBoundaryVertexID(bool ghost){
    std::unordered_map<long, std::set<int> > map = extractBoundaryFaceCellID(ghost);
    return extractBoundaryVertexID(map);
};

/*!
 * Extract interfaces who lies on the mesh boundaries.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with extractBoundaryFaceCellID
 * \return list of interface ids.
 */
livector1D
MimmoObject::extractBoundaryInterfaceID(std::unordered_map<long, std::set<int> > &map){

    updateInterfaces();

    std::unordered_set<long> container;
    container.reserve(getPatch()->getInterfaceCount());

    for(auto & tuple : map){
        long * interfCellList  = getPatch()->getCell(tuple.first).getInterfaces();
        for(int face : tuple.second){
            container.insert(interfCellList[face]);
        }
    }

    livector1D result;
    result.reserve(container.size());
    result.insert(result.end(), container.begin(), container.end());

    return result;
};

/*!
 * Extract interfaces who lies on the mesh boundaries.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] ghost true if the ghost interfaces must be accounted into the search, false otherwise
 * \return list of interface ids.
 */
livector1D
MimmoObject::extractBoundaryInterfaceID(bool ghost){
    std::unordered_map<long, std::set<int> > map = extractBoundaryFaceCellID(ghost);
    return extractBoundaryInterfaceID(map);
};

/*!
 * Extract boundary mesh if the current MimmoObject mesh.
   Boundaries will be stored in an indipendent MimmoObject container, whose topology is:
   - Surface Mesh (type 1 ) if the starting mesh is a Volume(2);
   - 3DCurve Mesh (type 4 ) if the starting mesh is a Surface(1);
   - Point Cloud  (type 3)  if the starting mesh is a 3DCurve(4);
   Bulk interfaces are used to create boundary tessellations.
   Bulk PIDs will be inherited into the boundary mesh.
   In MPI versions, boundaries will inherit the distributed status of the original mesh
 */
MimmoSharedPointer<MimmoObject>
MimmoObject::extractBoundaryMesh(){

    bool destroyBulkInterface = (getInterfacesSyncStatus() == SyncStatus::NONE);
    updateInterfaces();

    livector1D boundaryInterfaces, boundaryVertices;
    {
        std::unordered_map<long, std::set<int>> map = extractBoundaryFaceCellID(true);

        boundaryVertices = extractBoundaryVertexID(map);
        boundaryInterfaces = extractBoundaryInterfaceID(map);
    }

    //fill new boundary
    //instantiate mesh
    int type = int(getType() == 2) + 4*int(getType() == 1) + 3*int(getType() == 4);
    MimmoSharedPointer<MimmoObject> boundary(new MimmoObject(type, isParallel()));

    //get intefaces from current mesh
    bitpit::PiercedVector<bitpit::Interface> & bulkInterf = getInterfaces();

    //fill vertices
    for(const auto & idV: boundaryVertices){
        boundary->addVertex(getVertexCoords(idV),idV);
    }

    int rank;
    long PID;
    long * conn = nullptr;
    bitpit::ElementType eltype;
    for(long idI: boundaryInterfaces){
        bitpit::Interface & bInt = bulkInterf[idI];
        rank = -1;
        conn = bInt.getConnect();

#if MIMMO_ENABLE_MPI
        rank = getPatch()->getCellRank(bInt.getOwner());
#endif
        PID = getPatch()->getCell(bInt.getOwner()).getPID();
        boundary->addConnectedCell(livector1D(conn, conn+bInt.getConnectSize()), bInt.getType(), PID, idI, rank);
    }

    //check and release bulk Interfaces
    if(destroyBulkInterface) {
        destroyInterfaces();
    }



    //get a self consistent distributed representation of boundary
    // some ghost interfaces may result isolated check and clean it
#if MIMMO_ENABLE_MPI
    boundary->deleteOrphanGhostCells();
#endif

    if(boundary->getPatch()->countOrphanVertices() > 0){
        boundary->getPatch()->deleteOrphanVertices();
    }

    boundary->getPatch()->squeezeVertices();
    boundary->getPatch()->squeezeCells();

    boundary->update();

    return boundary;
}


/*!
  Return all cells faces at the mesh border. The method is meant for connected mesh only,
  return empty list otherwise.
  In MPI versions, differently from extractBoundaryFaceCellID, the method return all faces,
  those belonging to rank borders, plus those on physical borders, in non-collective way
  (operations on local rank, no communications)
  In serial version the method is equivalent to extractBoundaryFaceCellID.
   \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
  \return map of boundary cell unique-ids, with local boundary faces list.
 */
std::unordered_map<long, std::set<int> >
MimmoObject::getBorderFaceCells(){

	std::unordered_map<long, std::set<int> > result;
	if(m_type ==3)   return result;

    updateAdjacencies();
    //loop on internal cells
    int size;
    for (bitpit::PatchKernel::CellIterator it = getPatch()->cellBegin(); it != getPatch()->cellEnd(); ++it){
        size= it->getFaceCount();
        for(int face=0; face<size; ++face){
            if(it->isFaceBorder(face)){
                result[it.getId()].insert(face);
            }//endif
        }// end loop on face
    }

    return result;
};


/*!
 * Get all border cells who have one face at the mesh boundaries  at least(physical or rank border).
   In MPI versions, differently from extractBoundaryCellID, the method return all cells having border faces,
   those belonging to rank borders, plus those on physical borders, in non-collective way
   (operations on local rank, no communications).
   In serial version only is equivalent to method extractBoundaryCellID
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with getBorderFaceCells()
 * \return list of cell ids.
 */
livector1D
MimmoObject::getBorderCells(std::unordered_map<long, std::set<int> > & map){
    livector1D result;
    result.reserve(map.size());
    for(auto & touple: map){
        result.push_back(touple.first);
    }
    return result;
};

/*!
 * Get all border cells who have one face at the mesh boundaries  at least(physical or rank border).
   In MPI versions, differently from extractBoundaryCellID, the method return all cells having border faces,
   those belonging to rank borders, plus those on physical borders, in non-collective way
   (operations on local rank, no communications).
   In serial version only is equivalent to method extractBoundaryCellID
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with getBorderFaceCells()
 * \return list of cell ids.
 */
livector1D
MimmoObject::getBorderCells(){
    std::unordered_map<long, std::set<int> > map = getBorderFaceCells();
    return getBorderCells(map);
};

/*!
 * Get all border vertices belonging to border face at the mesh boundaries (physical or rank border).
   In MPI versions, differently from extractBoundaryVertexID, the method return all vertices belonging to border faces,
   those belonging to rank borders, plus those on physical borders, in non-collective way
   (operations on local rank, no communications).
   In serial version only is equivalent to method extractBoundaryVertexID
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with getBorderFaceCells()
 * \return list of vertex ids.
 */
livector1D
MimmoObject::getBorderVertices(std::unordered_map<long, std::set<int> > &map){

    std::unordered_set<long> container;
    container.reserve(getPatch()->getVertexCount());

    for (auto & touple : map){
        bitpit::Cell & cell = getPatch()->getCell(touple.first);
        for(int face : touple.second){
            bitpit::ConstProxyVector<long> list = cell.getFaceVertexIds(face);
            for(long id : list ){
                container.insert(id);
            }
        }// end loop on face
    }

    livector1D result;
    result.reserve(container.size());
    result.insert(result.end(), container.begin(), container.end());

    return result;
};

/*!
 * Get all border vertices belonging to border face at the mesh boundaries (physical or rank border).
   In MPI versions, differently from extractBoundaryVertexID, the method return all vertices belonging to border faces,
   those belonging to rank borders, plus those on physical borders, in non-collective way
   (operations on local rank, no communications).
   In serial version only is equivalent to method extractBoundaryVertexID
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] map of border faces previously calculated with getBorderFaceCells()
 * \return list of vertex ids.
 */
livector1D
MimmoObject::getBorderVertices(){
    std::unordered_map<long, std::set<int> > map = getBorderFaceCells();
    return getBorderVertices(map);
};


/*!
 * Extract all cells marked with a target PID flag.
    \param[in]   flag    PID for extraction
    \param[in]	squeeze		if true the result container is squeezed once full*
    \return  list of cells as unique-ids
 */
livector1D	MimmoObject::extractPIDCells(long flag, bool squeeze){

	if(m_pidsType.count(flag) < 1)	return livector1D(0);

	livector1D result(getNCells());
	int counter = 0;
	for(auto const & cell : getCells()){
		if ( cell.getPID() == flag)	{
			result[counter] = cell.getId();
			++counter;
		}
	}
	result.resize(counter);
	if (squeeze)
		result.shrink_to_fit();
	return  result;
};

/*!
 * Extract all cells marked with a series of target PIDs.
 * \param[in]   flag    list of PID for extraction
 * \param[in]	squeeze		if true the result container is squeezed once full
 * \return      list of cells as unique-ids
 */
livector1D	MimmoObject::extractPIDCells(livector1D flag, bool squeeze){
	livector1D result;
	result.reserve(getNCells());
	for(auto && id : flag){
		livector1D partial = extractPIDCells(id);
		result.insert(result.end(),partial.begin(), partial.end());
	}
	if (squeeze)
		result.shrink_to_fit();
	return(result);
};

/*!
 * Extract all cells ids rearranged according to PID subdivision.
    \return  map with PID key and list of cells belonging to PID as argument
 */
std::map<long, livector1D>
MimmoObject::extractPIDSubdivision(){

    std::map<long, livector1D> result;
    if (m_pidsType.empty()){
        return result;
    }
    int totCells = getNCells();
    int reserveSize = int(totCells*1.3)/int(m_pidsType.size()); //euristic to not overestimate the reserve on vectors.
    for(const long & entry: m_pidsType) {
        result[entry].reserve(reserveSize);
    }

    for(bitpit::PatchKernel::CellIterator it = getPatch()->cellBegin(); it!= getPatch()->cellEnd(); ++it){
        result[it->getPID()].push_back(it.getId());
    }
    for(std::pair<const long, livector1D> & entry: result){
        entry.second.shrink_to_fit();
    }

    return  result;
};

/*!
 * Check if a given connectivity list is coherent with a bitpit::ElementType type.
 *
 * \param[in] type cell type to check, as bitpit::ElementInfo enum
 * \param[in] conn connectivity list.
 * \return true if connectivity list is coherent with the specified cell type
 */
bool MimmoObject::checkCellConnCoherence(const bitpit::ElementType & type, const livector1D & list){

	switch(type){
	case bitpit::ElementType::VERTEX:
		return (list.size() == 1);
		break;
	case bitpit::ElementType::LINE:
		return (list.size() == 2);
		break;
	case bitpit::ElementType::TRIANGLE:
		return (list.size() == 3);
		break;
	case bitpit::ElementType::PIXEL:
		return (list.size() == 4);
		break;
	case bitpit::ElementType::QUAD:
		return (list.size() == 4);
		break;
	case bitpit::ElementType::POLYGON:
		if(list.size() < 3) return false;
		return(list.size() == std::size_t(list[0]+1));
		break;
	case bitpit::ElementType::TETRA:
		return (list.size() == 4);
		break;
	case bitpit::ElementType::VOXEL:
		return (list.size() == 8);
		break;
	case bitpit::ElementType::HEXAHEDRON:
		return (list.size() == 8);
		break;
	case bitpit::ElementType::WEDGE:
		return (list.size() == 6);
		break;
	case bitpit::ElementType::PYRAMID:
		return (list.size() == 5);
		break;
	case bitpit::ElementType::POLYHEDRON:
		if(list.size() < 9) return false;
		{
			//check if the record is a polyhedron
			long nFaces = list[0];
			std::size_t pos = 1;
			long countFaces = 0;
			while(pos < list.size() && countFaces<nFaces){
				countFaces++;
				pos += list[pos]+1;
			}
			return (pos == list.size() && countFaces == nFaces);
		}
		break;
	default:
		assert(false && "reached uncovered case");
		break;
	}
	return false;
};

/*!
 * Evaluate axis aligned bounding box of the current MimmoObject.
 * \param[out] pmin lowest bounding box point
 * \param[out] pmax highest bounding box point
 * \param[in] global true to ask for global bounding box over the processes
 */
void MimmoObject::getBoundingBox(std::array<double,3> & pmin, std::array<double,3> & pmax, bool global){
	getPatch()->getBoundingBox(global, pmin, pmax);
}

/*!
 * Reset and build again cell skdTree of your geometry (if supports connectivity elements).
 * Ghost cells are insert in the tree.
 *\param[in] value build the minimum leaf of the tree as a bounding box containing value elements at most.
 */
void MimmoObject::buildSkdTree(std::size_t value){
	if(!isSkdTreeSupported())   return;

	if (m_skdTreeSync != SyncStatus::SYNC){
		m_skdTree->clear();
		m_skdTree->build(value);
		m_skdTreeSync = SyncStatus::SYNC;
	}
	return;
}

/*!
 * Reset and build again vertex kdTree of your geometry.
 * Nodes fo ghost cells are insert in the tree.
 */
void MimmoObject::buildKdTree(){
	if( getNVertices() == 0)  return;
	long label;

	if (m_kdTreeSync != SyncStatus::SYNC){
		cleanKdTree();
		//TODO Why : + m_kdTree->MAXSTK ?
		m_kdTree->nodes.resize(getNVertices() + m_kdTree->MAXSTK);

		for(auto & val : getVertices()){
			label = val.getId();
			m_kdTree->insert(&val, label);
		}
		m_kdTreeSync = SyncStatus::SYNC;
	}
	return;
}

/*!
 * Clean the KdTree of the class
 */
void	MimmoObject::cleanKdTree(){
	m_kdTree->n_nodes = 0;
	m_kdTree->nodes.clear();
    if(m_kdTreeSync == SyncStatus::SYNC){
        m_kdTreeSync = SyncStatus::UNSYNC;
    }
}

/*!
 * Clean the SkdTree of the class
 */
void	MimmoObject::cleanSkdTree(){
    if (isSkdTreeSupported()){
        m_skdTree->clear();
        if(m_skdTreeSync == SyncStatus::SYNC){
            m_skdTreeSync = SyncStatus::UNSYNC;
        }
    }
}

/*!
 * Build cell patch numbering info of your geometry.
 */
void MimmoObject::buildPatchInfo(){
	if (m_internalPatch)
		m_patchInfo.setPatch(m_patch.get());
	else
		m_patchInfo.setPatch(m_extpatch);
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;
}

/*!
 * Reset cell patch numbering info of your geometry.
 */
void MimmoObject::cleanPatchInfo(){
    m_patchInfo.reset();
    if(m_infoSync == SyncStatus::SYNC){
        m_infoSync = SyncStatus::UNSYNC;
    }
}


/*!
 * Clean bounding box of your geometry (i.e. currently set to
 * SyncStatus::NONE the synchronization status).
 */
void MimmoObject::cleanBoundingBox(){
    if(m_boundingBoxSync == SyncStatus::SYNC) {
        m_boundingBoxSync = SyncStatus::UNSYNC;
    }
}

/*!
 * Update the MimmoObject after a manipulation, i.e. a mesh adaption, a cells/vertices insertion/delete or
 * a generic manipulation.
 * Note. To update parallel information the cell adjacencies have to be built.
 */
void MimmoObject::update()
{

#if MIMMO_ENABLE_MPI
    // Communicate sync status of all the structures
    if (isParallel()){
        cleanAllParallelSync();
    }
#endif

    // Status variable
    SyncStatus status;

    // Update adjacencies
    bool resetAdjacencies = false;
    status = m_AdjSync;
    if (status == SyncStatus::UNSYNC){
        updateAdjacencies();
    }
#if MIMMO_ENABLE_MPI
    else if (status == SyncStatus::NONE){
        // The adjacencies are needed in parallel case to update the patch in bitpit
        resetAdjacencies = true;
        updateAdjacencies();
    }
#endif

    // Update interfaces
    status = m_IntSync;
    if (status == SyncStatus::UNSYNC){
        updateInterfaces();
    }


    // UPDATE PATCH
    getPatch()->update();


    // Update trees
    status = m_skdTreeSync;
    if (status == SyncStatus::UNSYNC){
        buildSkdTree();
    }
    status = m_kdTreeSync;
    if (status == SyncStatus::UNSYNC){
        buildKdTree();
    }

    // Update patch info
    status = m_infoSync;
    if (status == SyncStatus::UNSYNC){
        buildPatchInfo();
    }

    // Update patch bounding box (//TODO when ready use automatic update in bitpit patch)
    status = m_boundingBoxSync;
    if (status != SyncStatus::SYNC){
        getPatch()->updateBoundingBox(true);
        m_boundingBoxSync = SyncStatus::SYNC;
    }

    // Update point connectivity
    status = getPointConnectivitySyncStatus();
    if (status == SyncStatus::UNSYNC){
        buildPointConnectivity();
    }

#if MIMMO_ENABLE_MPI
    // Always update/build point ghost exchange information
    status = m_pointGhostExchangeInfoSync;
    if (status != SyncStatus::SYNC){
        updatePointGhostExchangeInfo();
    }
#endif

    // Reset structures if needed
    if (resetAdjacencies){
        destroyAdjacencies();
    }
}

/*!
 * Return cell-cell adjacency synchronization status for your current mesh.
 * \return synchronization status of adjacencies.
 */
SyncStatus MimmoObject::getAdjacenciesSyncStatus(){

	//check if you are synchronized with patch
	if (getPatch()->getAdjacenciesBuildStrategy() == bitpit::PatchKernel::AdjacenciesBuildStrategy::ADJACENCIES_NONE){
	    m_AdjSync = SyncStatus::NONE;
	}

	return m_AdjSync;
};

/*!
 * Return cell-cell interfaces synchronization status for your current mesh.
 * \return synchronization status of interfaces.
 */
SyncStatus MimmoObject::getInterfacesSyncStatus(){

    //check if you are synchronized with patch
    if (getPatch()->getInterfacesBuildStrategy() == bitpit::PatchKernel::InterfacesBuildStrategy::INTERFACES_NONE){
        m_IntSync = SyncStatus::NONE;
    }

    return m_IntSync;
};

/*!
 * \return false if your local mesh has open edges/faces. True otherwise.
  Please note in distributed archs, the method cannot be called locally, but it needs to be called
  globally from each procs.
 */
bool MimmoObject::isClosedLoop(){

	updateAdjacencies();
	bool check = true;

	auto itp = getCells().cbegin();
	auto itend = getCells().cend();
	while(itp != itend && check){

		if (itp->isInterior()){
			int faces = itp->getFaceCount();
			for(int face=0; face<faces; ++face){
				// Check if it is a border face
				check = check && (!itp->isFaceBorder(face));
			}
			itp++;
		}
	}

#if MIMMO_ENABLE_MPI
	MPI_Allreduce(MPI_IN_PLACE, &check, 1, MPI_C_BOOL, MPI_LAND, m_communicator);
#endif

	return check;
};


/*!
 * If the mesh is composed by multiple unconnected patches or triage patches, the method returns
 * the cellID list belonging to each subpatches
 *\return list of cellIDs set for each subpatch
 */
//TODO PARALLEL VERSION (CURRENTLY USED ONLY IN CLOSEDCURVEINTERPOLATOR OF MIMIC)
livector2D MimmoObject::decomposeLoop(){
	updateAdjacencies();

	livector2D result;
	livector1D globalcellids = getCells().getIds();
	std::unordered_set<long> checked;
	bool outcycle = true;

	while(outcycle){

		livector1D stack, save;

		auto it = globalcellids.begin();
		bool found = false;
		long idstart = -1;
		while(!found && it != globalcellids.end()){
			if(checked.count(*it) == 0){
				idstart = *it;
				found = true;
			}
			++it;
		}

		checked.insert(idstart);
		stack.push_back(idstart);

		while(!stack.empty()){

			long target = stack.back();
			stack.pop_back();
			save.push_back(target);

			//get the number of faces.
			const bitpit::Cell & cell = getPatch()->getCells().at(target);
			int facecount = cell.getFaceCount();
			for(int i=0; i<facecount; ++i){

			    // Find face neighbours
                int neighscount = cell.getAdjacencyCount(i);
                const long * neighs = cell.getAdjacencies(i);
				bool found = false;
				int ineigh = 0;
				while(!found && ineigh < neighscount){
				    long neighId = neighs[ineigh];
					if(checked.count(neighId) == 0){
						stack.push_back(neighId);
						checked.insert(neighId);
						found = true;
					}
					++ineigh;
				}
			}
		}
		result.push_back(save);
		outcycle = (checked.size() < globalcellids.size());
	}

	return result;
}

/*!
 * Update cell-cell adjacency connectivity.
 */
void MimmoObject::updateAdjacencies(){

    switch(m_AdjSync){
    case SyncStatus::NOTSUPPORTED:
        return;
        break;
    case SyncStatus::NONE:
        getPatch()->initializeAdjacencies();
        break;
    case SyncStatus::UNSYNC:
        getPatch()->updateAdjacencies();
        break;
    case SyncStatus::SYNC:
        return;
        break;
    default:
        return;
        break;
    }
    m_AdjSync = SyncStatus::SYNC;

};

/*!
 * Update Interfaces connectivity. If needed the adjacencies
 * are updated too.
 * If MimmoObject does not support connectivity (as Point Clouds do)
 * does nothing.
 */
void MimmoObject::updateInterfaces(){

    if(m_AdjSync != SyncStatus::SYNC){
        updateAdjacencies();
    }

    switch(m_IntSync){
    case SyncStatus::NOTSUPPORTED:
        return;
        break;
    case SyncStatus::NONE:
        getPatch()->initializeInterfaces();
        break;
    case SyncStatus::UNSYNC:
        getPatch()->updateInterfaces();
        break;
    case SyncStatus::SYNC:
        return;
        break;
    default:
        return;
        break;
    }
    m_IntSync = SyncStatus::SYNC;

};

/*!
 * Destroy cell-cell adjacency connectivity.
 */
void MimmoObject::destroyAdjacencies(){
    getPatch()->destroyAdjacencies();
    m_AdjSync = SyncStatus::NONE;
};

/*!
 * Destroy Interfaces connectivity.
 */
void MimmoObject::destroyInterfaces(){
    getPatch()->destroyInterfaces(); // is the same as getPatch()->destroyInterfaces
    m_IntSync = SyncStatus::NONE;

};

/*!
 * Force the class to reset all the patch structures, i.e. vertices, cells, interfaces.
 */
void MimmoObject::resetPatch(){
	getPatch()->reset();
	m_AdjSync = SyncStatus::NONE;
	m_IntSync = SyncStatus::NONE;
	cleanSkdTree();
	m_skdTreeSync = SyncStatus::NONE;
    cleanKdTree();
	m_kdTreeSync = SyncStatus::NONE;
	m_patchInfo.reset();
    m_infoSync = SyncStatus::NONE;
    m_boundingBoxSync = SyncStatus::NONE;
#if MIMMO_ENABLE_MPI
	resetPointGhostExchangeInfo();
	m_pointGhostExchangeInfoSync = SyncStatus::NONE;
#endif
	cleanPointConnectivity();
	m_pointConnectivitySync = SyncStatus::NONE;
};

/*!
 * Desume Element given the vertex connectivity list associated. Polygons and Polyhedra require
 * a special writing for their connectivity list. Please read doxy of
   MimmoObject(int type, dvecarr3E & vertex, livector2D * connectivity = nullptr) custom constructor
   for further information.
 * Return undefined type for unexistent or unsupported element.
 * \param[in] locConn list of vertex indices composing the cell.
 * \return cell type hold by the current connectivity argument.
 */
//TODO To review in order to implement new derived classes MimmoSurfUnstructured and MimmoVolUnstructured
bitpit::ElementType	MimmoObject::desumeElement(const livector1D & locConn){
	bitpit::ElementType result = bitpit::ElementType::UNDEFINED;

	std::size_t sizeConn = locConn.size();
	switch(m_type){
	case    1:
		if(sizeConn == 3)        result = bitpit::ElementType::TRIANGLE;
		if(sizeConn == 4)        result = bitpit::ElementType::QUAD;
		if(sizeConn > 4 && sizeConn == std::size_t(locConn[0]+1))    result= bitpit::ElementType::POLYGON;
		break;
	case    2:
		if(sizeConn == 4)        result = bitpit::ElementType::TETRA;
		if(sizeConn == 8)        result = bitpit::ElementType::HEXAHEDRON;
		if(sizeConn == 5)        result = bitpit::ElementType::PYRAMID;
		if(sizeConn == 6)        result = bitpit::ElementType::WEDGE;
		if(sizeConn > 8 ){
			//check if the record is a polyhedron
			long nFaces = locConn[0];
			std::size_t pos = 1;
			long countFaces = 0;
			while(pos < sizeConn && countFaces<nFaces){
				countFaces++;
				pos += locConn[pos]+1;
			}
			if(pos == sizeConn && countFaces == nFaces){
				result= bitpit::ElementType::POLYHEDRON;
			}
		}
		break;
	case    3:
		if(sizeConn == 1) result = bitpit::ElementType::VERTEX;
		break;
	case    4:
		if(sizeConn == 2) result = bitpit::ElementType::LINE;
		break;
	default :
		assert(false && "reached uncovered case");
		break;
	}

	return result;
};

/*!
 * Reset class to default constructor set-up.
 * \param[in] type type of mesh from 1 to 4; See default constructor.
 * \param[in] isParallel if true reset to parallel MimmoObject (if MPI not enabled forced to false)
 */
void MimmoObject::reset(int type, bool isParallel){

    m_log = &bitpit::log::cout(MIMMO_LOG_FILE);

    // Reset patch
    resetPatch();

    m_patch = nullptr;
    m_extpatch = nullptr;

    // Initialize parallel data
    m_isParallel = isParallel && MIMMO_ENABLE_MPI;
#if MIMMO_ENABLE_MPI
    initializeMPI();
    initializeCommunicator(m_isParallel);
#else
    m_rank = 0;
    m_nprocs = 1;
#endif

	m_type = std::max(0, type);
	if (m_type > 4){
		m_type = 1;
	}

	switch(m_type){
	case 1:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(2, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(2)));
#endif
		m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoVolUnstructured(3, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoVolUnstructured(3)));
#endif
        m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::VolumeSkdTree(dynamic_cast<bitpit::VolumeKernel*>(m_patch.get()))));
        m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
        break;
    case 3:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoPointCloud(getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoPointCloud()));
#endif
        m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
        break;
    case 4:
#if MIMMO_ENABLE_MPI
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(1, getCommunicator())));
#else
        m_patch = std::move(std::unique_ptr<bitpit::PatchKernel>(new MimmoSurfUnstructured(1)));
#endif
        m_skdTree = std::move(std::unique_ptr<bitpit::PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<bitpit::SurfaceKernel*>(m_patch.get()))));
        m_kdTree  = std::move(std::unique_ptr<bitpit::KdTree<3,bitpit::Vertex,long> >(new bitpit::KdTree<3,bitpit::Vertex, long>()));
        break;
	default:
		//never been reached
		break;
	}

	m_internalPatch = true;

    // Set skdTreeSync and kdTreeSync to none
    m_skdTreeSync = SyncStatus::NONE;
    m_kdTreeSync = SyncStatus::NONE;

    // Set skdTreeSync to unsupported for point cloud
    if (m_type == 3){
        m_skdTreeSync = SyncStatus::NOTSUPPORTED;
    }

    // Set AdjSync and IntSync to none
    m_AdjSync = SyncStatus::NONE;
    m_IntSync = SyncStatus::NONE;

	m_patchInfo.setPatch(m_patch.get());
	m_infoSync = SyncStatus::NONE;
    m_boundingBoxSync = SyncStatus::NONE;
	m_pointConnectivitySync = SyncStatus::NONE;
}

/*!
 * Dump contents of your current MimmoObject to a stream.
 * Dump all strict necessary information except for search tree structure,
 * Point ghost exchange info and point connectivity are recreated when restores.
 * Write all in a binary format.
 * \param[in,out] stream to write on.
 */
void MimmoObject::dump(std::ostream & stream){

	//scan pids inside your patch to be sure everything it's in order.
	resyncPID();
	//reverse pid and pidnames in vector structures.
	auto mapPid = getPIDTypeListWNames() ;
	std::vector<long> pid(mapPid.size());
	std::vector<std::string> sspid(mapPid.size());
	int counter = 0;
	for(const auto & touple : mapPid){
		pid[counter] = touple.first;
		sspid[counter] = touple.second;
		++counter;
	}
	//write comparison variables
	bitpit::utils::binary::write(stream,m_type);
#if MIMMO_ENABLE_MPI
	bitpit::utils::binary::write(stream,m_nprocs);
#endif
	//write other variables
	bitpit::utils::binary::write(stream,pid);
	bitpit::utils::binary::write(stream,sspid);
	bitpit::utils::binary::write(stream,m_AdjSync);
	bitpit::utils::binary::write(stream,m_IntSync);
	bitpit::utils::binary::write(stream,m_pointConnectivitySync);

	//write the patch
	getPatch()->dump(stream);
}

/*!
 * Restore contents of a dumped MimmoObject from a stream in your current class.
 * New restored data will be owned internally by the class.
 * Every data previously stored will be lost.
 * Tree are not restored by default.
 * Read all in a binary format.
 * \param[in,out] stream to write on.
 */

void MimmoObject::restore(std::istream & stream){
	//check comparisons
	int type;
	bitpit::utils::binary::read(stream,type);
#if MIMMO_ENABLE_MPI
	int nprocs;
	bitpit::utils::binary::read(stream,nprocs);
	if(nprocs != m_nprocs){
		throw std::runtime_error("Error during MimmoObject::restore :  uncoherent procs number between contents and container");
	}
#endif

	//clean up and reset current object to ist virgin state
	// Reset to a parallel geometry if communicator is different from MPI_COMM_NULL
	bool isparallel = false;
#if MIMMO_ENABLE_MPI
	isparallel = m_communicator != MPI_COMM_NULL;
#endif
	reset(type, isparallel);
	//absorb the other data
	std::vector<long> pid;
	std::vector<std::string> sspid;

	bitpit::utils::binary::read(stream,pid);
	bitpit::utils::binary::read(stream,sspid);
	bitpit::utils::binary::read(stream,m_AdjSync);
	bitpit::utils::binary::read(stream,m_IntSync);
	bitpit::utils::binary::read(stream,m_pointConnectivitySync);

	getPatch()->restore(stream);

	//m_patchInfo has already the pointer to the patch set during reset(type)
	//update it.
	m_patchInfo.update();
	m_infoSync = SyncStatus::SYNC;

	getPatch()->updateBoundingBox();
    m_boundingBoxSync = SyncStatus::SYNC;

	//rebuild the point ghost exchange information.
#if MIMMO_ENABLE_MPI
	updatePointGhostExchangeInfo();
#endif

	//check point connectivity, if true build it, otherwise leave it as reset put it, i.e. false.
	if(m_pointConnectivitySync == SyncStatus::SYNC){
		this->buildPointConnectivity();
	}

	//uncompact and rewrite PID stuff
	int count = 0;
	for (long pp : pid){
		m_pidsType.insert(pp);
		m_pidsTypeWNames.insert( std::make_pair( pp, sspid[count]));
		++count;
	}

	//that's all folks.
}

/*!
 * Evaluate general volume of each cell in the current local mesh,
 * according to its topology.
 * If no patch or empty patch or pointcloud patch is present
 * in the current MimmoObject return empty map as result.
 * Ghost cells are considered.
 * \param[out] volumes volume values referred to cell ids
 */
void
MimmoObject::evalCellVolumes(bitpit::PiercedVector<double> & volumes){

	if(getPatch() == nullptr)   return;
	if(isEmpty())       return ;

	switch (getType()){
	case 1:
	case 4:
	{
		bitpit::SurfaceKernel * p = static_cast<bitpit::SurfaceKernel *>(getPatch());
		for (const auto & cell: getCells()){
			volumes.insert(cell.getId(), p->evalCellArea(cell.getId()));
		}
	}
	break;
	case 2:
	{
		bitpit::VolumeKernel * p = static_cast<bitpit::VolumeKernel *>(getPatch());
		for (const auto & cell: getCells()){
			volumes.insert(cell.getId(), p->evalCellVolume(cell.getId()));
		}
	}
	break;
	default:
		//leave map empty
		break;
	}
}

/*!
 * Evaluate Aspect Ratio of each cell in the current local mesh,
 * according to its topology.
 * VERTEX and LINE elements does not support a proper definition of Aspect Ratio
 * and will return always 0.0 as value. PointCloud and 3DCurve MimmoObject return an empty map.
 * If no patch or empty patch or pointcloud patch is present
 * in the current MimmoObject return empty map as result.
 * Ghost cells are considered.
 * \param[out] ARs aspect ratio values referred to cell id
 */
void
MimmoObject::evalCellAspectRatio(bitpit::PiercedVector<double> & ARs){

	if(getPatch() == nullptr)   return;
	if(isEmpty())       return;

	switch (getType()){
	case 1:
	{
		bitpit::SurfaceKernel * p = static_cast<bitpit::SurfaceKernel *>(getPatch());
		int edge;
		for (const auto & cell: getCells()){
			ARs[cell.getId()] = p->evalAspectRatio(cell.getId(), edge);
		}
	}
	break;
	case 2:
	{   //Following the example of OpenFoam, the AR index is calculated as
		// the ratio S between total surface and hydraulic surface of an equilater
		//   cylinder (h=2*r) of the same volume, that is S_hyd = 1/6.0 * (V^(2/3)).
	    updateInterfaces();
		bitpit::VolUnstructured * p = static_cast<bitpit::VolUnstructured *>(getPatch());

		//calculate interface area
		std::unordered_map<long, double> interfaceAreas;
		for (const auto & interf: getInterfaces()){
			interfaceAreas[interf.getId()] = p->evalInterfaceArea(interf.getId());
		}

		double Svalue = 0.0;
		double sumArea;
		int size;
		for (const auto & cell: getCells()){

			sumArea = 0.0;

			size = cell.getInterfaceCount();
			const long * conInt = cell.getInterfaces();

			for(int i=0; i<size; ++i){
				sumArea += interfaceAreas[conInt[i]];
			}

			double vol = p->evalCellVolume(cell.getId());
			if(vol <= std::numeric_limits<double>::min()){
				Svalue = std::numeric_limits<double>::max();
			}else{
				Svalue = sumArea/(6.0*std::pow(vol, 2.0/3.0));
			}

			ARs.insert(cell.getId(), Svalue);
		}

	}
	break;
	default:
		//leave map empty
		break;
	}
}

/*!
 * Evaluate volume of a target cell in the current local mesh.
 * \param[in] id cell id.
 * \return cell volume
 */
double
MimmoObject::evalCellVolume(const long & id){
	switch (getType()){
	case 1:
	case 4:
		return static_cast<bitpit::SurfaceKernel *>(getPatch())->evalCellArea(id);
		break;
	case 2:
		return static_cast<bitpit::VolumeKernel *>(getPatch())->evalCellVolume(id);
		break;
	default:
		return 0.0;
		break;
	}
}

/*!
 * Evaluate Aspect Ratio of a target local cell.
 * VERTEX and LINE elements does not support a proper definition of Aspect Ratio
 * and will return always 0.0 as value.
 * \param[in] id of the cell
 * \return value of cell AR.
 */
double
MimmoObject::evalCellAspectRatio(const long & id){
	int edge;
	switch (getType()){
	case 1:
		return static_cast<bitpit::SurfaceKernel *>(getPatch())->evalAspectRatio(id, edge);
		break;
	case 2:
	{
		//Following the example of OpenFoam, the AR index is calculated as
		// the ratio S between total surface and hydraulic surface of an equilater
		//   cylinder (h=2*r) of the same volume, that is S_hyd = 1/6.0 * (V^(2/3)).
	    updateInterfaces();
		bitpit::VolUnstructured * p = static_cast<bitpit::VolUnstructured *>(getPatch());

		double sumArea = 0.0;
		int size = p->getCell(id).getInterfaceCount();
		const long * conInt = p->getCell(id).getInterfaces();
		for(int i=0; i<size; ++i){
			sumArea += p->evalInterfaceArea(conInt[i]);
		}

		double vol = p->evalCellVolume(id);
		if(vol <= std::numeric_limits<double>::min()){
			return std::numeric_limits<double>::max();
		}else{
			return sumArea/(6.0*std::pow(vol, 2.0/3.0));
		}
	}
	break;
	default:
		return 0.0;
		break;
	}
}

/*!
 * \return map of all bitpit::ElementType (casted as integers) hold by
   the current bitpit::PatchKernel obj. Ghost cells are considered.
 * \param[in] obj target PatchKernel object.
 */
std::unordered_set<int>
MimmoObject::elementsMap(bitpit::PatchKernel & obj){

	std::unordered_set<int> result;

	for(const auto & cell : obj.getCells()){
		result.insert((int)cell.getType());
	}
	return result;
};

/*!
 * Get all the cells of the current mesh whose center is within a prescribed
   distance maxdist w.r.t to a target surface body.
 * Ghost cells are considered. See twin method getCellsNarrowBandToExtSurfaceWDist.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[in] seedlist (optional) list of cells to starting narrow band search
 * \return list of cells inside the narrow band at maxdist from target surface.
 */
livector1D
MimmoObject::getCellsNarrowBandToExtSurface(MimmoObject & surface, const double & maxdist, livector1D * seedlist){
	bitpit::PiercedVector<double> res = getCellsNarrowBandToExtSurfaceWDist(surface, maxdist, seedlist);
	return res.getIds();
};

/*!
 * Get all the cells of the current mesh whose center is within a prescribed distance
   maxdist w.r.t to a target surface body.
 * Ghost cells are considered.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[in] seedlist (optional) list of cells to starting narrow band search.
 * \return list of cells inside the narrow band at maxdist from target surface with their distance from it.
 */
bitpit::PiercedVector<double>
MimmoObject::getCellsNarrowBandToExtSurfaceWDist(MimmoObject & surface, const double & maxdist, livector1D * seedlist){

    //TODO this propagation does not work properly in parallel. Please review it
    //using propagated distance on ghost.
    surface.updateAdjacencies();

    if (m_skdTreeSync != SyncStatus::SYNC)
        buildSkdTree();

    if (surface.getSkdTreeSyncStatus() != SyncStatus::SYNC)
        surface.buildSkdTree();

    bitpit::PiercedVector<double> result;
    if(surface.getType() != 1) return result;
    if(getType() == 3)  return result;

    //use a stack-based search on face cell neighbors of some prescribed seed cells.
    //You need at least one cell to be near enough to surface

    //First step check seed list and precalculate distance. If dist >= maxdist
    //the seed candidate is out.
    int npoints(0);
    darray3E point;
    dvecarr3E points;
    dvector1D distances;
    livector1D surface_ids;
    livector1D candidates;
    double distance, maxdistance(maxdist);
    long id;



    if(seedlist){
        //check if seedlist contains valid cell ids for the current mesh/local rank partition.
        candidates.reserve(seedlist->size());
        bitpit::PiercedVector<bitpit::Cell> & cells = getCells();
        for(long id : *seedlist){
            if(cells.exists(id))    candidates.push_back(id);
        }
        candidates.shrink_to_fit();
        // Fill points array with seed list
        npoints = candidates.size();
        points.reserve(npoints);
        for(long idseed : candidates){
            points.emplace_back(getPatch()->evalCellCentroid(idseed));
        }

    } else {

        // Find the seeds by using all the cells of surface patch
        // Recover the seeds by selecting by patch the target cells
        // by using the surface input patch as selection patch.
        // Then check if the candidate cells are closer than maxdistance,
        // in this case insert in result as seeds


#if MIMMO_ENABLE_MPI
        if (surface.isParallel()){
            candidates = skdTreeUtils::selectByGlobalPatch(surface.getSkdTree(), getSkdTree(), maxdistance);
        } else
#endif
        {
            candidates = skdTreeUtils::selectByPatch(surface.getSkdTree(), getSkdTree(), maxdistance);
        }

        npoints = candidates.size();
        points.reserve(npoints);
        for(long idseed : candidates){
            points.emplace_back(getPatch()->evalCellCentroid(idseed));
        }

    } // end if seed list is null


    distances.resize(npoints, std::numeric_limits<double>::max());
    surface_ids.resize(npoints, bitpit::Cell::NULL_ID);

    // Compute distances from surface
#if MIMMO_ENABLE_MPI
    if (surface.isParallel()){
        ivector1D surface_ranks(npoints, -1);
        skdTreeUtils::globalDistance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), surface_ranks.data(), distances.data(), maxdistance);
    } else
#endif
    {
        skdTreeUtils::distance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), distances.data(), maxdistance);
    }

    // Fill seeds (directly in result) if distance < maxdistance
    for (int ipoint = 0; ipoint < npoints; ipoint++){
        distance = distances[ipoint];
        id = candidates[ipoint];
        if(distance < maxdist){
            result.insert(id, distance);
        }
    }

	// create the stack of neighbours with the seed i have.
	std::unordered_set<long> visited;
	livector1D stackNeighs, newStackNeighs;
	std::unordered_set<long> tt;
	for(auto it = result.begin(); it != result.end(); ++it){
		livector1D neighs = getPatch()->findCellNeighs(it.getId(), 1); //only face neighs.
		for(long id: neighs){
			if(!result.exists(id))  visited.insert(id);
		}
	}
	stackNeighs.insert(stackNeighs.end(), visited.begin(), visited.end());

	//add as visited also the id already in result;
	{
		livector1D temp = result.getIds();
		visited.insert(temp.begin(), temp.end());
	}

	// Search until the stack is empty.
	// In case of parallel and partitioned test the global stack size
	bool stackEmpty = stackNeighs.empty();
#if MIMMO_ENABLE_MPI
    if (isParallel()){
        MPI_Allreduce(MPI_IN_PLACE, &stackEmpty, 1, MPI_C_BOOL, MPI_LAND, m_communicator);
    }
#endif
    while(!stackEmpty){

        // use all stack neighs as candidates
        visited.insert(stackNeighs.begin(), stackNeighs.end());

        // evaluate their distances;
        npoints = stackNeighs.size();
        points.clear();
        points.reserve(npoints);
        distances.clear();
        distances.resize(npoints, std::numeric_limits<double>::max());
        surface_ids.clear();
        surface_ids.resize(npoints, bitpit::Cell::NULL_ID);
        for (long idtarget : stackNeighs){
            points.emplace_back(getPatch()->evalCellCentroid(idtarget));
        }

#if MIMMO_ENABLE_MPI
        if (surface.isParallel()){
            ivector1D surface_ranks(npoints, -1);
            skdTreeUtils::globalDistance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), surface_ranks.data(), distances.data(), maxdistance);
        } else
#endif
        {
            skdTreeUtils::distance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), distances.data(), maxdistance);
        }

        // Fill points inside narrow band in result and their neighbours in stack
        for (int ipoint = 0; ipoint < npoints; ipoint++){
            distance = distances[ipoint];
            id = stackNeighs[ipoint];
            if(distance < maxdist){
                result.insert(id, distance);
                livector1D neighs = getPatch()->findCellNeighs(id, 1); //only face neighs.
                for(long id : neighs){
                    if(visited.count(id) < 1){
                        visited.insert(id);
                        newStackNeighs.push_back(id);
                    }
                }
            }
        } // end loop on points

        // Empty stack by old targets and push the new stack neighs
        stackNeighs.swap(newStackNeighs);
        livector1D().swap(newStackNeighs);

        stackEmpty = stackNeighs.empty();
#if MIMMO_ENABLE_MPI
        if (isParallel()){
            MPI_Allreduce(MPI_IN_PLACE, &stackEmpty, 1, MPI_C_BOOL, MPI_LAND, m_communicator);
        }
#endif

    } // end while stack empty
    return result;

};

/*!
 * Get all the vertices of the current mesh  within a prescribed
   distance maxdist w.r.t to a target surface body.
 * Ghost vertices are considered. See twin method getCellsNarrowBandToExtSurfaceWDist.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[in] seedlist (optional) list of vertices to starting narrow band search.
              In case the current class is a point cloud, a non empty seed list is mandatory.
 * \return list of vertices inside the narrow band at maxdist from target surface.
 */
livector1D
MimmoObject::getVerticesNarrowBandToExtSurface(MimmoObject & surface, const double & maxdist, livector1D * seedlist){
	bitpit::PiercedVector<double> res = getVerticesNarrowBandToExtSurfaceWDist(surface, maxdist, seedlist);
	return res.getIds();
};

/*!
 * Get all the vertices of the current mesh within a prescribed distance
   maxdist w.r.t to a target surface body.
 * Ghost vertices are considered.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[in] seedlist (optional) list of vertices to starting narrow band search.
              In case the current class is a point cloud, a non empty seed list is mandatory.
 * \return list of vertices inside the narrow band at maxdist from target surface with their distance from it.
 */
bitpit::PiercedVector<double>
MimmoObject::getVerticesNarrowBandToExtSurfaceWDist(MimmoObject & surface, const double & maxdist, livector1D * seedlist){

    //TODO this propagation does not work properly in parallel. Please review it
    //using propagated distance on ghost.

    bitpit::PiercedVector<double> result;
    if(surface.getType() != 1) return result;

    surface.updateAdjacencies();

    if (m_skdTreeSync != SyncStatus::SYNC)
        buildSkdTree();

    if (surface.getSkdTreeSyncStatus() != SyncStatus::SYNC)
        surface.buildSkdTree();

    if(getPointConnectivitySyncStatus() != SyncStatus::SYNC)
        buildPointConnectivity();

    //use a stack-based search on ring vertex neighbors of some prescribed seed vertex.
    //You need at least one vertex to be near enough to surface

    //First step check seed list and precalculate distance. If dist >= maxdist
    //the seed candidate is out.
    int npoints(0);
    darray3E point;
    dvecarr3E points;
    dvector1D distances;
    livector1D surface_ids;
    livector1D candidates;
    double distance, maxdistance(maxdist);
    long id;

    // Fill points candidates with seedlist or found candidates
    if(seedlist){
        //check if seedlist contains valid vertex ids for the current mesh/local rank partition.
        candidates.reserve(seedlist->size());
        bitpit::PiercedVector<bitpit::Vertex> & verts = getVertices();
        for(long id : *seedlist){
            if(verts.exists(id))    candidates.push_back(id);
        }
        candidates.shrink_to_fit();
        // Fill points array with seed list
        npoints = candidates.size();
        points.reserve(npoints);
        for(long idseed : candidates){
            points.emplace_back(getVertexCoords(idseed));
        }

    }else if(getType() != 3){

        // Find the seeds by using all the cells of surface patch
        // Recover the seeds by selecting by patch the target cells
        // by using the surface input patch as selection patch.
        // Then check if the candidate cells are closer than maxdistance,
        // in this case insert in result as seeds

        // Create cell candidates structure
        livector1D cell_candidates;
#if MIMMO_ENABLE_MPI
        if (surface.isParallel()){
            cell_candidates = skdTreeUtils::selectByGlobalPatch(surface.getSkdTree(), getSkdTree(), maxdistance);
        } else
#endif
        {
            cell_candidates = skdTreeUtils::selectByPatch(surface.getSkdTree(), getSkdTree(), maxdistance);
        }

        livector1D candidates = getVertexFromCellList(cell_candidates);
        // Fill points candidates with all the vertices of the found cells
        for(long idseed : candidates){
            points.emplace_back(getVertexCoords(idseed));
        }

    } // end if seed list is null

    distances.resize(npoints, std::numeric_limits<double>::max());
    surface_ids.resize(npoints, bitpit::Cell::NULL_ID);
    // Compute distances from surface
#if MIMMO_ENABLE_MPI
    if (surface.isParallel()){
        ivector1D surface_ranks(npoints, -1);
        skdTreeUtils::globalDistance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), surface_ranks.data(), distances.data(), maxdistance);
    } else
#endif
    {
        skdTreeUtils::distance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), distances.data(), maxdistance);
    }

    // Fill seed points (directly in result) if distance < maxdistance
    for (int ipoint = 0; ipoint < npoints; ipoint++){
        distance = distances[ipoint];
        id = candidates[ipoint];
        if(distance < maxdist){
            result.insert(id, distance);
        }
    }

    // create the stack of neighbours with the seed i have.
    std::unordered_set<long> visited;
    livector1D stackNeighs, newStackNeighs;
    std::unordered_set<long> tt;
    for(auto it = result.begin(); it != result.end(); ++it){
        std::unordered_set<long> neighs = getPointConnectivity(it.getId()); //only edge connected neighbours.
        for(long id: neighs){
            if(!result.exists(id))  visited.insert(id);
        }
    }
    stackNeighs.insert(stackNeighs.end(), visited.begin(), visited.end());

    //add as visited also the id already in result;
    {
        livector1D temp = result.getIds();
        visited.insert(temp.begin(), temp.end());
    }

    // Search until the stack is empty.
    // In case of parallel and partitioned test the global stack size
    bool stackEmpty = stackNeighs.empty();
#if MIMMO_ENABLE_MPI
    if (isParallel()){
        MPI_Allreduce(MPI_IN_PLACE, &stackEmpty, 1, MPI_C_BOOL, MPI_LAND, m_communicator);
    }
#endif
    while(!stackEmpty){

        // use all stack neighs as candidates
        visited.insert(stackNeighs.begin(), stackNeighs.end());

        // evaluate their distances;
        npoints = stackNeighs.size();
        points.clear();
        points.reserve(npoints);
        distances.clear();
        distances.resize(npoints, std::numeric_limits<double>::max());
        surface_ids.clear();
        surface_ids.resize(npoints, bitpit::Cell::NULL_ID);
        for (long idtarget : stackNeighs){
            points.emplace_back(getVertexCoords(idtarget));
        }

#if MIMMO_ENABLE_MPI
        if (surface.isParallel()){
            ivector1D surface_ranks(npoints, -1);
            skdTreeUtils::globalDistance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), surface_ranks.data(), distances.data(), maxdistance);
        } else
#endif
        {
            skdTreeUtils::distance(npoints, points.data(), surface.getSkdTree(), surface_ids.data(), distances.data(), maxdistance);
        }

        // Fill points inside narrow band in result and their neighbours in stack
        for (int ipoint = 0; ipoint < npoints; ipoint++){
            distance = distances[ipoint];
            id = stackNeighs[ipoint];
            if(distance < maxdist){
                result.insert(id, distance);
                std::unordered_set<long int> neighs = getPointConnectivity(id); //only edge connected
                for(long id : neighs){
                    if(visited.count(id) < 1){
                        visited.insert(id);
                        newStackNeighs.push_back(id);
                    }
                }
            }
        } // end loop on points

        // Empty stack by old targets and push the new stack neighs
        stackNeighs.swap(newStackNeighs);
        livector1D().swap(newStackNeighs);

        stackEmpty = stackNeighs.empty();
#if MIMMO_ENABLE_MPI
        if (isParallel()){
            MPI_Allreduce(MPI_IN_PLACE, &stackEmpty, 1, MPI_C_BOOL, MPI_LAND, m_communicator);
        }
#endif

    } // end while stack empty

    return result;

};


/*!
 * Get a minimal inverse connectivity of a target geometry mesh.
 * Each vertex (passed by Id) is associated at list to one of the possible simplex
 * (passed by Id) which it belongs. This is returned in an unordered_map having as key the
 * vertex Id and as value the Cell id. Id is meant as the unique label identifier associated
 * to bitpit::PatchKernel original geometry
 * Ghost cells are considered.
 *\return    unordered_map of vertex ids (key) vs cell-belonging-to ids(value)
 */
std::unordered_map<long,long> MimmoObject::getInverseConnectivity(){

	std::unordered_map<long,long> invConn ;

	long cellId;
	for(const auto &cell : getCells()){
		cellId = cell.getId();
		bitpit::ConstProxyVector<long> vList = cell.getVertexIds();
		for(const auto & idV : vList){
			invConn[idV] = cellId;
		}
	}

	return(invConn);
};

/*!
 * Return Vertex-Vertex One Ring of a specified target vertex. Does not work with unconnected meshes (point clouds).
 * In case of distributed patches, the input cell must be an interior cell;
 * in case of ghost cell passed, it returns an empty set.
 * \param[in]    cellId     bitpit::PatchKernel Id of a cell which target belongs to
 * \param[in]    vertexId    bitpit::PatchKernel Id of the target vertex
 * \return        list of all vertex in the One Ring of the target, by their bitpit::PatchKernel Ids
 */
std::set<long> MimmoObject::findVertexVertexOneRing(const long & cellId, const long & vertexId){

    std::set<long> result;
    if( getType() == 3 )  return result;

	bitpit::PatchKernel * tri = getPatch();
	bitpit::Cell &cell =  tri->getCell(cellId);

    if( !cell.isInterior() )  return result;

	int loc_target = cell.findVertex(vertexId);
	if(loc_target == bitpit::Vertex::NULL_ID) return result;

	livector1D list = tri->findCellVertexOneRing(cellId, loc_target);

	for(const auto & index : list){
		bitpit::ConstProxyVector<long> ids = tri->getCell(index).getVertexIds();
		result.insert(ids.begin(), ids.end());
	}
	result.erase(vertexId);
	return result;
}

/*!
 * Evaluate the centroid of the target cell.
 *\param[in] id of cell, no control if cell exists is done
 *\return cell centroid coordinate
 */
std::array<double,3> MimmoObject::evalCellCentroid(const long & id){
	std::array<double,3> result = {{0.0,0.0,0.0}};
	if(getPatch()){
		result = getPatch()->evalCellCentroid(id);
	}
	return result;
}

/*!
 * Evaluate the centroid of the target interface. Need to have interface built.
 *\param[in] id of interface, no control if interface exists is done
 *\return interface centroid coordinate
 */
std::array<double,3> MimmoObject::evalInterfaceCentroid(const long & id){
	std::array<double,3> result = {{0.0,0.0,0.0}};
	if(m_IntSync != SyncStatus::SYNC) return result;

	result = getPatch()->evalInterfaceCentroid(id);

	return result;
}

/*!
 * Evaluate the normal of the target interface, in the sense of owner-neighbour cell. Need to have interface built.
 * The method return zero normal for Point Cloud, 3D-Curve and Undefined mimmoObject meshes.
 * If volume mesh return the normal to the face in common between owner and neighbour cells and directed from the owner to the neighbor.
 * if surface mesh return the normal to the edge in common between owner and neighbour cells and directed from the owner to the neighbor.
 *
 *\param[in] id of interface,no control if interface exists is done
 *\return normal to interface
 */
std::array<double,3> MimmoObject::evalInterfaceNormal(const long & id){
	std::array<double,3> result ({0.0,0.0,0.0});
	if(m_IntSync != SyncStatus::SYNC)  return result;

	switch(m_type){
	case 1:
	{
		bitpit::Interface & interf = getInterfaces().at(id);
		std::array<double,3> enormal = static_cast<bitpit::SurfaceKernel*>(getPatch())->evalEdgeNormal(interf.getOwner(), interf.getOwnerFace());
		bitpit::ConstProxyVector<long> vv = interf.getVertexIds();
		result = crossProduct(getVertexCoords(vv[1]) - getVertexCoords(vv[0]), enormal);
		double normres = norm2(result);
		if(normres > std::numeric_limits<double>::min()){
			result /= normres;
		}
	}
	break;

	case 2:
		result = static_cast<bitpit::VolUnstructured*>(getPatch())->evalInterfaceNormal(id);
		break;

	default:
		//do nothing
		break;
	}
	return result;
}

/*!
 * Evaluate the area/lenght of the target interface. Need to have interface built.
 * The method return zero area for Point Cloud, 3D-Curve and Undefined mimmoObject meshes.
 * If volume mesh return the area of the the 2D Element face.
 * if surface mesh return the lenght of the edge in common between owner and neighbour cells.
 *
 *\param[in] id of interface,no control if interface exists is done
 *\return value of interface area/length.
 */
double MimmoObject::evalInterfaceArea(const long & id){
	double result (0.0);
	if(m_IntSync != SyncStatus::SYNC)  return result;

	switch(m_type){
	case 1:
	{
		bitpit::Interface & interf = getInterfaces().at(id);
		result = static_cast<bitpit::SurfaceKernel*>(getPatch())->evalEdgeLength(interf.getOwner(), interf.getOwnerFace());
	}
	break;

	case 2:
		result = static_cast<bitpit::VolUnstructured*>(getPatch())->evalInterfaceArea(id);
		break;

	default:
		//do nothing
		break;
	}
	return result;
}

/*!
  Build the Node-Node connectivity of the tessellated mesh,(nodes connected by edges)
  and store it internally.
 */
void
MimmoObject::buildPointConnectivity()
{
	cleanPointConnectivity();

	//No point connectivity for point cloud
	if(getType() == 3) return;

    m_pointConnectivity.reserve(getNVertices());

    // Surface/volume mesh & 3d curve point connectivity
	if(getType() == 1 || getType() == 2 || getType() == 4){

		// Edge connectivity
	    // All cells are considered, both interiors and ghosts
		std::set<std::pair<long,long> > edges;
		for (bitpit::Cell & cell : getCells()){
			int ne = 0;
			if (m_type == 1)
				ne = cell.getFaceCount();
			if (m_type == 2)
				ne = cell.getEdgeCount();
			if (m_type == 4)
				ne = 1;

			for (int i=0; i<ne; i++){
				bitpit::ConstProxyVector<long> ids;
				if (m_type == 1)
					ids = cell.getFaceVertexIds(i);
				if (m_type == 2)
					ids = cell.getEdgeVertexIds(i);
				if (m_type == 4)
					ids = cell.getVertexIds();

				// Edges have always two nodes
				long id1 = ids[0];
				long id2 = ids[1];
				std::pair<long,long> item;
				if (id1<id2)
					item=std::pair<long,long>(id1,id2);
				else
					item = std::pair<long,long>(id2,id1);
				if (!edges.count(item)){
					edges.insert(item);
					m_pointConnectivity[id1].insert(id2);
					m_pointConnectivity[id2].insert(id1);
				}
			}
		}
	}
	else{
		//No allowed type
		return;
	}

	m_pointConnectivitySync = SyncStatus::SYNC;
}

/*!
    Clean the point connectivity structure.
 */
void
MimmoObject::cleanPointConnectivity()
{
	std::unordered_map<long, std::unordered_set<long> >().swap(m_pointConnectivity);

    if(m_pointConnectivitySync == SyncStatus::SYNC){
        m_pointConnectivitySync = SyncStatus::UNSYNC;
    };
}

/*!
    Get the connectivity of a target node/vertex
    \param[in] id of target node
    \return connectivity nodes list of id-target.
 */
std::unordered_set<long> &
MimmoObject::getPointConnectivity(const long & id)
{
	assert(m_pointConnectivity.count(id) > 0 && "MimmoObject::not valid id in getPointConnectivity call");
	return m_pointConnectivity[id];
}

/*!
    \return the node-node connectivity sync status.
 */
SyncStatus
MimmoObject::getPointConnectivitySyncStatus(){
	return m_pointConnectivitySync;
}

/*!
 * Triangulate the linked geometry. It works only for surface geometries (type = 1).
 * After the method call the geometry (internal or linked) is forever modified.
 */
void
MimmoObject::triangulate(){

	if (m_type != 1){
		(*m_log)<<"Error MimmoObject: data structure type different from Surface in triangulate method"<<std::endl;
		throw std::runtime_error ("MimmoObject: data structure type different from Surface in triangulate method");
	}

	// In case of polygons (nVertices > 4) more nodes are added to the mesh.
	// In a distributed patch, new cells and nodes are added in all the partitions.
	// The connectivity order on two different partitions of the same cell (interior/ghost)
	// is supposed to be the same on each partition. This allows to add independently cells
	// on different partitions, even if working on the same physical cell.
	// The ghosts structures are updated at the end of the methods, by calling setPartitioned method.
	// Note. The node Ids on different partitions can be different and currently not tracked;
	// for this reason for now a serialization is forbidden after a refinement.

	// TODO INTRODUCE ADAPTING INFORMATION ON VERTICES AND CELLS WHEN USER INTERFACE IS READY IN BITPIT

	bitpit::PatchKernel * patch = getPatch();

	if (!isEmpty()){

	    long maxID, newID, newVertID;
	    const auto orderedCellID = getCells().getIds(true);
	    maxID = orderedCellID[(int)orderedCellID.size()-1];
	    newID = maxID+1;
	    {
	        const auto orderedVertID = getVertices().getIds(true);
	        newVertID = orderedVertID[(int)orderedVertID.size()-1] +1;
	    }

	    bitpit::ElementType eletype;
	    bitpit::ElementType eletri = bitpit::ElementType::TRIANGLE;
	    livector1D connTriangle(3);

	    for(const auto &idcell : orderedCellID){

	        livector1D conn = getCellConnectivity(idcell);
	        eletype = patch->getCell(idcell).getType();
	        long pid = patch->getCell(idcell).getPID();
	        int rank = -1;
#if MIMMO_ENABLE_MPI
	        rank = patch->getCellRank(idcell);
#endif
	        switch (eletype){
	        case bitpit::ElementType::QUAD:
	        case bitpit::ElementType::PIXEL:
	        {
	            patch->deleteCell(idcell);
	            for(std::size_t i=0; i<2; ++i){
	                connTriangle[0] = conn[0];
	                connTriangle[1] = conn[i+1];
	                connTriangle[2] = conn[i+2];
	                addConnectedCell(connTriangle, eletri, pid, newID, rank);
	                ++newID;
	            }
	        }
	        break;
	        case bitpit::ElementType::POLYGON:
	        {
	            std::size_t startIndex = 1;
	            std::size_t nnewTri = conn.size() - startIndex;
	            //calculate barycenter and add it as new vertex
	            darray3E barycenter = patch->evalCellCentroid(idcell);
	            addVertex(barycenter, newVertID);
	            //delete current polygon
	            patch->deleteCell(idcell);
	            //insert new triangles from polygon subdivision
	            for(std::size_t i=0; i<nnewTri; ++i){
	                connTriangle[0] = newVertID;
	                connTriangle[1] = conn[ startIndex + std::size_t( i % nnewTri) ];
	                connTriangle[2] = conn[ startIndex + std::size_t( (i+1) % nnewTri ) ];
	                addConnectedCell(connTriangle, eletri, pid, newID, rank);
	                ++newID;
	            }
	            //increment label of vertices
	            ++newVertID;

	        }
	        break;
	        case bitpit::ElementType::TRIANGLE:
	            //do nothing
	            break;
	        default:
	            throw std::runtime_error("unrecognized cell type in 3D surface mesh of CGNSPidExtractor");
	            break;
	        }
	    }

	    // Unsync structures and update geometry
	    m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
        m_pointConnectivitySync = std::min(m_pointConnectivitySync, SyncStatus::UNSYNC);
        m_skdTreeSync = std::min(m_skdTreeSync, SyncStatus::UNSYNC);
        m_kdTreeSync = std::min(m_kdTreeSync, SyncStatus::UNSYNC);
        m_AdjSync = std::min(m_AdjSync, SyncStatus::UNSYNC);
        m_IntSync = std::min(m_IntSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
        m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif
    } // end if patch is not empty

#if MIMMO_ENABLE_MPI
    deleteOrphanGhostCells();
#endif

    update();

}

/*!
 * Check the correctness elements of the geometry. Find degenerate elements in terms
  of vertices, i.e. an element is considered degenerate if at least two vertices
  are equal. If a degenerate element is found, it is degraded to a valid inferior
  element preserving the same cell id, e.g a degenerate QUAD is degraded to a TRIANGLE,
  if only one vertex is repeated.
  If a degraded element is invalid (e.g. a QUAD in a threedimensional mesh)
  the cell is deleted.
 * \param[out] degradedDeletedCells If not a null pointer the pointed vector is
               filled with the original deleted or degraded cells
 * \param[out] collapsedVertices If not a null pointer the pointed vector is filled
               with the original collapsed (i.e. deleted) vertices
 */
void
MimmoObject::degradeDegenerateElements(bitpit::PiercedVector<bitpit::Cell>* degradedDeletedCells,
                                       bitpit::PiercedVector<bitpit::Vertex>* collapsedVertices)
{
    std::vector<long> toDelete;
    std::vector<bitpit::Cell> toInsert;

    // Reserve a tenth of the entire mesh for cells to be insert/delete
    toDelete.reserve(getNCells()/10);
    toInsert.reserve(getNCells()/10);

    std::unordered_map<long,long> vertexMap;

    bool fillCells = false;
    if (degradedDeletedCells){
        fillCells = true;
        degradedDeletedCells->clear();
    }

    bool fillVertices = false;
    if (collapsedVertices){
        fillVertices = true;
        collapsedVertices->clear();
    }

    // Loop on all cells to find the cell to be degraded or deleted
    // In parallel all the processes do the same things on local/ghost shared cells
    // The cells are degraded in the same manner and the cells/vertices delted
    // are the same.
    // Note. The new cells IDs can be different on different processes (local Id != ghost Id)
    for (bitpit::Cell & cell : getCells()){

        long cellId = cell.getId();
        bitpit::ElementType cellType = cell.getType();
        bitpit::ConstProxyVector<long> cellVertexIds = cell.getVertexIds();
        std::vector<long> cellConnectivity;
        std::vector<bitpit::Vertex> cellVertices;
        cellConnectivity.reserve(cellVertexIds.size());
        cellVertices.reserve(cellVertexIds.size());
        // Unique vertices cell connectivity
        for (long vertexId : cellVertexIds){
            bitpit::Vertex & candidateVertex = getPatch()->getVertex(vertexId);
            std::vector<bitpit::Vertex>::iterator it = std::find(cellVertices.begin(), cellVertices.end(), candidateVertex);
            if (it == cellVertices.end()){
                cellConnectivity.push_back(vertexId);
                cellVertices.emplace_back(candidateVertex);
            }
            else{
                //Collapse vertices
                vertexMap.insert({vertexId, it->getId()});
                if (fillVertices){
                    collapsedVertices->insert(vertexId, candidateVertex);
                }
            }
        }

        bitpit::ElementType desumedType = desumeElement(cellConnectivity);

        if (desumedType != cellType){
            // If degenerate delete from geometry
            toDelete.push_back(cellId);
            if (desumedType != bitpit::ElementType::UNDEFINED){
                // If it can be degraded do it
                std::unique_ptr<long[]> connectStorage = std::unique_ptr<long[]>(new long[cellConnectivity.size()]);
                std::copy(cellConnectivity.begin(), cellConnectivity.end(), connectStorage.get());
                bitpit::Cell newCell(cellId, desumedType, std::move(connectStorage));
                newCell.setPID(cell.getPID());
                toInsert.push_back(newCell);
            }
            if (fillCells){
                degradedDeletedCells->insert(cellId, cell);
            }
        }

    } // end loop on cells

    // Delete degenerate cells
    getPatch()->deleteCells(toDelete);

    // Insert new degraded cells
    for (bitpit::Cell & cell : toInsert){
        addCell(cell, cell.getId());
    }

    // Collapse vertices
    if (!vertexMap.empty()) {
        // Renumber cell vertices
        for (bitpit::Cell &cell : getCells()) {
            cell.renumberVertices(vertexMap);
        }
    }

    // Unsync structures and update geometry if some cells deleted
    if (!toDelete.empty()){
        // Unsync structures and update geometry
        m_infoSync = std::min(m_infoSync, SyncStatus::UNSYNC);
        m_pointConnectivitySync = std::min(m_pointConnectivitySync, SyncStatus::UNSYNC);
        m_skdTreeSync = std::min(m_skdTreeSync, SyncStatus::UNSYNC);
        m_kdTreeSync = std::min(m_kdTreeSync, SyncStatus::UNSYNC);
        m_AdjSync = std::min(m_AdjSync, SyncStatus::UNSYNC);
        m_IntSync = std::min(m_IntSync, SyncStatus::UNSYNC);
#if MIMMO_ENABLE_MPI
        m_pointGhostExchangeInfoSync = std::min(m_pointGhostExchangeInfoSync, SyncStatus::UNSYNC);
#endif

    }

    update();

}

}
