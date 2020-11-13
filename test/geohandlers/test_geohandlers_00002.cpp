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

#include "mimmo_geohandlers.hpp"

/*!
 * Creating surface triangular mesh and return it in a MimmoObject.
 * \param[in,out] mesh pointer to a MimmoObject mesh to fill.
 * \return true if successfully created mesh
 */
bool createMimmoMesh(mimmo::MimmoObject * mesh){

	double dx = 0.25, dy = 0.25;
	int nV, nC;
	//create vertexlist
	dvecarr3E vertex(35,{{0.0,0.0,0.0}});
	livector2D conn(48, livector1D(3));

	for(int i=0; i<7; ++i){
		for(int j=0; j<5; j++){
			nV = 5*i + j;
			vertex[nV][0] = i*dx;
			vertex[nV][1] = j*dy;
		}
	}

	for(int j=0; j<4; ++j){
		for(int i=0; i<3; ++i){
			nC = 8*i + 2*j;

			conn[nC][0] = 5*i + j;
			conn[nC][1] = 5*(i+1) + j;
			conn[nC][2] = 5*i + j+1;

			conn[nC+1][0] = 5*(i+1) + j;
			conn[nC+1][1] = 5*(i+1) + j+1;
			conn[nC+1][2] = 5*i + j+1;
		}
	}

	for(int j=0; j<4; ++j){
		for(int i=3; i<6; ++i){
			nC = 8*i + 2*j;

			conn[nC][0] = 5*i + j;
			conn[nC][1] = 5*(i+1) + j;
			conn[nC][2] = 5*(i+1) + j+1;

			conn[nC+1][0] = 5*i + j;
			conn[nC+1][1] = 5*(i+1) + j+1;
			conn[nC+1][2] = 5*i + j+1;
		}
	}

	mesh->getVertices().reserve(35);
	mesh->getCells().reserve(48);

	//fill the mimmoObject;
	long cV=0;
	for(auto & val: vertex){
		mesh->addVertex(val, cV);
		cV++;
	}

	long cC=0;
	bitpit::ElementType eltype = bitpit::ElementType::TRIANGLE;
	for(auto & val: conn){
		mesh->addConnectedCell(val, eltype, cC);
		cC++;
	}

	bool check = (mesh->getNCells() == 48) && (mesh->getNVertices() == 35);

	mesh->buildAdjacencies();

	return check;
}
// =================================================================================== //
/*!
 * Testing geohandlers module. Clipping, Selecting and Reconstructing Fields
 */
int test2() {

	//define 3 single triangle mesh
	mimmo::MimmoObject * m1 = new mimmo::MimmoObject(1);
	if(!createMimmoMesh(m1)){
		delete m1;
		return 1;
	}

	mimmo::ClipGeometry * clip = new mimmo::ClipGeometry();
	clip->setClipPlane({{0.75,0.5,0.0}}, {{1.0,0.0,0.0}});
	clip->setGeometry(m1);
	clip->setPlotInExecution(true);
	clip->exec();

	mimmo::ClipGeometry * clip2 = new mimmo::ClipGeometry();
	clip2->setInsideOut(true);
	clip2->setGeometry(m1);
	clip2->setClipPlane({{0.75,0.5,0.0}}, {{1.0,0.0,0.0}});
	clip2->setPlotInExecution(true);
	clip2->exec();

	mimmo::Lattice * latt1 = new mimmo::Lattice();
	latt1->setShape(mimmo::ShapeType::SPHERE);
	latt1->setOrigin({{1.5,0.5,0.0}});
	latt1->setSpan({{0.6,2.0*M_PI, M_PI}});
	latt1->setGeometry(clip->getClippedPatch());
	iarray3E dim = {{2,31,15}};
	latt1->setDimension(dim);
	latt1->setPlotInExecution(true);
	latt1->exec();

	mimmo::SelectionBySphere * sel1 = new mimmo::SelectionBySphere();
	sel1->setOrigin({{1.5,0.5,0.0}});
	sel1->setSpan({{0.6,2.0*M_PI, M_PI}});
	sel1->setDual(false);
	sel1->setGeometry(clip->getClippedPatch());
	sel1->setPlotInExecution(false);
	sel1->exec();

	mimmo::SelectionBySphere * sel2 = new mimmo::SelectionBySphere();
	sel2->setOrigin({{1.5,0.5,0.0}});
	sel2->setSpan({{0.6,2.0*M_PI, M_PI}});
	sel2->setGeometry(clip->getClippedPatch());
	sel2->setDual(true);
	sel2->setPlotInExecution(false);
	sel2->exec();

	mimmo::dmpvector1D field1(clip2->getClippedPatch());
	mimmo::dmpvector1D field2(sel1->getPatch());
	mimmo::dmpvector1D field3(sel2->getPatch());

	field1.setName("data1");
	field2.setName("data2");
	field3.setName("data3");

	for (auto vertex : clip2->getClippedPatch()->getVertices()){
		long int ID = vertex.getId();
		field1.insert(ID, 1.0);
	}

	for (auto vertex : sel1->getPatch()->getVertices()){
		long int ID = vertex.getId();
		field2.insert(ID, 1.0);
	}

	for (auto vertex : sel2->getPatch()->getVertices()){
		long int ID = vertex.getId();
		field3.insert(ID, 1.0);
	}

	mimmo::ReconstructScalar * recon = new mimmo::ReconstructScalar();

	recon->setGeometry(m1);
	recon->setOverlapCriterium(3);
	recon->addData(&field1);
	recon->addData(&field2);
	recon->addData(&field3);

	recon->exec();

	mimmo::MimmoPiercedVector<double> finalfield(*(recon->getResultField()));

	bool check= true;
	for(double & val : finalfield){
		check = check && (val == 1.0);
	}

	mimmo::SelectScalarField * swtch = new mimmo::SelectScalarField();

	swtch->setFields(recon->getResultFields());
	swtch->setGeometry(sel1->getPatch());
	swtch->setMode(mimmo::SelectType::GEOMETRY);
	swtch->setPlotInExecution(true);
	swtch->exec();

	mimmo::MimmoPiercedVector<double> * finalfieldswitch = swtch->getSelectedField();
	std::vector<double> vectorfield = finalfieldswitch->getDataAsVector();

	for(auto & val : vectorfield){
		check = check && (val == 1.0);
	}

	delete m1;
	delete clip;
	delete clip2;
	delete sel1;
	delete sel2;
	delete recon;
	delete swtch;

    std::cout<<"test passed :"<<check<<std::endl;

	return int(!check);
}

// =================================================================================== //

int main( int argc, char *argv[] ) {

	BITPIT_UNUSED(argc);
	BITPIT_UNUSED(argv);

#if MIMMO_ENABLE_MPI
	MPI_Init(&argc, &argv);
#endif
	/**<Calling mimmo Test routines*/
	int val = 1;
	try{
		val = test2() ;
	}
	catch(std::exception & e){
		std::cout<<"test_geohandlers_00002 exited with an error of type : "<<e.what()<<std::endl;
		return 1;
	}
#if MIMMO_ENABLE_MPI
	MPI_Finalize();
#endif

	return val;
}
