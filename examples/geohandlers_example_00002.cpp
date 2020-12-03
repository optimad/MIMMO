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
#if MIMMO_ENABLE_MPI
#include "mimmo_parallel.hpp"
#endif

// =================================================================================== //
/*!
	\example geohandlers_example_00002.cpp

	\brief Example of usage of stitching block of two input geometry.

	Geometry handler block used: StitchGeometry.

	<b>To run</b>: ./geohandlers_example_00002 \n

	<b> visit</b>: <a href="http://optimad.github.io/mimmo/">mimmo website</a> \n
 */


// =================================================================================== //

void test00002() {

    /* Creation of mimmo containers.
     * Inputs and output MimmoGeometry are instantiated.
     */
	mimmo::MimmoGeometry * mimmo0 = new mimmo::MimmoGeometry(mimmo::MimmoGeometry::IOMode::CONVERT);
	mimmo0->setReadDir("geodata");
	mimmo0->setReadFilename("sphere2");
	mimmo0->setReadFileType(FileType::STL);
    mimmo0->setWriteDir("./");
    mimmo0->setWriteFileType(FileType::STL);
    mimmo0->setWriteFilename("geohandlers_output_00002.0000");

    mimmo::MimmoGeometry * mimmo1 = new mimmo::MimmoGeometry(mimmo::MimmoGeometry::IOMode::CONVERT);
    mimmo1->setReadDir("geodata");
    mimmo1->setReadFilename("stanfordBunny2");
    mimmo1->setReadFileType(FileType::STL);
    mimmo1->setWriteDir("./");
    mimmo1->setWriteFileType(FileType::STL);
    mimmo1->setWriteFilename("geohandlers_output_00002.0001");

    mimmo::MimmoGeometry * mimmo2 = new mimmo::MimmoGeometry(mimmo::MimmoGeometry::IOMode::WRITE);
    mimmo2->setWriteDir("./");
    mimmo2->setWriteFileType(FileType::STL);
    mimmo2->setWriteFilename("geohandlers_output_00002.0002");

#if MIMMO_ENABLE_MPI
    /* Instantiation of a Partition object with default partition method space filling curve.
     * Plot Optional results during execution active for Partition block.
     */
    mimmo::Partition* partition0 = new mimmo::Partition();
    partition0->setPlotInExecution(true);

    /* Instantiation of a Partition object with default patition method space filling curve.
     * Plot Optional results during execution active for Partition block.
     */
    mimmo::Partition* partition1 = new mimmo::Partition();
    partition1->setPlotInExecution(true);
#endif

    /* Instantiation of a Stitcher Geometry block.
     * Plot Optional results during execution active for Stitcher block.
     */
    mimmo::StitchGeometry * stitcher = new mimmo::StitchGeometry(1);
	stitcher->setPlotInExecution(true);
	stitcher->setOutputPlot(".");

    /* Setup pin connections.
     */
#if MIMMO_ENABLE_MPI
    mimmo::pin::addPin(mimmo0, partition0, M_GEOM, M_GEOM);
    mimmo::pin::addPin(partition0, stitcher, M_GEOM, M_GEOM);
    mimmo::pin::addPin(mimmo1, partition1, M_GEOM, M_GEOM);
    mimmo::pin::addPin(partition1, stitcher, M_GEOM, M_GEOM);
#else
    mimmo::pin::addPin(mimmo0, stitcher, M_GEOM, M_GEOM);
	mimmo::pin::addPin(mimmo1, stitcher, M_GEOM, M_GEOM);
#endif
	mimmo::pin::addPin(stitcher, mimmo2, M_GEOM, M_GEOM);

    /* Setup execution chain.
     */
	mimmo::Chain ch0;
	ch0.addObject(mimmo0);
	ch0.addObject(mimmo1);
#if MIMMO_ENABLE_MPI
    ch0.addObject(partition0);
    ch0.addObject(partition1);
#endif
	ch0.addObject(stitcher);
	ch0.addObject(mimmo2);

    /* Execution of chain.
     * Use debug flag true to to print out the execution steps.
     */
    ch0.exec(true);

    /* Clean up & exit;
     */
    delete mimmo0;
    delete mimmo1;
#if MIMMO_ENABLE_MPI
    delete partition0;
    delete partition1;
#endif
    delete mimmo2;
    delete stitcher;

    mimmo0 = NULL;
    mimmo1 = NULL;
#if MIMMO_ENABLE_MPI
    partition0 = NULL;
    partition1 = NULL;
#endif
    mimmo2 = NULL;
	stitcher = NULL;

	return;
}

// =================================================================================== //

int main( int argc, char *argv[] ) {

	BITPIT_UNUSED(argc);
	BITPIT_UNUSED(argv);

#if MIMMO_ENABLE_MPI
    MPI_Init(&argc, &argv);
#endif
		/**<Calling mimmo Test routines*/
        try{
            test00002() ;
        }
        catch(std::exception & e){
            std::cout<<"geohandlers_example_00002 exited with an error of type : "<<e.what()<<std::endl;
            return 1;
        }
#if MIMMO_ENABLE_MPI
	MPI_Finalize();
#endif

	return 0;
}
