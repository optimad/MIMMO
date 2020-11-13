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
#include "mimmo_iocgns.hpp"
#include "mimmo_geohandlers.hpp"
#include "mimmo_manipulators.hpp"
#include "mimmo_propagators.hpp"
#if MIMMO_ENABLE_MPI
#include "mimmo_parallel.hpp"
#endif

/*!
 * \example iocgns_example_00001.cpp
 *
 * \brief Morphing of a CGNS volume mesh, following step are performed:
         - reading a CGNS volume mesh
         - boundary extraction
         - definition of a surface deformation on boundaries
         - propagation of deformation field from boundaries to volume mesh bulk
         - application of deformation to volume mesh
         - write deformed mesh to CGNS file
 *
 * Using: IOCGNS, SelectionByPID, SelectionByBox, ReconstructVector, RotationGeometry, Apply, ExtractVectorFields
 *
 * Depends on mimmo optional module geohandlers
 *
 * <b>To run</b>: ./iocgns_example_00001 \n
 *
 * <b> visit</b>: <a href="http://optimad.github.io/mimmo/">mimmo website</a> \n
 */

// =================================================================================== //

void example00001() {

    /* Create IO_CGNS object to import input file. */
	mimmo::IOCGNS * cgnsI = new mimmo::IOCGNS(mimmo::IOCGNS::IOCGNS_Mode::READ);
    cgnsI->setDir("geodata");
    cgnsI->setFilename("grid");

    /* Create IO_CGNS object to export output file. */
    mimmo::IOCGNS * cgnsO = new mimmo::IOCGNS(mimmo::IOCGNS::IOCGNS_Mode::WRITE);
    cgnsO->setDir(".");
    cgnsO->setFilename("iocgns_output_00001");

#if MIMMO_ENABLE_MPI

    /* Instantiation of a Partition object with default patition method space filling curve.
     * Plot Optional results during execution active for Partition block.
     */
    mimmo::Partition *partition = new mimmo::Partition();
    partition->setPartitionMethod(mimmo::PartitionMethod::PARTGEOM);
    partition->setPlotInExecution(true);

    /* Instantiation of a Partition object to serialize mesh right before the writing cgns.
     */
    mimmo::Partition *serialize = new mimmo::Partition();
    serialize->setPartitionMethod(mimmo::PartitionMethod::SERIALIZE);
    serialize->setPlotInExecution(true);
#endif

    /* Create CGNS PID extractor object to test input file.
     * Extraction of PID = 1,2 (Wing wall and outer part boundaries
     * where imposing Dirichlet conditions).
     */
    mimmo::SelectionByPID * cgnsDirichlet = new mimmo::SelectionByPID();
    cgnsDirichlet->setPID({1, 2});
    cgnsDirichlet->setPlotInExecution(true);

    /* Create CGNS PID extractor object to test input file.
     * Extraction of PID = 3 (Simmetry plane
     * where imposing Slip/impermeability conditions).
     */
    mimmo::SelectionByPID * cgnsSlip = new mimmo::SelectionByPID();
    cgnsSlip->setPID({3});
    cgnsSlip->setPlotInExecution(true);

    /* Instantiation of a Selection By Box block.
     * Setup of span and origin of cube.
     */
    mimmo::SelectionByBox * boxSel = new mimmo::SelectionByBox();
    boxSel->setOrigin({{700., 800., 0.}});
    boxSel->setSpan(1500.,1600.,100.);
    boxSel->setPlotInExecution(true);

    /* Creation of rotation block.
     */
    mimmo::RotationGeometry* rotation = new mimmo::RotationGeometry();
    rotation->setDirection(darray3E{0.1,1.,0.});
    rotation->setRotation((BITPIT_PI/18.0));

    /* Create reconstruct vector block and set to reconstruct rotation
     * displacement field over the whole Dirichlet surface geometry
     */
    mimmo::ReconstructVector* recon = new mimmo::ReconstructVector();
    recon->setPlotInExecution(true);

    /* Create propagate vector block and set to propagate over the whole
     * input volume geometry the displacements field.
     */
    mimmo::PropagateVectorField* prop = new mimmo::PropagateVectorField();
    prop->setPlotInExecution(true);
    prop->setTolerance(1.0e-9);
    prop->setUpdateThreshold(1.0e-12);
    prop->setDamping(true);
    prop->setDampingType(0);
    prop->setDampingInnerDistance(0.01);
    prop->setDampingOuterDistance(80.0);
    prop->setDampingDecayFactor(3.0);
    prop->setNarrowBand(true);
    prop->setNarrowBandWidth(80.0);
    prop->setNarrowBandRelaxation(0.3);
    prop->forcePlanarSlip(true);
    prop->setSolverMultiStep(1);

    /* Create propagate vector block and set to propagate over the whole
     * input volume geometry the displacements field.
     */
    mimmo::ExtractVectorField* extrF = new mimmo::ExtractVectorField();
    extrF->setMode(1);
    extrF->setPlotInExecution(true);

    /* Create applier block.
     * It applies the deformation displacements
     * to the selected input volume geometry.
     */
    mimmo::Apply* applier = new mimmo::Apply();

    /* Create applier block.
     * It applies the deformation displacements
     * to the selected input surface geometry.
     */
    mimmo::Apply* applierS = new mimmo::Apply();

    /* Create PINs. */
#if MIMMO_ENABLE_MPI
    mimmo::pin::addPin(cgnsI, partition, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(cgnsI, partition, M_GEOM2, M_GEOM2)  ;
    mimmo::pin::addPin(partition, cgnsDirichlet, M_GEOM2, M_GEOM)  ;
    mimmo::pin::addPin(partition, cgnsSlip, M_GEOM2, M_GEOM)  ;
#else
    mimmo::pin::addPin(cgnsI, cgnsDirichlet, M_GEOM2, M_GEOM)  ;
    mimmo::pin::addPin(cgnsI, cgnsSlip, M_GEOM2, M_GEOM)  ;
#endif

    mimmo::pin::addPin(cgnsDirichlet, boxSel, M_GEOM, M_GEOM)  ;

    mimmo::pin::addPin(boxSel, rotation, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(rotation, recon, M_GDISPLS, M_VECTORFIELD)  ;
    mimmo::pin::addPin(cgnsDirichlet, recon, M_GEOM, M_GEOM)  ;

#if MIMMO_ENABLE_MPI
    mimmo::pin::addPin(partition, prop, M_GEOM, M_GEOM)  ;
#else
    mimmo::pin::addPin(cgnsI, prop, M_GEOM, M_GEOM)  ;
#endif
    mimmo::pin::addPin(cgnsDirichlet, prop, M_GEOM, M_GEOM2)  ;
    mimmo::pin::addPin(cgnsSlip, prop, M_GEOM, M_GEOM4)  ;
    mimmo::pin::addPin(boxSel, prop, M_GEOM, M_GEOM3)  ;
    mimmo::pin::addPin(boxSel, prop, M_GEOM, M_GEOM7)  ;

    mimmo::pin::addPin(recon, prop, M_VECTORFIELD, M_GDISPLS)  ;
    mimmo::pin::addPin(prop, applier, M_GDISPLS, M_GDISPLS)  ;
#if MIMMO_ENABLE_MPI
    mimmo::pin::addPin(partition, applier, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(partition, extrF, M_GEOM2, M_GEOM)  ;
    mimmo::pin::addPin(partition, applierS, M_GEOM2, M_GEOM)  ;
#else
    mimmo::pin::addPin(cgnsI, applier, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(cgnsI, extrF, M_GEOM2, M_GEOM)  ;
    mimmo::pin::addPin(cgnsI, applierS, M_GEOM2, M_GEOM)  ;
#endif

    mimmo::pin::addPin(prop, extrF, M_GDISPLS, M_VECTORFIELD)  ;
    mimmo::pin::addPin(extrF, applierS, M_VECTORFIELD, M_GDISPLS)  ;

#if MIMMO_ENABLE_MPI
    mimmo::pin::addPin(applier, serialize, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(applierS, serialize, M_GEOM, M_GEOM2)  ;
    mimmo::pin::addPin(serialize, cgnsO, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(serialize, cgnsO, M_GEOM2, M_GEOM2)  ;
#else
    mimmo::pin::addPin(applier, cgnsO, M_GEOM, M_GEOM)  ;
    mimmo::pin::addPin(applierS, cgnsO, M_GEOM, M_GEOM2)  ;
#endif

    mimmo::pin::addPin(cgnsI, cgnsO, M_BCCGNS, M_BCCGNS)  ;

    /* Create and execute chain. */
    mimmo::Chain ch0, ch1;
    ch0.addObject(cgnsI);
#if MIMMO_ENABLE_MPI
    ch0.addObject(partition);
#endif
    ch0.addObject(cgnsDirichlet);
    ch0.addObject(cgnsSlip);
    ch0.addObject(boxSel);
    ch0.addObject(rotation);
    ch0.addObject(recon);
    ch0.addObject(prop);
    ch0.addObject(extrF);
    ch0.addObject(applier);
    ch0.addObject(applierS);
    #if MIMMO_ENABLE_MPI
       ch0.addObject(serialize);
    #endif

    ch1.addObject(cgnsO);

    ch0.exec(true);
    ch1.exec(true);

    /* Destroy objects. */
    delete cgnsI;
#if MIMMO_ENABLE_MPI
    delete partition;
    delete serialize;
#endif
    delete cgnsDirichlet;
    delete cgnsSlip;
    delete boxSel;
    delete rotation;
    delete recon;
    delete prop;
    delete extrF;
    delete applier;
    delete applierS;
    delete cgnsO;

    return;
}

// =================================================================================== //

int main( int argc, char *argv[] ) {

    BITPIT_UNUSED(argc);
    BITPIT_UNUSED(argv);

#if MIMMO_ENABLE_MPI
    MPI_Init(&argc, &argv);

    {
#endif
        try{
            /**< Call mimmo example routine. */
            example00001();
        }
        catch(std::exception & e){
            std::cout<<"iocgns_example_00001 exited with an error of type : "<<e.what()<<std::endl;
            return 1;
        }
#if MIMMO_ENABLE_MPI
    }

    MPI_Finalize();
#endif

    return 0;
}
