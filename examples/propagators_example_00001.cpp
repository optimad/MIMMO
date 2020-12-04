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
#if MIMMO_ENABLE_MPI
#include "mimmo_parallel.hpp"
#endif
#include "mimmo_propagators.hpp"

#include <iostream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

// =================================================================================== //
/*!
	example propagators_example_00001.cpp

	brief Example of usage of PropagateVectorField in propagators.

	<b>To run</b>: mpirun -np X propagators_example_00001 \n

	<b> visit</b>: <a href="http://optimad.github.io/mimmo/">mimmo website</a> \n
 */


// =================================================================================== //

mimmo::MimmoSharedPointer<mimmo::MimmoObject> createTestVolumeMesh(int rank, std::vector<long> &bcdir1_vertlist, std::vector<long> &bcdir2_vertlist){

	std::array<double,3> center({{0.0,0.0,0.0}});
	double radiusin(2.0), radiusout(5.0);
	double azimuthin(0.0), azimuthout(0.5*BITPIT_PI);
	double heightbottom(-1.0), heighttop(1.0);
	int nr(20), nt(20), nh(20);

	double deltar = (radiusout - radiusin)/ double(nr);
	double deltat = (azimuthout - azimuthin)/ double(nt);
	double deltah = (heighttop - heightbottom)/ double(nh);

	std::vector<std::array<double,3> > verts ((nr+1)*(nt+1)*(nh+1));

	int counter = 0;
	for(int k=0; k<=nh; ++k){
		for(int j=0; j<=nt; ++j){
			for(int i=0; i<=nr; ++i){
				verts[counter][0] =(radiusin + i*deltar)*std::cos(azimuthin + j*deltat);
				verts[counter][1] =(radiusin + i*deltar)*std::sin(azimuthin + j*deltat);
				verts[counter][2] =(heightbottom + k*deltah);
				++counter;
			}
		}
	}

	//create the volume mesh mimmo.
	mimmo::MimmoSharedPointer<mimmo::MimmoObject> mesh(new mimmo::MimmoObject(2));

	if (rank == 0){
		mesh->getPatch()->reserveVertices((nr+1)*(nt+1)*(nh+1));
		mesh->getPatch()->reserveCells(nr*nt*nh);

		//pump up the vertices
		for(const auto & vertex : verts){
			mesh->addVertex(vertex); //automatic id assigned to vertices.
		}

		//create connectivities for hexa elements
		std::vector<long> conn(8,0);
		for(int k=0; k<nh; ++k){
			for(int j=0; j<nt; ++j){
				for(int i=0; i<nr; ++i){
					conn[0] = (nr+1)*(nt+1)*k + (nr+1)*j + i;
					conn[1] = (nr+1)*(nt+1)*k + (nr+1)*j + i+1;
					conn[2] = (nr+1)*(nt+1)*k + (nr+1)*(j+1) + i+1;
					conn[3] = (nr+1)*(nt+1)*k + (nr+1)*(j+1) + i;
					conn[4] = (nr+1)*(nt+1)*(k+1) + (nr+1)*j + i;
					conn[5] = (nr+1)*(nt+1)*(k+1) + (nr+1)*j + i+1;
					conn[6] = (nr+1)*(nt+1)*(k+1) + (nr+1)*(j+1) + i+1;
					conn[7] = (nr+1)*(nt+1)*(k+1) + (nr+1)*(j+1) + i;

					mesh->addConnectedCell(conn, bitpit::ElementType::HEXAHEDRON);
				}
			}
		}

        bcdir1_vertlist.clear();
		bcdir2_vertlist.clear();
		bcdir1_vertlist.reserve(mesh->getNVertices());
		bcdir2_vertlist.reserve(mesh->getNVertices());

		for(int k=0; k<=nh; ++k){
			for(int i=0; i<=nr; ++i){
				bcdir1_vertlist.push_back((nr+1)*(nt+1)*k + i);
				bcdir2_vertlist.push_back((nr+1)*(nt+1)*k + (nr+1)*nt + i);
			}
		}
	}

	mesh->updateInterfaces();
	mesh->update();

	return mesh;
}

// =================================================================================== //

