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
#include "Operators.hpp"
#include "SkdTreeUtils.hpp"
#include <set>

using namespace std;
using namespace bitpit;

namespace mimmo{

/*!
 * MimmoSurfUnstructured default constructor
 */
MimmoSurfUnstructured::MimmoSurfUnstructured():bitpit::SurfUnstructured(){}

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] patch_dim dimensionality of elements (2D-triangles/quads/polygons, 1D-lines)
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(int patch_dim):
                    				   bitpit::SurfUnstructured(int(patch_dim),int(3)){}

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] id custom identification label of the mesh
 * \param[in] patch_dim dimensionality of elements (2D-triangles/quads/polygons, 1D-lines)
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(const int & id, int patch_dim):
                    				   bitpit::SurfUnstructured(id, int(patch_dim), int(3)){}

/*!
 * MimmoSurfUnstructured custom constructor
 * \param[in] stream input stream where reading from
 */
MimmoSurfUnstructured::MimmoSurfUnstructured(std::istream & stream):
                    				   bitpit::SurfUnstructured(stream){}


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
	return std::unique_ptr<MimmoSurfUnstructured>(new MimmoSurfUnstructured(*this));
}

/*!
 * MimmoVolUnstructured default constructor
 * \param[in] dimension dimensionality of elements (3D - tetrahedra/hexahedra ..., 2D-triangles/quads/polygons)
 */
MimmoVolUnstructured::MimmoVolUnstructured(const int& dimension):
				bitpit::VolUnstructured(dimension){}

/*!
 * MimmoVolUnstructured custom constructor
 * \param[in] id custom identification label of the mesh
 * \param[in] dimension dimensionality of elements (3D - tetrahedra/hexahedra ..., 2D-triangles/quads/polygons)
 */
MimmoVolUnstructured::MimmoVolUnstructured(const int & id, const int & dimension):
				bitpit::VolUnstructured(id, dimension){}

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
	return std::unique_ptr<MimmoVolUnstructured>(new MimmoVolUnstructured(*this));
}

/*!
 * MimmoPointCloud basic constructor
 */
MimmoPointCloud::MimmoPointCloud():
				bitpit::SurfUnstructured(int(2),int(3)){}

/*!
 * MimmoPointCloud custom constructor
 * \param[in] id custom identification label of the mesh
 */
MimmoPointCloud::MimmoPointCloud(const int & id):
				bitpit::SurfUnstructured(id, int(2), int(3)){}

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
	return std::unique_ptr<MimmoPointCloud>(new MimmoPointCloud(*this));
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
 */
MimmoObject::MimmoObject(int type){

	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);
	m_type = max(type,1);
	if (m_type > 4){
		(*m_log)<<"Error MimmoObject: unrecognized data structure type in class construction. Switch to DEFAULT 1-Surface"<<std::endl;
		throw std::runtime_error ("MimmoObject : unrecognized mesh type in class construction");
	}
	switch(m_type){
	case 1:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(2)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoVolUnstructured(3)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoPointCloud()));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(1)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

	m_internalPatch = true;
	m_extpatch = NULL;

	m_skdTreeSupported = (m_type != 3);
	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_AdjBuilt = false;
	m_IntBuilt = false;
#if MIMMO_ENABLE_MPI
	initializeParallel();
#endif
	m_patchInfo.setPatch(m_patch.get());
	m_patchInfo.update();
	m_infoSync = true;
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
 * Please note, despite type argument, if null connectivity structure is provided, MimmoObject will be built
 * as a standard cloud point of vertices provided by vertex.
 * Connectivity for each standard cell is simply defined as a list of vertex indices which compose it
 * (indices are meant as positions in the provided vertex list argument).
 * Polygonal and Polyhedral cells requires a special convention to define their connectivity:
 * - polygons: require on top of the list the total number of vertices which compose it,
 *             followed by the indices in the vertex list.(ex. polygon with 5 vertices: 5 i1 i2 i3 i4 i5)
 * - polyhedra: require on top the total number of faces, followed by the number of vertices which composes the local face + the vertex
 *   indices which compose the faces, and so on for all the faces which compose the polyhedron (ex. polyhedron with 3 faces,
 *   first face is a triangle, second is a quad etc...  3 3 i1 i2 i3 4 i2 i3 i4 i5 ...)
 * \param[in] type type of meshes.
 * \param[in] vertex Coordinates of geometry vertices.
 * \param[in] connectivity pointer to mesh connectivity list (optional).
 */
MimmoObject::MimmoObject(int type, dvecarr3E & vertex, livector2D * connectivity){

	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);

	if(connectivity == NULL){
		m_type = 3;
	}else{
		m_type = max(type,1);
		if (m_type > 4){
			(*m_log)<<"Error MimmoObject: unrecognized data structure type in class construction. Switch to DEFAULT 1-Surface"<<std::endl;
			throw std::runtime_error ("MimmoObject : unrecognized mesh type in class construction");
		}
	}

	switch(m_type){
	case 1:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(2)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoVolUnstructured(3)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoPointCloud()));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(1)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

	m_internalPatch = true;
	m_extpatch = NULL;

	m_skdTreeSupported = (m_type != 3);
	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_AdjBuilt = false;
	m_IntBuilt = false;

	bitpit::ElementType eltype;
	std::size_t sizeVert = vertex.size();

	m_patch->reserveVertices(sizeVert);

	for(const auto & vv : vertex)   addVertex(vv);

	m_log->setPriority(bitpit::log::Priority::DEBUG);

	if(m_type != 3){
		livector1D temp;
		std::size_t sizeCell = connectivity->size();
		m_patch->reserveCells(sizeCell);
		for(auto const & cc : *connectivity){
			eltype = desumeElement(cc);
			if(eltype != bitpit::ElementType::UNDEFINED){
				addConnectedCell(cc, eltype);
			}else{
				(*m_log)<<"warning: in MimmoObject custom constructor. Undefined cell type detected and skipped."<<std::endl;
			}
		}

		m_pidsType.insert(0);
		m_pidsTypeWNames.insert(std::make_pair(0,""));

	}else{
		(*m_log)<<"Not supported connectivity found for MimmoObject"<<std::endl;
		(*m_log)<<"Proceeding as Point Cloud geometry"<<std::endl;
	}
	m_log->setPriority(bitpit::log::Priority::NORMAL);

#if MIMMO_ENABLE_MPI
	initializeParallel();
#endif
	m_patchInfo.setPatch(m_patch.get());
	m_patchInfo.update();
	m_infoSync = true;
};

/*!
 * Custom constructor of MimmoObject.
 * This constructor builds a geometry data structure soft linking to an external bitpit::PatchKernel object, that is
 * it does not own the geometry data structure, but simple access it, while it is instantiated elsewhere.
 * If a null or empty or not coherent geometry patch is linked, throw an error exception.
 * \param[in] type type of mesh
 * \param[in] geometry pointer to a geometry of class PatchKernel to be linked.
 */
MimmoObject::MimmoObject(int type, bitpit::PatchKernel* geometry){

	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);
	m_type = max(type,1);
	if (m_type > 4 || geometry == NULL){
		(*m_log)<<"Error MimmoObject: unrecognized data structure type or NULL argument in class construction."<<std::endl;
		throw std::runtime_error ("MimmoObject : unrecognized mesh type or NULL argument in class construction");
	}
	if (geometry->getVertexCount() ==0){
		(*m_log)<<"Error MimmoObject: no points detected in the linked mesh."<<std::endl;
		throw std::runtime_error ("MimmoObject : no points detected in the linked mesh.");
	}
	if (geometry->getCellCount() ==0 && m_type != 3){
		(*m_log)<<"Error MimmoObject: no connectivity detected in the linked mesh."<<std::endl;
		throw std::runtime_error ("MimmoObject : no connectivity detected in the linked mesh.");
	}

	m_internalPatch = false;
	m_extpatch = geometry;

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
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(geometry))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
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
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(geometry))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
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
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(geometry))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

	m_skdTreeSupported = (m_type != 3);
	m_skdTreeSync = false;
	m_kdTreeSync = false;

	//check if adjacencies and interfaces are built.
	{
		long free = 0, facesCount=0;
		for(const auto & cell : getPatch()->getCells()){
			const long * adj = cell.getAdjacencies();
			for(int i=0; i<cell.getAdjacencyCount(); ++i){
				free += long(adj[i] == bitpit::Cell::NULL_ID);
				facesCount++;
			}
		}
		m_AdjBuilt = (free < facesCount);
	}

	m_IntBuilt = (getPatch()->getInterfaces().size() > 0);

	//recover cell PID
	for(const auto &cell : getPatch()->getCells()){
		auto PID = cell.getPID();
		m_pidsType.insert((long)PID);
		m_pidsTypeWNames.insert(std::make_pair( (long)PID , "") );
	}

#if MIMMO_ENABLE_MPI
	initializeParallel();
#endif
	m_patchInfo.setPatch(m_extpatch);
	m_patchInfo.update();
	m_infoSync = true;
}

/*!
 * Custom constructor of MimmoObject.
 * This constructor builds a geometry data structure owning an external bitpit::PatchKernel object, that is
 * it takes the ownership of the geometry data structure treting it as an internal structure, and it will
 * be responsible of its own destruction.
 * If a null or empty or not coherent geometry patch is linked, throw an error exception.
 * \param[in] type type of mesh
 * \param[in] geometry unique pointer to a geometry of class PatchKernel to be linked.
 */
MimmoObject::MimmoObject(int type, std::unique_ptr<bitpit::PatchKernel> & geometry){

	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);
	m_type = max(type,1);
	if (m_type > 4 || !geometry){
		(*m_log)<<"Error MimmoObject: unrecognized data structure type or NULL argument in class construction."<<std::endl;
		throw std::runtime_error ("MimmoObject : unrecognized mesh type or NULL argument in class construction");
	}
	if (geometry->getVertexCount() ==0){
		(*m_log)<<"Error MimmoObject: no points detected in the linked mesh."<<std::endl;
		throw std::runtime_error ("MimmoObject : no points detected in the linked mesh.");
	}
	if (geometry->getCellCount() ==0 && m_type != 3){
		(*m_log)<<"Error MimmoObject: no connectivity detected in the linked mesh."<<std::endl;
		throw std::runtime_error ("MimmoObject : no connectivity detected in the linked mesh.");
	}

	//check among elements if they are coherent with the type currently hold by the linked mesh.
	std::unordered_set<int> mapEle = elementsMap(*(geometry.get()));
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
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(geometry.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
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
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(geometry.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
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
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(geometry.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

	m_internalPatch = true;
	m_extpatch = NULL;
	m_patch = std::move(geometry);

	m_skdTreeSupported = (m_type != 3);
	m_skdTreeSync = false;
	m_kdTreeSync = false;

	//check if adjacencies and interfaces are built.
	{
		long free = 0, facesCount=0;
		for(const auto & cell : getPatch()->getCells()){
			const long * adj = cell.getAdjacencies();
			for(int i=0; i<cell.getAdjacencyCount(); ++i){
				free += long(adj[i] == bitpit::Cell::NULL_ID);
				facesCount++;
			}
		}
		m_AdjBuilt = (free < facesCount);
	}

	m_IntBuilt = (getPatch()->getInterfaces().size() > 0);

	//recover cell PID
	for(const auto &cell : getPatch()->getCells()){
		auto PID = cell.getPID();
		m_pidsType.insert((long)PID);
		m_pidsTypeWNames.insert(std::make_pair( (long)PID , "") );
	}

#if MIMMO_ENABLE_MPI
	initializeParallel();
#endif
	m_patchInfo.setPatch(m_patch.get());
	m_patchInfo.update();
	m_infoSync = true;
}

/*!
 * Default destructor
 */
MimmoObject::~MimmoObject(){

};

/*!
 * Copy constructor of MimmoObject.
 * Internal allocated PatchKernel is copied from the argument object as a soft link
 * (copied by its pointer only, geometry data structure is treated as external in the new copied class).
 * Search trees are instantiated, but their containts (if any) are not copied by default.
 */
MimmoObject::MimmoObject(const MimmoObject & other){

	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);

	m_extpatch = other.m_extpatch;
	if(other.m_internalPatch){
		m_extpatch      = other.m_patch.get();
	}
	m_internalPatch = false;

	m_type              = other.m_type;
	m_pidsType          = other.m_pidsType;
	m_pidsTypeWNames    = other.m_pidsTypeWNames;
	m_skdTreeSupported  = other.m_skdTreeSupported;
	m_AdjBuilt          = other.m_AdjBuilt;
	m_IntBuilt          = other.m_IntBuilt;
	m_infoSync 			= other.m_infoSync;

	m_skdTreeSync    = false;
	m_kdTreeSync    = false;

	//instantiate empty trees:
	switch(m_type){
	case 1:
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_extpatch))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(m_extpatch))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_extpatch))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

#if MIMMO_ENABLE_MPI
	m_rank = other.m_rank;
	m_nprocs = other.m_nprocs;
	m_communicator = other.m_communicator;
	m_nglobalvertices = other.m_nglobalvertices;
	m_pointGhostExchangeInfoSync = false;
#endif
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
	std::swap(m_skdTreeSupported, x.m_skdTreeSupported);
	std::swap(m_AdjBuilt, x.m_AdjBuilt);
	std::swap(m_IntBuilt, x.m_IntBuilt);
	std::swap(m_skdTree, x.m_skdTree);
	std::swap(m_kdTree, x.m_kdTree);
	std::swap(m_skdTreeSync, x.m_skdTreeSync);
	std::swap(m_kdTreeSync, x.m_kdTreeSync);
	std::swap(m_infoSync, x.m_infoSync);
#if MIMMO_ENABLE_MPI
	std::swap(m_communicator, x.m_communicator);
	std::swap(m_rank, x.m_rank);
	std::swap(m_nprocs, x.m_nprocs);
	std::swap(m_nglobalvertices, x.m_nglobalvertices);
	m_pointGhostExchangeInfoSync = false;
#endif
}

#if MIMMO_ENABLE_MPI
/*!
 * Initialize MPI structures.
 */
void
MimmoObject::initializeParallel(){

	int initialized;
	MPI_Initialized(&initialized);
	if (!initialized)
		MPI_Init(NULL, NULL);

	//Recover or fix communicator
	if (m_internalPatch){
		if (m_patch->isCommunicatorSet()){
			m_communicator = m_patch->getCommunicator();
		}
		else{
			m_communicator = MPI_COMM_WORLD;
			m_patch->setCommunicator(m_communicator);
		}
	}
	else{
		if (m_extpatch->isCommunicatorSet()){
			m_communicator = m_extpatch->getCommunicator();
		}
		else{
			m_communicator = MPI_COMM_WORLD;
			m_extpatch->setCommunicator(m_communicator);
		}
	}

	MPI_Comm_rank(m_communicator, &m_rank);
	MPI_Comm_size(m_communicator, &m_nprocs);

	m_pointGhostExchangeInfoSync = false;

}
#endif

/*!
 * Is the object empty?
 * \return True/False if geometry data structure is empty.
 */
bool
MimmoObject::isEmpty(){
	bool check = getNVertices() > 0;
	if(m_type != 3) check = check && (getNCells() > 0);
	return !check;
};

/*!
 * Is the object empty? const overloading.
 * \return True/False if geometry data structure is empty.
 */
bool
MimmoObject::isEmpty() const{
	bool check = getNVertices() > 0;
	if(m_type != 3) check = check && (getNCells() > 0);
	return !check;
};


/*!
 * Is the skdTree (former bvTree) ordering supported w/ the current geometry?
 * \return True for connectivity-based meshes, false por point clouds
 */
bool
MimmoObject::isBvTreeSupported(){
	return isSkdTreeSupported();
};

/*!
 * Is the skdTree (former bvTree) ordering supported w/ the current geometry?
 * \return True for connectivity-based meshes, false por point clouds
 */