int test00001() {

#if MIMMO_ENABLE_MPI
	// Initialize mpi
	int nProcs;
	int    rank;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#else
	int nProcs = 1;
	int rank = 0;
#endif

    std::vector<long> bc1list, bc2list;
    mimmo::MimmoSharedPointer<mimmo::MimmoObject> mesh = createTestVolumeMesh(rank, bc1list, bc2list);
    mimmo::MimmoSharedPointer<mimmo::MimmoObject> boundary= mesh->extractBoundaryMesh();
    //pidding the boundary
    livector1D pidlist1 = boundary->getCellFromVertexList(bc1list, true);
    livector1D pidlist2 = boundary->getCellFromVertexList(bc2list, true);
    //pid the cells.
    for(long cellid : pidlist1){
        boundary->setPIDCell(cellid, long(1));
    }
    for(long cellid : pidlist2){
        boundary->setPIDCell(cellid, long(2));
    }

    //create a subselection of the boundary
    mimmo::MimmoSharedPointer<mimmo::MimmoObject> bdirMesh(new mimmo::MimmoObject(boundary->getType()));

    if(rank == 0){   //create a subselection of the whole boundary
        //vertices
        std::unordered_set<long> potvertices(bc1list.begin(), bc1list.end());
        potvertices.insert(bc2list.begin(), bc2list.end());
        for(long idv: potvertices){
            bdirMesh->addVertex(boundary->getVertexCoords(idv), idv);
        }
        //cell
        //mark to delete all cells not in the pidlist1 and 2.
        std::unordered_set<long> preservedcells (pidlist1.begin(), pidlist1.end());
        preservedcells.insert(pidlist2.begin(), pidlist2.end());

        for(long idc: preservedcells){
            bdirMesh->addCell(boundary->getPatch()->getCell(idc), idc, rank);
        }
    }
    bdirMesh->updateInterfaces();
    bdirMesh->update();


#if MIMMO_ENABLE_MPI
	/* Instantiation of a Partition object with default patition method space filling curve.
	 * Plot Optional results during execution active for Partition block.
	 */
	mimmo::Partition* partition = new mimmo::Partition();
	partition->setPlotInExecution(true);
	partition->setGeometry(mesh);
	partition->setBoundaryGeometry(bdirMesh);
    partition->setPartitionMethod(mimmo::PartitionMethod::PARTGEOM);
	auto t1 = Clock::now();
	if (rank ==0)
		std::cout << "Start Partition mesh " << std::endl;
	partition->exec();
	auto t2 = Clock::now();
	if (rank ==0)
	{
		std::cout << "Partition mesh execution time: "
				<< std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count()
				<< " seconds" << std::endl;
	}
#endif

	// and the field of Dirichlet values on its nodes.
	mimmo::MimmoPiercedVector<std::array<double,3>> bc_surf_field;
	//MimmoPiercedVector<double> bc_surf_field;
	bc_surf_field.setGeometry(bdirMesh);
	bc_surf_field.setDataLocation(mimmo::MPVLocation::POINT);
	bc_surf_field.reserve(bdirMesh->getNVertices());

	for(auto & cell : bdirMesh->getCells()){
		if (cell.getPID() == 1){
			for (long id : cell.getVertexIds()){
				if (!bc_surf_field.exists(id))
					bc_surf_field.insert(id, {{1., 1., 0.}});
			}
		}
		if (cell.getPID() == 2){
			for (long id : cell.getVertexIds()){
				if (!bc_surf_field.exists(id))
					bc_surf_field.insert(id, {{0., 0., 0.}});
			}
		}
	}

    mesh->update();
    bdirMesh->update();

	// Now create a PropagateScalarField and solve the laplacian.
	mimmo::PropagateVectorField * prop = new mimmo::PropagateVectorField();
	prop->setGeometry(mesh);
	prop->addDirichletBoundaryPatch(bdirMesh);
	prop->addDirichletConditions(&bc_surf_field);
	prop->setSolverMultiStep(10);
	prop->setPlotInExecution(true);
	prop->setApply(true);

    prop->setDamping(true);
    prop->setDampingType(1);
    prop->setDampingDecayFactor(1.0);
    prop->setDampingInnerDistance(0.05);
    prop->setDampingOuterDistance(0.35);
    prop->addDampingBoundarySurface(bdirMesh);

    prop->setNarrowBand(false);
    prop->setNarrowBandWidth(0.6);
    prop->setNarrowBandRelaxation(0.7);
    prop->addNarrowBandBoundarySurface(bdirMesh);


    t1 = Clock::now();
    if (rank == 0){
        std::cout << "Start Propagator vector field " << std::endl;
    }
    prop->exec();
    t2 = Clock::now();
    if (rank ==0){
        std::cout << "Propagator vector field execution time: "
                  << std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count()
                  << " seconds" << std::endl;
    }

	prop->getGeometry()->getPatch()->write("deformed");

	bool error = false;

#if MIMMO_ENABLE_MPI
	delete partition;
#endif
	delete prop;
	return error;
}

// =================================================================================== //

int main( int argc, char *argv[] ) {

	BITPIT_UNUSED(argc);
	BITPIT_UNUSED(argv);

#if MIMMO_ENABLE_MPI
	MPI_Init(&argc, &argv);


#endif
	/**<Calling mimmo Test routines*/

	int val = test00001() ;

#if MIMMO_ENABLE_MPI


	MPI_Finalize();
#endif

	return val;
}