bool
MimmoObject::isSkdTreeSupported(){
	return m_skdTreeSupported;
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
 * Return the total number of local vertices within the data structure. Ghost cells are considered in the count.
 * \return number of mesh vertices
 */
long
MimmoObject::getNVertices() const {
	const auto p = getPatch();
	return p->getVertexCount();
};

/*!
 * Return the total number of local cells within the data structure. Ghost cells are considered in the count.
 * \return number of mesh cells.
 */
long
MimmoObject::getNCells() const {
	const auto p = getPatch();
	return p->getCellCount();
};

/*!
 * Return the total number of local internal cells within the data structure. Ghost cells are not considered in the count.
 * \return number of mesh cells.
 */
long
MimmoObject::getNInternals() const {
	const auto p = getPatch();
	return p->getInternalCount();
};

#if MIMMO_ENABLE_MPI
/*!
 * Return the total number of global vertices within the data structure.
 * \return number of mesh vertices
 */
long
MimmoObject::getNGlobalVertices(){
	if (!getPatch()->isPartitioned())
		return getNVertices();

	if (!(m_nglobalvertices == 0)){
		return m_nglobalvertices;
	}

	//Count the global number of vertices

	//Recover the information of ghost exchange source cells
	std::unordered_map<int, std::vector<long> > ranksources = getPatch()->getGhostExchangeSources();
	std::unordered_map<int, std::vector<long> > ranktargets = getPatch()->getGhostExchangeTargets();

	std::unordered_map<int, std::set<long> > rankst;
	for (auto & pair : ranksources){
		for (long id : pair.second)
			rankst[pair.first].insert(id);
	}
	for (auto & pair : ranktargets){
		for (long id : pair.second)
			rankst[pair.first].insert(id);
	}

	//Count occurences for each source to other processors
	std::unordered_map<long, int> stoccurences;
	for (auto pair : rankst){
		for (long source : pair.second){
			if (stoccurences.count(source)){
				stoccurences[source] = stoccurences[source] + 1;
			}
			else{
				stoccurences[source] = 2;
			}
		}
	}

	//Initialize occurences for each common vertex
	std::unordered_map<long, int> vertexoccurences;
	for (auto & pair : stoccurences){
		long idcell = pair.first;
		const bitpit::Cell & cell = getPatch()->getCell(idcell);
		for (long idvertex : cell.getVertexIds()){
			if (vertexoccurences.count(idvertex)){
				vertexoccurences[idvertex] = std::max(vertexoccurences[idvertex], pair.second);
			}
			else{
				vertexoccurences[idvertex] = pair.second;
			}
		}
	}

	//Count local vertices
	//Use double to weight the contribution of multiple occurences due to the vertices shared by processors
	double localVerticesCount = 0.;
	std::set<long> counted;
	for (const bitpit::Cell & cell : getPatch()->getCells()){
		for (long id : cell.getVertexIds()){
			if (!counted.count(id)){
				counted.insert(id);
				double weight = 1.;
				if (vertexoccurences.count(id)){
					weight /= (double(vertexoccurences[id]));
				}
				localVerticesCount += 1. * weight;
			}
		}
	}

	double globalVerticesCount = 0.;

	MPI_Allreduce(&localVerticesCount, &globalVerticesCount, 1, MPI_DOUBLE, MPI_SUM, m_communicator);

	return long(round(globalVerticesCount));

};

/*!
 * Return the total number of global cells within the data structure.
 * \return number of mesh cells.
 */
long
MimmoObject::getNGlobalCells() {
	if (!getPatch()->isPartitioned())
		return getNCells();

	PatchNumberingInfo info(getPatch());
	return info.getCellGlobalCount();
};
#endif

/*!
 * Return the compact list of local vertices hold by the class. Ghost cells are considered to fill the structure.
 * \return coordinates of mesh vertices
 * \param[out] mapDataInv pointer to inverse of Map of vertex ids, for aligning external vertex data to bitpit::Patch ordering.
 */
dvecarr3E
MimmoObject::getVerticesCoords(liimap* mapDataInv){
	dvecarr3E result(getNVertices());
	int  i = 0;

	auto pvert = getVertices();

	if (mapDataInv != NULL){
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
 * If i is not found, return a [1.e18,1.e18,1.e18] default value.
 * \param[in] i bitpit::PatchKernel id of the vertex in the mesh.
 * \return Coordinates of the i-th vertex of geometry mesh.
 */
darray3E
MimmoObject::getVertexCoords(long i) const
{
	if(!(getVertices().exists(i)))	return darray3E({{1.e18,1.e18,1.e18}});
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
MimmoObject::getCompactConnectivity(liimap & mapDataInv){

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
MimmoObject::getCellConnectivity(long i){
	if (!(getCells().exists(i)))    return livector1D(0);

	bitpit::Cell & cell = getPatch()->getCell(i);
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
 * bitpit::PatchKernel unique-labeled indexing. Ghost cells are considered.
 * \return id of cells
 */
livector1D
MimmoObject::getCellsIds(){
	return getPatch()->getCells().getIds();
};

/*!
 * \return pointer to bitpit::PatchKernel structure hold by the class.
 */
PatchKernel*
MimmoObject::getPatch(){
	if(!m_internalPatch) return m_extpatch;
	else return m_patch.get();
};

/*!
 * \return pointer to bitpit::PatchKernel structure hold by the class.
 */
const PatchKernel*
MimmoObject::getPatch() const{
	if(!m_internalPatch) return m_extpatch;
	else return m_patch.get();
};

/*!
 * Return the local indexing vertex map, to pass from local, compact indexing
 * to bitpit::PatchKernel unique-labeled indexing.
 * \return local/unique-id map
 */
livector1D
MimmoObject::getMapData(){
	livector1D mapData(getNVertices());
	int i = 0;
	for (auto const & vertex : getVertices()){
		mapData[i] = vertex.getId();
		++i;
	}
	return mapData;
};

/*!
 * Return the local indexing vertex map, to pass from bitpit::PatchKernel unique-labeled indexing
 * to local, compact indexing.
 * \return unique-id/local map
 */
liimap
MimmoObject::getMapDataInv(){
	liimap mapDataInv;
	int i = 0;
	for (auto const & vertex : getVertices()){
		mapDataInv[vertex.getId()] = i;
		++i;
	}
	return mapDataInv;
};

/*!
 * Return the map of consecutive global indexing of the local cells; to pass from consecutive global index of a local cell
 * to bitpit::PatchKernel unique-labeled indexing. Ghost cells are considered.
 * \return global consecutive / local unique id map
 */
liimap
MimmoObject::getMapCell(){
	liimap mapCell;
	if (!isInfoSync()) buildPatchInfo();
	for (auto const & cell : getCells()){
		long id = cell.getId();
		long i = getPatchInfo()->getCellConsecutiveId(id);
		mapCell[i] = id;
	}
	return mapCell;
};

/*!
 * Return the map of consecutive global indexing of the local cells; to pass from bitpit::PatchKernel unique-labeled indexing of a local cell
 * to consecutive global index. Ghost cells are considered.
 * \return local unique id / global consecutive map
 */
liimap
MimmoObject::getMapCellInv(){
	if (!isInfoSync()) buildPatchInfo();
	liimap mapCellInv = getPatchInfo()->getCellConsecutiveMap();
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
 * \return the list of PID associated to each cell of tessellation in compact
 * sequential ordering. Ghost cells are considered.
 * If empty list is returned, pidding is not supported for this geometry.
 */
livector1D
MimmoObject::getCompactPID() {
	if(!m_skdTreeSupported || m_pidsType.empty())	return livector1D(0);
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
	if(!m_skdTreeSupported || m_pidsType.empty())	return std::unordered_map<long,long>();
	std::unordered_map<long,long> 	result;
	for(auto const & cell : getCells()){
		result[cell.getId()] = (long) cell.getPID();
	}
	return(result);
};

/*!
 * \return true if the skdTree ordering structure for cells is built/synchronized
 * with your current geometry
 */
bool
MimmoObject::isBvTreeBuilt(){
	return isSkdTreeSync();
}

/*!
 * \return true if the skdTree ordering structure for cells is built/synchronized
 * with your current geometry
 */
bool
MimmoObject::isBvTreeSync(){
	return isSkdTreeSync();
}

/*!
 * \return true if the skdTree ordering structure for cells is built/synchronized
 * with your current geometry.
 */
bool
MimmoObject::isSkdTreeSync(){
	return m_skdTreeSync;
}

/*!
 * \return true if the patch numbering info structure for cells is built/synchronized
 * with your current geometry
 */
bool
MimmoObject::isInfoSync(){
	return m_infoSync;
}

/*!
 * \return pointer to geometry skdTree search structure
 */
bitpit::PatchSkdTree*
MimmoObject::getBvTree(){
	return getSkdTree();
}

/*!
 * \return pointer to geometry skdTree search structure
 */
bitpit::PatchSkdTree*
MimmoObject::getSkdTree(){
	if (!m_skdTreeSupported) return NULL;
	if (!isSkdTreeSync()) buildSkdTree();
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
 * \return true if the kdTree vertices ordering structure is
 * built/synchronized with your current geometry
 */
bool
MimmoObject::isKdTreeBuilt(){
	return isKdTreeSync();
}

/*!
 * \return true if the kdTree vertices ordering structure is
 * built/synchronized with your current geometry
 */
bool
MimmoObject::isKdTreeSync(){
	return m_kdTreeSync;
}

/*!
 * \return pointer to geometry KdTree internal structure
 */
bitpit::KdTree<3, bitpit::Vertex, long >*
MimmoObject::getKdTree(){
	if (!m_kdTreeSync) buildKdTree();
	return m_kdTree.get();
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
	Gets the MPI rank associated to the object

	\return The MPI rank associated to the object.
*/
int MimmoObject::getRank() const
{
	return m_rank;
}

/*!
	Gets the MPI processors in the communicator associated to the object

	\return The MPI processors in the communicator associated to the object
*/
int MimmoObject::getProcessorCount() const
{
	return m_nprocs;
}

/*!
	Gets a constant reference to the ghost targets needed for point data exchange.

	\result A constant reference to the ghost targets needed for point data
	exchange.
*/
const std::unordered_map<int, std::vector<long>> & MimmoObject::getPointGhostExchangeTargets() const
{
	return m_pointGhostExchangeTargets;
}

/*!
	Gets a constant reference to the ghost sources needed for point data exchange.

	\result A constant reference to the ghost sources needed for point data
	exchange.
*/
const std::unordered_map<int, std::vector<long>> & MimmoObject::getPointGhostExchangeSources() const
{
	return m_pointGhostExchangeSources;
}


/*!
	Checks if the ghost exchange information are dirty.

	\result Returns true if the ghost exchange information on points are dirty, false
	otherwise.
*/
bool MimmoObject::arePointGhostExchangeInfoSync() const
{
	return m_pointGhostExchangeInfoSync;
}

/*!
	Update the information needed for ghost point data exchange.
*/
void MimmoObject::updatePointGhostExchangeInfo()
{

	// Clear targets
	m_pointGhostExchangeTargets.clear();

	//Fill the nodes of the targets
	for (const auto &entry : m_patch->getGhostExchangeTargets()) {
		int ghostRank = entry.first;
		std::vector<long> ghostIds = entry.second;
		std::unordered_set<long> ghostVertices;
		for (long cellId : ghostIds){
			for (long vertexId : m_patch->getCell(cellId).getVertexIds()){
				ghostVertices.insert(vertexId);
			}
		}
		m_pointGhostExchangeTargets[ghostRank].assign(ghostVertices.begin(), ghostVertices.end());
	}

	// Clear sources
	m_pointGhostExchangeSources.clear();

	//Fill the nodes of the sources
	for (const auto &entry : m_patch->getGhostExchangeSources()) {
		int recvRank = entry.first;
		std::vector<long> localIds = entry.second;
		std::unordered_set<long> localVertices;
		for (long cellId : localIds){
			for (long vertexId : m_patch->getCell(cellId).getVertexIds()){
				localVertices.insert(vertexId);
			}
		}
		m_pointGhostExchangeSources[recvRank].assign(localVertices.begin(), localVertices.end());
	}

	//Erase common vertices
	for (auto & sourceEntry : m_pointGhostExchangeSources){
		int recvRank = sourceEntry.first;
		if (m_pointGhostExchangeTargets.count(recvRank)){
			std::size_t sourceLast = sourceEntry.second.size()-1;
			auto & sourceVertices = sourceEntry.second;
			std::size_t targetLast = m_pointGhostExchangeTargets[recvRank].size()-1;
			auto & targetVertices = m_pointGhostExchangeTargets[recvRank];
			std::vector<long>::iterator itsourceend = sourceVertices.end();
			for (std::vector<long>::iterator itsource=sourceVertices.begin(); itsource!=itsourceend; itsource++){
				auto iter = std::find(targetVertices.begin(), targetVertices.end(), *itsource);
				if (iter != targetVertices.end()){
					//The nodes are not repeated so the target veritces don't need to be resized
					*iter = targetVertices[targetLast];
					targetLast--;
					*itsource = *(sourceVertices.end()-1);
					sourceLast--;
					sourceVertices.resize(sourceLast+1);
					itsourceend = sourceVertices.end();
					itsource--;
				}
			}
			targetVertices.resize(targetLast+1);
		}
	}

	// Sort the targets
	for (auto &entry : m_pointGhostExchangeTargets) {
		std::vector<long> &rankTargets = entry.second;
		std::sort(rankTargets.begin(), rankTargets.end(), VertexPositionLess(*this));
	}

	// Sort the sources
	for (auto &entry : m_pointGhostExchangeSources) {
		std::vector<long> &rankSources = entry.second;
		std::sort(rankSources.begin(), rankSources.end(), VertexPositionLess(*this));
	}

	// Exchange info are now updated
	m_pointGhostExchangeInfoSync = true;
}

#endif

/*!
 * Set the vertices structure of the class, clearing any previous vertex list stored.
 * Be careful: any connectivity information stored in an existent cell list will be erased too.
 * The cell list will survive, but carrying no connectivity information.
 * \param[in] vertices geometry vertex structure .
 * \return false if no geometry is linked, not all vertices inserted or empty argument.
 */
bool
MimmoObject::setVertices(const bitpit::PiercedVector<bitpit::Vertex> & vertices){

	if (vertices.empty()) return false;

	getPatch()->resetVertices();

	int sizeVert = vertices.size();
	getPatch()->reserveVertices(sizeVert);

	long id;
	darray3E coords;
	bool checkTot = true;
	for (auto && val : vertices){
		id = val.getId();
		coords = val.getCoords();
		checkTot = checkTot && addVertex(coords, id);
	}
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
	return checkTot;
};

/*!
 *It adds one vertex to the mesh.
 * If unique-id is specified for the vertex, assign it, otherwise provide itself
 * to get a unique-id for the added vertex. The latter option is the default.
 * If the unique-id is already assigned, return with unsuccessful insertion.
 * Vertices of ghost cells have to be considered.
 *
 * \param[in] vertex vertex coordinates to be added
 * \param[in] idtag  unique id associated to the vertex
 * \return true if the vertex is successful inserted.
 */
bool
MimmoObject::addVertex(const darray3E & vertex, const long idtag){

	if(idtag != bitpit::Vertex::NULL_ID && getVertices().exists(idtag))    return false;

	bitpit::PatchKernel::VertexIterator it;
	auto patch = getPatch();
	if(idtag == bitpit::Vertex::NULL_ID){
		it = patch->addVertex(vertex);
	}else{
		it = patch->addVertex(vertex, idtag);
	}

	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
	return true;
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
 * \return true if the vertex is successful inserted.
 */
bool
MimmoObject::addVertex(const bitpit::Vertex & vertex, const long idtag){

	if(idtag != bitpit::Vertex::NULL_ID && getVertices().exists(idtag))    return false;

	bitpit::PatchKernel::VertexIterator it;
	auto patch = getPatch();
	if(idtag == bitpit::Vertex::NULL_ID){
		it = patch->addVertex(vertex);
	}else{
		it = patch->addVertex(vertex, idtag);
	}

	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
	return true;
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
	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_infoSync = false;
	return true;
};

/*!
 * Sets the cell structure of the geometry, clearing any previous cell list stored.
 * Does not do anything if class type is a point cloud (mesh type 3).
 * Previous PID and name associated will be wiped out and replaced.
 * Ghost cells have to be considered.
 *
 * \param[in] cells cell structure of geometry mesh.
 * \return false if no geometry is linked, not all cells are inserted or empty argument.
 */
bool
MimmoObject::setCells(const bitpit::PiercedVector<Cell> & cells){

	if (cells.empty() || !m_skdTreeSupported) return false;

	m_pidsType.clear();
	m_pidsTypeWNames.clear();
	getPatch()->resetCells();

	int sizeCell = cells.size();
	getPatch()->reserveCells(sizeCell);

	long idc;
	int  nSize;
	long pid;
	bitpit::ElementType eltype;
	bool checkTot = true;
	livector1D connectivity;

	for (const auto & cell : cells){
		// get ID
		idc = cell.getId();
		//check on element type
		eltype = cell.getType();
		//check info PID
		pid = (long)cell.getPID();
		nSize = cell.getConnectSize();
		connectivity.resize(nSize);
		auto const conn = cell.getConnect();
		for(int i=0; i<nSize; ++i){
			connectivity[i] = conn[i];
		}
		checkTot = checkTot && addConnectedCell(connectivity, eltype, pid, idc);
	}

#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif

	return checkTot;
};

/*!
 * It adds one cell with its vertex-connectivity (vertex in bitpit::PatchKernel unique id's), the type of cell to
 * be added and its own unique id. If no id is specified, teh method assigns it automatically.
 * Any kind of cell in bitpit::ElementType enum can be added according to mesh dimensionality
 * (3D element in volume mesh, 2D in surface mesh, etc..). The method does nothing, if class type
 * is a pointcloud one (type 3).
 * As a reminder for connectivity conn argument:
 *  - Connectivity of polygons must be defined as (nV,V1,V2,V3,V4,...) where nV is the number of vertices
 * defining the polygon and V1,V2,V3... are indices of vertices.
 *  - Connectivity of polyhedrons must be defined as (nF,nF1V, V1, V2, V3,..., nF2V, V2,V4,V5,V6,...,....)
 * where nF is the total number of faces of polyhedros, nF1V is the number of vertices composing the face 1,
 * followed by the indices of vertices which draws it. Face 2,3,..,n are defined in the same way.
 *  - Any other standard cell element is uniquely defined by its list of vertex indices.
 *
 * \param[in] conn  connectivity of target cell of geometry mesh.
 * \param[in] type  type of element to be added, according to bitpit::ElementInfo enum.
 * \param[in] idtag id of the cell
 * \return false if no geometry is linked, idtag already assigned or mismatched connectivity/element type
 */
bool
MimmoObject::addConnectedCell(const livector1D & conn, bitpit::ElementType type, long idtag){

	if (conn.empty() || !m_skdTreeSupported) return false;
	if(idtag != bitpit::Cell::NULL_ID && getCells().exists(idtag)) return false;

	if(!checkCellConnCoherence(type, conn))  return false;

	bitpit::PatchKernel::CellIterator it;
	auto patch = getPatch();

#if MIMMO_ENABLE_MPI
		it = patch->addCell(type, conn, m_rank, idtag);
#else
		it = patch->addCell(type, conn, idtag);
#endif

	m_pidsType.insert(0);
	m_pidsTypeWNames.insert(std::make_pair( 0, "") );

	m_skdTreeSync = false;
	m_AdjBuilt = false;
	m_IntBuilt = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
	return true;
};

/*!
 * It adds one cell with its vertex-connectivity (vertex in bitpit::PatchKernel unique id's), the type of cell to
 * be added and its own unique id. If no id is specified, teh method assigns it automatically.
 * Cell type and connectivity dimension of the cell are cross-checked with the mesh type of the class: if mismatch, the method
 * does not add the cell and return false.
 * The method does nothing, if class type is a pointcloud one (type 3).
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
 * \param[in] conn  connectivity of target cell of geometry mesh.
 * \param[in] type  type of element to be added, according to bitpit ElementInfo enum.
 * \param[in] PID   part identifier
 * \param[in] idtag id of the cell
 * \return false if no geometry is linked, idtag already assigned or mismatched connectivity/element type
 */
bool
MimmoObject::addConnectedCell(const livector1D & conn, bitpit::ElementType type, long PID, long idtag){

	if (conn.empty() || !m_skdTreeSupported) return false;
	if(idtag != bitpit::Cell::NULL_ID && getCells().exists(idtag)) return false;

	if(!checkCellConnCoherence(type, conn))  return false;

	bitpit::PatchKernel::CellIterator it;
	auto patch = getPatch();

	long checkedID;
#if MIMMO_ENABLE_MPI
	it = patch->addCell(type, conn, m_rank, idtag);
#else
	it = patch->addCell(type, conn, idtag);
#endif
	checkedID = it->getId();

	setPIDCell(checkedID, PID);
	m_skdTreeSync = false;
	m_AdjBuilt = false;
	m_IntBuilt = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
	return true;
};

/*!
 *It adds one cell to the mesh.
 * If unique-id is specified for the cell, assign it, otherwise provide itself
 * to get a unique-id for the added cell. The latter option is the default.
 * If the unique-id is already assigned, return with unsuccessful insertion.
 *
 *
 * \param[in] cell cell to be added
 * \param[in] idtag  unique id associated to the cell
 * \return true if the cell is successful inserted.
 */
bool
MimmoObject::addCell(bitpit::Cell & cell, const long idtag){

    if(idtag != bitpit::Cell::NULL_ID && getCells().exists(idtag))    return false;

    cell.resetAdjacencies();

    bitpit::PatchKernel::CellIterator it;
    auto patch = getPatch();
#if MIMMO_ENABLE_MPI
    it = patch->addCell(cell, m_rank, idtag);
#else
    it = patch->addCell(cell, idtag);
#endif

    m_pidsType.insert(cell.getPID());
    m_pidsTypeWNames.insert(std::make_pair( cell.getPID(), "") );

    m_skdTreeSync = false;
    m_kdTreeSync = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
    return true;
};


/*!
 * Set PIDs for all geometry cells available.
 * Any previous PID subdivion with label name associated will be erased.
 * The PID list must be referred to the compact local/indexing of the cells in the class.
 * \param[in] pids PID list.
 */
void
MimmoObject::setPID(livector1D pids){
	if((int)pids.size() != getNCells() || !m_skdTreeSupported)	return;

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
	if(getNCells() == 0 || !m_skdTreeSupported)	return;

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
	for(auto const & cell : getCells()){
		m_pidsType.insert( (long)cell.getPID() );
	}
	for(const auto & pid : m_pidsType){
		m_pidsTypeWNames.insert(std::make_pair( pid, copynames[pid]));
	}
};


/*!
 * Set your current class as a "hard" copy of another MimmoObject.
 * All preexistent data are destroyed and replaced by those provided by the argument, except for search Trees,
 * which are instantiated, but not filled/built/synchronized.
 * Hard means that the geometry data structure is allocated internally
 * as an exact and stand-alone copy of the geometry data structure of the argument,
 * indipendently on how the argument owns or links it.
 * \param[in] other MimmoObject class.
 */
void MimmoObject::setHARDCopy(const MimmoObject * other){

	m_type  = other->m_type;
	m_internalPatch = true;
	m_extpatch = NULL;

	switch(m_type){
	case 1:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(2)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoVolUnstructured(3)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoPointCloud));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(1)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

	m_skdTreeSupported = other->m_skdTreeSupported;
	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif

	//copy data
	const bitpit::PiercedVector<bitpit::Vertex> & pvert = other->getVertices();
	setVertices(pvert);

	if(m_skdTreeSupported){
		const bitpit::PiercedVector<bitpit::Cell> & pcell = other->getCells();
		setCells(pcell);
	}

	if(other->m_AdjBuilt)   buildAdjacencies();
	if(other->m_IntBuilt)   buildInterfaces();

};

/*!
 * Clone your MimmoObject in a new indipendent MimmoObject. All data of the current class will be "hard" copied in a new
 * MimmoObject class. Search Trees will be only instantiated but not filled/built/synchronized.
 * Hard means that the geometry data structure will be allocated internally into the new object
 * as an exact and stand-alone copy of the geometry data structure of the current class, indipendently on how the current class
 * owns or links it.
 * \return cloned MimmoObject.
 */
std::unique_ptr<MimmoObject> MimmoObject::clone(){
	std::unique_ptr<MimmoObject> result(new MimmoObject(getType()));
	//copy data
	result->setVertices(getVertices());
	if(m_skdTreeSupported){
		result->setCells(getCells());
	}
	if(m_AdjBuilt)   result->buildAdjacencies();
	if(m_IntBuilt)   result->buildInterfaces();

	result->resyncPID();
	auto mapPID = getPIDTypeListWNames();
	for(const auto & touple: mapPID){
		result->setPIDName(touple.first, touple.second);
	}
	return std::move(result);
};

/*!
 * It cleans geometry duplicated and, in case of connected tessellations, all orphan/isolated vertices.
 * \return false if the geometry member pointer is NULL.
 */
bool
MimmoObject::cleanGeometry(){
	auto patch = getPatch();
	patch->deleteCoincidentVertices();
	if(m_skdTreeSupported)  patch->deleteOrphanVertices();

	m_kdTreeSync = false;
	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
	return true;
};

/*!
 * Extract vertex list from an ensamble of geometry cells.
 *\param[in] cellList list of bitpit::PatchKernel ids identifying cells.
 *\return the list of bitpit::PatchKernel ids of involved vertices.
 */
livector1D MimmoObject::getVertexFromCellList(const livector1D &cellList){
    if(isEmpty() || getType() == 3)   return livector1D(0);

    livector1D result;
    set<long int> ordV;
    auto patch = getPatch();
    bitpit::PiercedVector<bitpit::Cell> & cells = getCells();
    //get conn from each cell of the list
    for(const auto id : cellList){
        if(cells.exists(id)){
            bitpit::ConstProxyVector<long> ids = cells.at(id).getVertexIds();
            for(const auto & val: ids){
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
    if(isEmpty() || getType() == 3)   return livector1D(0);

    if(!areInterfacesBuilt())   buildInterfaces();
    livector1D result;
    set<long int> ordV;
    auto patch = getPatch();

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
    		if (targetCells.count(idowner) && (targetCells.count(idneigh) || idneigh<0)){
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
 * \param[in] strict boolean true to restrict search for cells having all vertices in the list, false to include all cells having at least 1 vertex in the list.
 * \return list of bitpit::PatchKernel IDs of involved 1-Ring cells.
 */
livector1D MimmoObject::getCellFromVertexList(const livector1D & vertexList, bool strict){
    if(isEmpty() || getType() == 3)   return livector1D(0);

    livector1D result;
    std::unordered_set<long int> ordV, ordC;
    ordV.insert(vertexList.begin(), vertexList.end());
    //get conn from each cell of the list
    for(auto it = getPatch()->cellBegin(); it != getPatch()->cellEnd(); ++it){
        bitpit::ConstProxyVector<long> vIds= it->getVertexIds();
        bool check;
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
 * \param[in] vertexList list of bitpit::PatchKernel IDs identifying vertices.
 * \param[in] strict boolean true to restrict search for cells having all vertices in the list, false to include all cells having at least 1 vertex in the list.
 * \param[in] border boolean true to restrict the search to border interfaces, false to include all interfaces
 * \return list of bitpit::PatchKernel IDs of involved interfaces
 */

livector1D MimmoObject::getInterfaceFromVertexList(const livector1D & vertexList, bool strict, bool border){
    if(isEmpty() || getType() == 3)   return livector1D(0);

    if(!areInterfacesBuilt())   buildInterfaces();

    livector1D result;
    std::unordered_set<long int> ordV, ordI;
    ordV.insert(vertexList.begin(), vertexList.end());
    //get conn from each cell of the list
    for(auto it = getPatch()->interfaceBegin(); it != getPatch()->interfaceEnd(); ++it){
        if(border && !it->isBorder()) continue;
        bitpit::ConstProxyVector<long> vIds= it->getVertexIds();
        bool check;
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
 * Extract vertices at the mesh boundaries, if any. The method is meant for connected mesh only,
 * return empty list otherwise.
 * \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
 * \return list of vertex unique-ids.
 */
livector1D 	MimmoObject::extractBoundaryVertexID(bool ghost){

    std::unordered_map<long, std::set<int> > cellmap = extractBoundaryFaceCellID(ghost);
    if(cellmap.empty()) return livector1D(0);

	std::unordered_set<long> container;

	for (const auto & val : cellmap){
		bitpit::Cell & cell = getPatch()->getCell(val.first);
		for(const auto face : val.second){
			bitpit::ConstProxyVector<long> list = cell.getFaceVertexIds(face);
			for(const auto & index : list ){
				container.insert(index);
			}
		}// end loop on face
	}

	livector1D result;
	result.reserve(container.size());
	result.insert(result.end(), container.begin(), container.end());

	return result;
};

/*!
 * Extract cells who have one face at the mesh boundaries at least, if any.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
 * \return list of cell unique-ids.
 */
livector1D  MimmoObject::extractBoundaryCellID(bool ghost){

	if(isEmpty() || m_type==3)   return livector1D(0);
	if(!areAdjacenciesBuilt())   getPatch()->buildAdjacencies();

    std::unordered_set<long> container;
    auto itBegin = getPatch()->internalBegin();
    auto itEnd = getPatch()->internalEnd();
    if(ghost){
        itEnd = getPatch()->ghostEnd();
    }
    for (auto it=itBegin; it!=itEnd; ++it){
        int size = it->getFaceCount();
        for(int face=0; face<size; ++face){
            if(it->isFaceBorder(face)){
                container.insert(it.getId());
            }//endif
        }// end loop on face
    }
	livector1D result;
	result.reserve(container.size());
	result.insert(result.end(), container.begin(), container.end());

	return result;
};

/*!
 * Extract cells  who have one face at the mesh boundaries at least, if any.
 * Return the list of the local faces per cell, which lie exactly on the boundary.
 * The method is meant for connected mesh only, return empty list otherwise.
*  \param[in] ghost true if the ghosts must be accounted into the search, false otherwise
 * \return map of boundary cell unique-ids, with local boundary faces list.
 */
std::unordered_map<long, std::set<int> >  MimmoObject::extractBoundaryFaceCellID(bool ghost){

	std::unordered_map<long, std::set<int> > result;
	if(isEmpty() || m_type ==3)   return result;
	if(!areAdjacenciesBuilt())   getPatch()->buildAdjacencies();

    auto itBegin = getPatch()->internalBegin();
    auto itEnd = getPatch()->internalEnd();
    if(ghost){
        itEnd = getPatch()->ghostEnd();
    }
    for (auto it=itBegin; it!=itEnd; ++it){
        int size = it->getFaceCount();
        for(int face=0; face<size; ++face){
            if(it->isFaceBorder(face)){
                result[it.getId()].insert(face);
            }//endif
        }// end loop on face
    }
    return result;
};

/*!
 * Extract vertices at the mesh boundaries, provided the map of the cell faces at boundaries.
 * The method is meant for connected mesh only, return empty list otherwise.
 * \param[in] cellmap map of border faces of the mesh written as cell-ID vs local cell face index.
 * \return list of vertex unique-ids.
 */
livector1D  MimmoObject::extractBoundaryVertexID(std::unordered_map<long, std::set<int> > &cellmap){

	if(cellmap.empty()) return livector1D(0);

	std::unordered_set<long> container;

	for (const auto & val : cellmap){
		bitpit::Cell & cell = getPatch()->getCell(val.first);
		for(const auto face : val.second){
			bitpit::ConstProxyVector<long> list = cell.getFaceVertexIds(face);
			for(const auto & index : list ){
				container.insert(index);
			}
		}// end loop on face
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
 * \return list of cell unique-ids.
 */
livector1D  MimmoObject::extractBoundaryInterfaceID(bool ghost){

    if(isEmpty() || m_type==3)   return livector1D(0);
    if(!areInterfacesBuilt())   buildInterfaces();

    std::unordered_set<long> container;
    std::unordered_map<long, std::set<int> > facemap = extractBoundaryFaceCellID(ghost);

    for(auto & tuple : facemap){
        long * interfCellList  = getPatch()->getCell(tuple.first).getInterfaces();
        for(auto & val : tuple.second){
            if(interfCellList[val] < 0) continue;
            container.insert(interfCellList[val]);
        }
    }

    livector1D result;
    result.reserve(container.size());
    result.insert(result.end(), container.begin(), container.end());

    return result;
};

/*!
 * Extract all cells marked with a target PID flag.
 * \param[in]   flag    PID for extraction
 * \return  list of cells as unique-ids
 */
livector1D	MimmoObject::extractPIDCells(long flag){

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
	return  result;
};

/*!
 * Extract all cells marked with a series of target PIDs.
 * \param[in]   flag    list of PID for extraction
 * \return      list of cells as unique-ids
 */
livector1D	MimmoObject::extractPIDCells(livector1D flag){
	livector1D result;
	result.reserve(getNCells());
	for(auto && id : flag){
		livector1D partial = extractPIDCells(id);
		result.insert(result.end(),partial.begin(), partial.end());
	}
	return(result);
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
		if(list.size() < 5) return false;
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
 */
void MimmoObject::getBoundingBox(std::array<double,3> & pmin, std::array<double,3> & pmax){
	getPatch()->getBoundingBox(pmin,pmax);
	return;
}

/*!
 * Reset and build again cell skdTree of your geometry (if supports connectivity elements).
 * Ghost cells are insert in the tree.
 *\param[in] value build the minimum leaf of the tree as a bounding box containing value elements at most.
 */
void MimmoObject::buildBvTree(int value){
	if(!m_skdTreeSupported || isEmpty())   return;

	if (!m_skdTreeSync){
		m_skdTree->clear();
		m_skdTree->build(value);
		m_skdTreeSync = true;
	}
	return;
}

/*!
 * Reset and build again cell skdTree of your geometry (if supports connectivity elements).
 * Ghost cells are insert in the tree.
 *\param[in] value build the minimum leaf of the tree as a bounding box containing value elements at most.
 */
void MimmoObject::buildSkdTree(int value){
	if(!m_skdTreeSupported || isEmpty())   return;

	if (!m_skdTreeSync){
		m_skdTree->clear();
		m_skdTree->build(value);
		m_skdTreeSync = true;
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

	if (!m_kdTreeSync){
		cleanKdTree();
		m_kdTree->nodes.resize(getNVertices() + m_kdTree->MAXSTK);

		for(auto & val : getVertices()){
			label = val.getId();
			m_kdTree->insert(&val, label);
		}
		m_kdTreeSync = true;
	}
	return;
}

/*!
 * Clean the KdTree of the class
 */
void	MimmoObject::cleanKdTree(){
	m_kdTree->n_nodes = 0;
	m_kdTree->nodes.clear();
}

/*!
 * Reset and build again cell patch numbering info of your geometry.
 */
void MimmoObject::buildPatchInfo(){
	m_patchInfo.reset();
	if (m_internalPatch)
		m_patchInfo.setPatch(m_patch.get());
	else
		m_patchInfo.setPatch(m_extpatch);
	m_patchInfo.update();
	m_infoSync = true;
	return;
}

/*!
 * \return true if cell-cell adjacency is built for your current mesh.
 */
bool MimmoObject::areAdjacenciesBuilt(){
	return  m_AdjBuilt;
};

/*!
 * \return true if Interfaces are built for your current mesh.
 */
bool MimmoObject::areInterfacesBuilt(){
	return  m_IntBuilt;
};

/*!
 * \return false if your local mesh has open edges/faces. True otherwise.
 */
bool MimmoObject::isClosedLoop(){

	if(!areAdjacenciesBuilt())	buildAdjacencies();
	bool check = true;

	auto itp = getCells().cbegin();
	auto itend = getCells().cend();

	while(itp != itend && check){

		if (itp->isInterior()){
			int cAdj = itp->getAdjacencyCount();
			const long * adj = itp->getAdjacencies();
			for(int i=0; i<cAdj; ++i){
				check = check && (adj[i] < 0);
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
 * If the mesh is composed by multiple unconnected patches, the method returns
 * the cellID list belonging to each subpatches
 *\return list of cellIDs set for each subpatch
 */
livector2D MimmoObject::decomposeLoop(){
    if(!areAdjacenciesBuilt())	buildAdjacencies();

    livector2D result;
    livector1D globalcellids = getCells().getIds();
    std::unordered_set<long> globalcells(globalcellids.begin(), globalcellids.end());
    globalcellids.clear();

    while(!globalcells.empty()){

        livector1D stack, save;
        stack.push_back(*(globalcells.begin()));

        while(!stack.empty()){

            long target = stack.back();
            stack.pop_back();
            save.push_back(target);
            globalcells.erase(target);
            livector1D neighs = getPatch()->findCellNeighs(target,1);
            for(long val: neighs){
                if(globalcells.count(val) >0){
                    stack.push_back(val);
                }
            }
        }
        result.push_back(save);
    }

    return result;
}

/*!
 * Force the class to build cell-cell adjacency connectivity.
 */
void MimmoObject::buildAdjacencies(){
	if(m_type !=3){
		getPatch()->buildAdjacencies();
		m_AdjBuilt = true;
	}
};

/*!
 * Force the class to build Interfaces connectivity.
 * If MimmoObject does not support connectivity (as Point Clouds do)
 * does nothing.
 */
void MimmoObject::buildInterfaces(){
	if(m_type !=3){
		if(!areAdjacenciesBuilt()) buildAdjacencies();
		getPatch()->buildInterfaces();
		m_IntBuilt=  true;
	}
};

/*!
 * Force the class to reset cell-cell adjacency connectivity.
 */
void MimmoObject::resetAdjacencies(){
	if(m_type !=3){
		for (bitpit::Cell & cell : getPatch()->getCells()){
			cell.deleteAdjacencies();
		}
		m_AdjBuilt = false;
	}
};

/*!
 * Force the class to reset Interfaces connectivity.
 */
void MimmoObject::resetInterfaces(){
	if(m_type !=3){
		getPatch()->resetInterfaces();
		m_IntBuilt = false;
	}
};

/*!
 * Force the class to reset all the patch structures, i.e. vertices, cells, interfaces.
 */
void MimmoObject::resetPatch(){
	getPatch()->reset();
	m_AdjBuilt = false;
	m_IntBuilt = false;
	m_skdTreeSync = false;
	m_kdTreeSync = false;
 	m_patchInfo.reset();
 	m_infoSync = false;
#if MIMMO_ENABLE_MPI
	m_pointGhostExchangeInfoSync = false;
#endif
};

/*!
 * Desume Element given the vertex connectivity list associated. Polygons and Polyhedra require
 * a special writing for their connectivity list. Please read doxy of MimmoObject(int type, dvecarr3E & vertex, livector2D * connectivity = NULL)
 * custom constructor for further information.
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
		result = bitpit::ElementType::VERTEX;
		break;
	case    4:
		result = bitpit::ElementType::LINE;
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
 */
void MimmoObject::reset(int type){
	m_log = &bitpit::log::cout(MIMMO_LOG_FILE);
	m_type = std::max(0, type);
	if (m_type > 4){
		m_type = 1;
	}

	switch(m_type){
	case 1:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(2)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 2:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoVolUnstructured(3)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new VolumeSkdTree(dynamic_cast<VolumeKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 3:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoPointCloud()));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	case 4:
		m_patch = std::move(std::unique_ptr<PatchKernel>(new MimmoSurfUnstructured(1)));
		m_skdTree = std::move(std::unique_ptr<PatchSkdTree>(new bitpit::SurfaceSkdTree(dynamic_cast<SurfaceKernel*>(m_patch.get()))));
		m_kdTree  = std::move(std::unique_ptr<KdTree<3,bitpit::Vertex,long> >(new KdTree<3,bitpit::Vertex, long>()));
		break;
	default:
		//never been reached
		break;
	}

	m_internalPatch = true;
	m_extpatch = NULL;

	m_skdTreeSupported = (m_type != 3);
	m_skdTreeSync = false;
	m_kdTreeSync = false;
	m_AdjBuilt = false;
	m_IntBuilt = false;
#if MIMMO_ENABLE_MPI
	initializeParallel();
#endif
	m_patchInfo.setPatch(m_patch.get());
	m_infoSync = false;
}

/*!
 * Dump contents of your current MimmoObject to a stream.
 * Search trees and adjacencies will not be dumped.
 * Write all in a binary format.
 * \param[in,out] stream to write on.
 */
void MimmoObject::dump(std::ostream & stream){
	auto mapPid = getPIDTypeListWNames();
	std::vector<long> pid(mapPid.size());
	std::vector<std::string> sspid(mapPid.size());
	int counter = 0;
	for(const auto & touple : mapPid){
		pid[counter] = touple.first;
		sspid[counter] = touple.second;
		++counter;
	}

	bitpit::utils::binary::write(stream,m_type);
	bitpit::utils::binary::write(stream,pid);
	bitpit::utils::binary::write(stream,sspid);
	getPatch()->dump(stream);
}

/*!
 * Restore contents of a dumped MimmoObject from a stream in your current class.
 * New restored data will be owned internally by the class.
 * Every data previously stored will be lost.
 * Read all in a binary format.
 * \param[in,out] stream to write on.
 */

void MimmoObject::restore(std::istream & stream){
	int type;
	std::vector<long> pid;
	std::vector<std::string> sspid;

	bitpit::utils::binary::read(stream,type);
	bitpit::utils::binary::read(stream,pid);
	bitpit::utils::binary::read(stream,sspid);
	reset(type);

	getPatch()->restore(stream);

	int count = 0;
	for (const auto &pp : pid){
		m_pidsType.insert(pp);
		m_pidsTypeWNames.insert( std::make_pair( pp, sspid[count]));
		++count;
	}
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

	if(getPatch() == NULL)   return;
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

	if(getPatch() == NULL)   return;
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
		if(!areInterfacesBuilt())   buildInterfaces();
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
		if(!areInterfacesBuilt())   buildInterfaces();
		bitpit::VolUnstructured * p = static_cast<bitpit::VolUnstructured *>(getPatch());

		double Svalue = 0.0;
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
 * \return map of all bitpit::ElementType (casted as integers) hold by the current bitpit::PatchKernel obj. Ghost cells are considered.
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
 * Get all the cells of the current mesh whose center is within a prescribed distance maxdist w.r.t to a target surface body
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
 * Get all the cells of the current mesh whose center is within a prescribed distance maxdist w.r.t to a target surface body
 * Ghost cells are considered.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[in] seedlist (optional) list of cells to starting narrow band search
 * \return list of cells inside the narrow band at maxdist from target surface with their distance from it.
 */
bitpit::PiercedVector<double>
MimmoObject::getCellsNarrowBandToExtSurfaceWDist(MimmoObject & surface, const double & maxdist, livector1D * seedlist){

    bitpit::PiercedVector<double> result;
    if(surface.isEmpty() || surface.getType() != 1) return result;
	if(isEmpty() || getType() == 3)  return result;

    if(!surface.areAdjacenciesBuilt()){
        surface.buildAdjacencies();
    }

    //use a stack-based search on face cell neighbors of some prescribed seed cells.
    //You need at least one cell to be near enough to surface

    //First step check seed list and precalculate distance. If dist >= maxdist
    //the seed candidate is out.
    darray3E pp;
    double distance, maxdistance(maxdist);
    long idsuppsurf;
    if(seedlist){
        for(long id: *seedlist){
            pp = getPatch()->evalCellCentroid(id);
            distance = mimmo::skdTreeUtils::distance(&pp, surface.getSkdTree(),idsuppsurf,maxdistance);
            if(distance < maxdist){
                result.insert(id, distance);
            }
        }
    }
    // if no seed list is given or none of the seed are into the maxdist search for a seed.
    if(result.empty()){
        //find all the unconnected patches composing surfaces.
        livector2D loops = surface.decomposeLoop();
        //ping a seed on each subpatch
        for(livector1D & loop : loops){
            long idseed;
            auto itsurf = loop.begin();
            auto itsurfend = loop.end();
            darray3E point;
            while(itsurf != itsurfend ){
                //continue to search for a seed cell candidate, visiting all point of the target surface.
                idseed = mimmo::skdTreeUtils::closestCellToPoint(surface.evalCellCentroid(*itsurf), *(this->getSkdTree()));
                if(idseed < 0) {
                    ++itsurf;
                    continue;
                }
                pp = getPatch()->evalCellCentroid(idseed);
                distance = mimmo::skdTreeUtils::distance(&pp, surface.getSkdTree(),idsuppsurf,maxdistance);
                if(distance >= maxdist){
                    ++itsurf;
                    continue;
                }
                result.insert(idseed, distance);
                itsurf = itsurfend;
            }
        }
    }

    // if no seed is found exit.
    if(result.empty()){
        //impossible to find a seed;
        return result;
    }

    // create the stack of neighbours with the seed i have.
    std::unordered_set<long> visited;
    livector1D stackNeighs;
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

    long target;
    //search until the stack is empty.
    while(!stackNeighs.empty()){

        // extract the candidate
        target = stackNeighs.back();

        stackNeighs.pop_back();
        visited.insert(target);
        // evaluate its distance;
        pp = getPatch()->evalCellCentroid(target);
        distance = mimmo::skdTreeUtils::distance(&pp, surface.getSkdTree(),idsuppsurf,maxdistance);

        if(distance < maxdist){
            result.insert(target, distance);

            livector1D neighs = getPatch()->findCellNeighs(target, 1); //only face neighs.
            for(long id: neighs){
                if(visited.count(id) < 1){
                    visited.insert(id);
                    stackNeighs.push_back(id);
                }
            }
        }
    }

    //TODO need to find a cooking recipe here in case of PARALLEL computing of the narrow band.

};

/*!
 * Get all the vertices of the current mesh within a prescribed distance maxdist w.r.t to a target surface body
 * Nodes of ghost cells are considered.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[out] idList list of vertex IDs within maxdistance
 */
void
MimmoObject::getVerticesNarrowBandToExtSurface(MimmoObject & surface, const double & maxdist, livector1D & idList){

	if(surface.isEmpty() || surface.getType() != 1) return;
	if(isEmpty())  return ;

	idList.clear();
	idList.reserve(getNVertices());
	darray3E pp;
	double distance, maxdistance(maxdist);
	long idsuppsurf;
	for(const auto & vertex : getVertices()){
		pp = vertex.getCoords();
		distance = mimmo::skdTreeUtils::distance(&pp, surface.getSkdTree(),idsuppsurf,maxdistance);
		if(distance < maxdist)  idList.push_back(vertex.getId());
	}
};

/*!
 * Get all the vertices of the current mesh within a prescribed distance maxdist w.r.t to a target surface body
 * Nodes of ghost cells are considered.
 * \param[in] surface MimmoObject of type surface.
 * \param[in] maxdist threshold distance.
 * \param[out] distList map of vertex IDs-distances positive matches within maxdistance
 */
void
MimmoObject::getVerticesNarrowBandToExtSurface(MimmoObject & surface, const double & maxdist,bitpit::PiercedVector<double> & distList){
	if(surface.isEmpty() || surface.getType() != 1) return;
	if(isEmpty())  return ;

	darray3E pp;
	double distance, maxdistance(maxdist);
	long idsuppsurf;
	distList.clear();
	distList.reserve(getNVertices());
	for(const auto & vertex : getVertices()){
		pp = vertex.getCoords();
		distance = mimmo::skdTreeUtils::distance(&pp, surface.getSkdTree(),idsuppsurf,maxdistance);
		if(distance < maxdist)  distList.insert(vertex.getId(),distance);
	}
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
	if(getType() == 3) return invConn;

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
 * Return Vertex-Vertex One Ring of a specified target vertex. Does not work with unconnected meshes (point clouds)
 * \param[in]    cellId     bitpit::PatchKernel Id of a cell which target belongs to
 * \param[in]    vertexId    bitpit::PatchKernel Id of the target vertex
 * \return        list of all vertex in the One Ring of the target, by their bitpit::PatchKernel Ids
 */
std::set<long> MimmoObject::findVertexVertexOneRing(const long & cellId, const long & vertexId){
	std::set<long> result;
	if(getType() == 3)  return result;

	bitpit::PatchKernel * tri = getPatch();
	bitpit::Cell &cell =  tri->getCell(cellId);

	int loc_target = cell.findVertex(vertexId);
	if(loc_target ==bitpit::Vertex::NULL_ID) return result;

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
    if(areInterfacesBuilt()){
        result = getPatch()->evalInterfaceCentroid(id);
    }
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
    if(!areInterfacesBuilt())  return result;

    switch(m_type){
        case 1:
            {
                bitpit::Interface & interf = getInterfaces().at(id);
                std::array<double,3> enormal = static_cast<bitpit::SurfaceKernel*>(getPatch())->evalEdgeNormal(interf.getOwner(), interf.getOwnerFace());
                ConstProxyVector<long> vv = interf.getVertexIds();
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
    if(!areInterfacesBuilt())  return result;

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


}
