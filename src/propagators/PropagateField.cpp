
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

#include "PropagateField.hpp"

namespace mimmo {
//--------------------------------------
//--------------------------------------
// SCALARFIELD (USUALLY FILTER DISPLACEMENTS)
//--------------------------------------
//--------------------------------------

/*!
 * Constructor
 */
PropagateScalarField::PropagateScalarField():PropagateField<1>(){
    m_name = "mimmo.PropagateScalarField";
};

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
PropagateScalarField::PropagateScalarField(const bitpit::Config::Section & rootXML):PropagateField<1>(){

    m_name = "mimmo.PropagateScalarField";

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.PropagateScalarField"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!
 * Destructor;
 */
PropagateScalarField::~PropagateScalarField(){
    clear();
};

/*!
 * Copy constructor
 */
PropagateScalarField::PropagateScalarField(const PropagateScalarField & other):PropagateField<1>(other){};

/*!
 * Assignment operator of the class
 */
PropagateScalarField & PropagateScalarField::operator=(PropagateScalarField other){
    swap(other);
    return *this;
};

/*!
 * Swap function.
 * \param[in] x object to be swapped
 */
void PropagateScalarField::swap(PropagateScalarField & x) noexcept {
    PropagateField<1>::swap(x);
}

/*!
 * It builds the input/output ports of the object
 */
void
PropagateScalarField::buildPorts(){
    bool built = true;
    built = (built && createPortIn<dmpvector1D, PropagateScalarField>(this, &PropagateScalarField::setDirichletConditions, M_FILTER));
    built = (built && createPortOut<dmpvector1D, PropagateScalarField>(this, &PropagateScalarField::getPropagatedField, M_FILTER));
    PropagateField<1>::buildPorts();
    m_arePortsBuilt = built;
};

/*!
 * It gets the resulting propagated field on the whole bulk mesh.
 * \return Deformation field.
 */
dmpvector1D
PropagateScalarField::getPropagatedField(){
    dmpvector1D field;
    field.reserve(m_field.size());
    field.setDataLocation(m_field.getDataLocation());
    field.setGeometry(m_field.getGeometry());
    for(auto it = m_field.begin(); it != m_field.end(); ++it){
        field.insert(it.getId(), (*it)[0]);
    }

    return(field);
}

/*!
 * It sets the Dirichlet conditions for scalar field on the previously linked
 * Dirichlet Boundary patch.
 * \param[in] bc dirichlet conditions
 */
void
PropagateScalarField::setDirichletConditions(dmpvector1D bc){
    if (bc.isEmpty()) return;
    m_surface_bc_dir.reserve(bc.size());
    m_surface_bc_dir.setDataLocation(bc.getDataLocation());
    m_surface_bc_dir.setGeometry(bc.getGeometry());
    for(auto it = bc.begin(); it != bc.end(); ++it){
        m_surface_bc_dir.insert(it.getId(), std::array<double,1>({*it}));
    }
}


/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void PropagateScalarField::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){
    BITPIT_UNUSED(name);
    //start absorbing
    PropagateField<1>::absorbSectionXML(slotXML, name);
};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void PropagateScalarField::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);
    PropagateField<1>::flushSectionXML(slotXML, name);
};

/*!
 * Clear all data actually stored in the class
 */
void
PropagateScalarField::clear(){
    PropagateField<1>::clear();
};


/*!
 * Check coherence of the input data of the class, in particular:
 * - check if boundary patches are referred to target bulk mesh
 * - check if boundary conditions vectors are coherent with relative boundary patches.
 * In case of successfull check, the internal member m_ibsp is filled out.
 * \return true if coherence is satisfied, false otherwise.
 */
bool PropagateScalarField::checkBoundariesCoherence(){

    //Clean the old m_isbp and initialize it again.
    initializeBoundaryInfo();
    //1st step verify coherence of the Dirichlet point field on boundary surface
    // with the dirichlet boundary surface provided
    if(m_surface_bc_dir.getGeometry() != m_bsurface || !m_surface_bc_dir.completeMissingData({0.0})){
        return false;
    }

    //transfer point field info of boundary dirichlet on the volume mesh interfaces.
    MimmoPiercedVector<std::array<double,1>> temp;
    temp.setGeometry(m_geometry);
    temp.setDataLocation(MPVLocation::POINT);
    temp.reserve(m_surface_bc_dir.size());
    for(auto it=m_surface_bc_dir.begin(); it!=m_surface_bc_dir.end(); ++it){
        temp.insert(it.getId(), *it );
    }
    //interpolate now point data to interface data
    m_bc_dir.clear();
    m_bc_dir = temp.pointDataToBoundaryInterfaceData();

    //update the m_isbp marking dirichlet interfaces
    for(auto it=m_bc_dir.begin(); it != m_bc_dir.end(); ++it){
        m_isbp.at(it.getId()) = 1;
    }

    return true;
}

/*!
 * Plot optional results on vtu unstructured grid file
 */
void
PropagateScalarField::plotOptionalResults(){


    if(getGeometry() == NULL || getGeometry()->isEmpty())    return;

    bitpit::VTKUnstructuredGrid& vtk = getGeometry()->getPatch()->getVTK();

    std::vector<std::array<double,1> > dataraw = m_field.getDataAsVector();
    dvector1D data;
    data.reserve(dataraw.size());
    for (auto val : dataraw){
        data.push_back(val[0]);
    }
    vtk.addData("field", bitpit::VTKFieldType::SCALAR, bitpit::VTKLocation::POINT, data);

    dvector1D datad = m_dumping.getDataAsVector() ;
    vtk.addData("dumping", bitpit::VTKFieldType::SCALAR, bitpit::VTKLocation::CELL, datad);

    vtk.setCounter(getId());
    getGeometry()->getPatch()->write(m_name +"_field");
    vtk.removeData("field");
    vtk.removeData("dumping");
    vtk.unsetCounter();

};

/*!
 * Execution command. After the execution the result constraint field is stored in the class.
 */
void
PropagateScalarField::execute(){

    if(getGeometry() == NULL){
        (*m_log)<<"Error in "<<m_name<<" .No target volume mesh linked"<<std::endl;
        throw std::runtime_error("Error in PropagateScalarField execute. No target volume mesh linked");
    }

    if(m_bsurface == NULL ){
        (*m_log)<<"Error in "<<m_name<<" .No Dirichlet Boundary patch linked"<<std::endl;
        throw std::runtime_error("Error in PropagateScalarField execute. No Dirichlet Boundary patch linked");
    }

    if(!checkBoundariesCoherence()){
        (*m_log)<<"Error in "<<m_name<<" .Boundary patches linked are uncoherent with target bulk geometry"
        "or bc-fields not coherent with boundary patches"<<std::endl;
        throw std::runtime_error("Error in PropagateScalarField execute. Boundary patches linked are uncoherent"
        "with target bulk geometry or bc-fields not coherent with boundary patches");
    }
    //allocate the solver;
    m_solver = std::unique_ptr<bitpit::SystemSolver>(new bitpit::SystemSolver());

    //get this inverse map -> you will need it to compact the stencils.
    liimap dataInv = getGeometry()->getMapCellInv();
    //compute the dumping.
    computeDumpingFunction();
    //compute the gradient stencils @ interface with Neutral conditions.
    FVolStencil::MPVGradientUPtr faceGradients  = computeGradientStencilsWithNeutralBC();
    // compute the laplacian stencils ;
    FVolStencil::MPVDivergenceUPtr laplaceStencils = FVolStencil::computeFVLaplacianStencil(*(faceGradients.get()), &m_dumping);
    // initialize the laplacian Matrix in solver and release the laplace stencils.
    initializeLaplaceSolver(laplaceStencils.get(), dataInv);
    laplaceStencils = nullptr;

    //prepare the right hand side and release the faceGradients;
    dvector1D rhs(getGeometry()->getPatch()->getInternalCount(), 0.0);
    appendToRHSFromBorderFluxes(0, faceGradients.get(), dataInv, rhs);
    faceGradients = nullptr;
    dataInv.clear();

    //solve
    dvector1D result(rhs.size(), 0.0);
    solveLaplace(rhs, result);

    // push result in a mpv linked to target mesh and on cell location.
    MimmoPiercedVector<std::array<double,1> > tempres;
    tempres.setGeometry(m_geometry);
    tempres.setDataLocation(MPVLocation::CELL);
    tempres.reserve(result.size());
    livector1D cellmap = getGeometry()->getMapCell();
    int counter = 0;
    for(const double & val: result){
        tempres.insert(cellmap[counter], std::array<double,1>({val}));
        ++counter;
    }
    cellmap.clear();
    // interpolate result to POINT location.
    m_field = tempres.cellDataToPointData();

    //destroy the solver;
    m_solver = nullptr;
}


//--------------------------------------
//--------------------------------------
// VECTORFIELD (USUALLY GEOMETRY DISPLACEMENTS)
//--------------------------------------
//--------------------------------------

/*!
 * Constructor
 */
PropagateVectorField::PropagateVectorField():PropagateField<3>(){
    m_name = "mimmo.PropagateVectorField";
    m_nstep = 1;
    m_slipsurface = NULL;
    m_slipratio   = 100;

};

/*!
 * Set most significant parameters to constructor defaults
 */
void PropagateVectorField::setDefaults(){
    PropagateField<3>::setDefaults();
    m_nstep = 1;
    m_slipsurface = NULL;
    m_slipratio   = 100;
}

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
PropagateVectorField::PropagateVectorField(const bitpit::Config::Section & rootXML):PropagateField<3>(){

    m_name = "mimmo.PropagateVectorField";
    m_nstep = 1;
    m_slipsurface = NULL;
    m_slipratio   = 100;

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.PropagateVectorField"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!
 * Destructor;
 */
PropagateVectorField::~PropagateVectorField(){
    clear();
};

/*!
 * Copy constructor
 */
PropagateVectorField::PropagateVectorField(const PropagateVectorField & other):PropagateField<3>(other){
    m_slipsurface = other.m_slipsurface;
    m_nstep = other.m_nstep;
    m_slipratio = other.m_slipratio;
};

/*!
 * Assignment operator of the class
 */
PropagateVectorField & PropagateVectorField::operator=(PropagateVectorField other){
    swap(other);
    return *this;
};

/*!
 * Swap function.
 * \param[in] x object to be swapped
 */
void PropagateVectorField::swap(PropagateVectorField & x) noexcept {
    std::swap(m_slipsurface, x.m_slipsurface);
    std::swap(m_nstep, x.m_nstep);
    std::swap(m_slipratio, x.m_slipratio);
    PropagateField<3>::swap(x);
}

/*!
 * It builds the input/output ports of the object
 */
void
PropagateVectorField::buildPorts(){
    bool built = true;
    built = (built && createPortIn<dmpvecarr3E, PropagateVectorField>(this, &PropagateVectorField::setDirichletConditions, M_GDISPLS));
    built = (built && createPortIn<MimmoObject *, PropagateVectorField>(this, &PropagateVectorField::setSlipBoundarySurface, M_GEOM4));
    built = (built && createPortOut<dmpvecarr3E, PropagateVectorField>(this, &PropagateVectorField::getPropagatedField, M_GDISPLS));
    PropagateField<3>::buildPorts();
    m_arePortsBuilt = built;
};

/*!
 * It gets the resulting deformation field on points cloud.
 * \return Deformation field.
 */
dmpvecarr3E
PropagateVectorField::getPropagatedField(){
    return(m_field);
}

/*!
 * Sets the portion of boundary mesh relative to geometry target
 * that must be constrained component-wise with zero normal field throughout boundary surface.
 * This patch is optional. If nothing is linked, the relative boundary is
 * solved free of any conditions.
 * \param[in] surface Boundary patch.
 */
void
PropagateVectorField::setSlipBoundarySurface(MimmoObject* surface){
    if (surface == NULL)       return;
    if (surface->isEmpty())    return;
    if (surface->getType()!= 1 ) return;

    m_slipsurface = surface;

}

/*!
 * Sets a threshold > 1, meant to adjust defects in vertex-normals of candidate slipsurface. if the rate between
 * the maximum component of the normal and the candidate component a is greater then this value,
 * the candidate component will be set to 0, and the normal will be recalculated.
 * \param[in] thres threshold.
 */
void
PropagateVectorField::setSlipNormalRatio(double thres){
    m_slipratio = std::fmax(1.0, thres);
}

/*!
 * It sets the Dirichlet conditions for each component of the vector field on the previously linked
 * Dirichlet Boundary patch.
 * \param[in] bc dirichlet conditions
 */
void
PropagateVectorField::setDirichletConditions(dmpvecarr3E bc){
    if (bc.isEmpty()) return;
    m_surface_bc_dir = bc;
}

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void PropagateVectorField::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){
    BITPIT_UNUSED(name);
    //start absorbing
    PropagateField<3>::absorbSectionXML(slotXML, name);

    if(slotXML.hasOption("MultiStep")){
        std::string input = slotXML.get("MultiStep");
        input = bitpit::utils::string::trim(input);
        double value = 1.0;
        if(!input.empty()){
            std::stringstream ss(input);
            ss >> value;
        }
        unsigned int value2 = 1;
        if(value >= 0.0) value2 = value;
        setSolverMultiStep(value2);
    }

    if(slotXML.hasOption("SlipNormalRatio")){
        std::string input = slotXML.get("SlipNormalRatio");
        input = bitpit::utils::string::trim(input);
        double value = 100.0;
        if(!input.empty()){
            std::stringstream ss(input);
            ss >> value;
        }
        setSlipNormalRatio(value);
    }

};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void PropagateVectorField::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);
    PropagateField<3>::flushSectionXML(slotXML, name);
    slotXML.set("MultiStep", std::to_string(int(m_nstep)));
    slotXML.set("SlipNormalRatio", std::to_string(m_slipratio));
};


/*!
 * Clear all data actually stored in the class
 */
void
PropagateVectorField::clear(){
    PropagateField<3>::clear();
    setDefaults();
};

/*!
 * Check coherence of the input data of the class, in particular:
 * - check if boundary patches are referred to target bulk mesh
 * - check if boundary conditions vectors are coherent with relative boundary patches.
 * In case of successfull check, the internal member m_ibsp is filled out.
 * \return true if coherence is satisfied, false otherwise.
 */
bool PropagateVectorField::checkBoundariesCoherence(){

    // //initialize m_isbp
    // bitpit::PiercedVector<bitpit::Vertex> & pVtarget = m_geometry->getVertices();
    //
    // //1st step: verify boundary IDs of Dirichlet boundary patch and target are coherent
    // // and fill m_isbp with flag true and mark 1 for Dirichlet condition.
    // long id;
    // for(const auto & vert: m_bsurface->getVertices()){
    //     id= vert.getId();
    //     if(!pVtarget.exists(id)) {
    //         m_isbp.clear();
    //         return false;
    //     }
    //     m_isbp.insert(id, 1);
    // }
    //
    // //2nd step verify coherence of the Dirichlet field with boundary surface
    // if(m_bc_dir.getGeometry() != m_bsurface || !m_bc_dir.completeMissingData({{0.0, 0.0,0.0}})){
    //     m_isbp.clear();
    //     return false;
    // }
    //
    // // verify if optional Neumann condition are set. If not, exit with true condition
    // if(m_slipsurface == NULL)    return true; //ok
    //
    // //neumann surface is not null, double step check as m_bsurface
    // //3rd step: verify boundary IDs of Neumann boundary patch and target are coherent
    // // and fill m_isbp with flag true and mark 2 for Neumann condition.
    // for(const auto & vert: m_slipsurface->getVertices()){
    //     id= vert.getId();
    //     if(!pVtarget.exists(id)) {
    //         m_isbp.clear();
    //         return false;
    //     }
    //     if( !(m_isbp.exists(id)) ){
    //         m_isbp.insert(id, 2);
    //     }
    // }
    //
    //
    // m_vNormals.clear();
    // m_vNormals.reserve(m_slipsurface->getNVertex());
    //
    // bitpit::ConstProxyVector<long> verts;
    // std::size_t size;
    // long idN;
    // //save the vertex Normals using the boundary surface m_slipsurface;
    // //check m_slipratio stuff and correct accordingly the normals.
    //
    // bitpit::SurfaceKernel* skernel = static_cast<bitpit::SurfaceKernel*>(m_slipsurface->getPatch());
    //
    // for(const auto & cell: m_slipsurface->getCells()){
    //     verts= cell.getVertexIds();
    //     size = verts.size();
    //     for(std::size_t i=0; i<size; ++i){
    //         idN = verts[i];
    //         if(m_isbp[idN]==2 && !m_vNormals.exists(idN)){
    //             m_vNormals.insert(idN, skernel->evalVertexNormal(cell.getId(), i));
    //
    //             int comp = 0;
    //             if(std::abs(m_vNormals[idN][1]) > std::abs(m_vNormals[idN][comp])) comp = 1;
    //             if(std::abs(m_vNormals[idN][2]) > std::abs(m_vNormals[idN][comp])) comp = 2;
    //
    //             if(m_slipratio * std::abs(m_vNormals[idN][(comp+1)%3]/m_vNormals[idN][comp]) < 1.0)   m_vNormals[idN][(comp+1)%3] = 0.0;
    //             if(m_slipratio * std::abs(m_vNormals[idN][(comp+2)%3]/m_vNormals[idN][comp]) < 1.0)   m_vNormals[idN][(comp+2)%3] = 0.0;
    //
    //             m_vNormals[idN] /= norm2(m_vNormals[idN]);
    //         }
    //     }
    // }
    //
    // //if it is survived, then it's all ok.
    return true;
}

/*!
 * Plot optional results on vtu unstructured grid file
 */
void
PropagateVectorField::plotOptionalResults(){


    if(getGeometry() == NULL || getGeometry()->isEmpty())    return;

    bitpit::VTKUnstructuredGrid& vtk = getGeometry()->getPatch()->getVTK();
    dvecarr3E data(m_field.size());
    int count = 0;
    for (auto val : m_field){
        data[count] = val;
        count++;
    }
    vtk.addData("field", bitpit::VTKFieldType::VECTOR, bitpit::VTKLocation::POINT, data);

    dvector1D datad(m_dumping.size());
    count = 0;
    for (auto val : m_dumping){
        datad[count] = val;
        count++;
    }
    vtk.addData("dumping", bitpit::VTKFieldType::SCALAR, bitpit::VTKLocation::POINT, datad);

    vtk.setCounter(getId());
    getGeometry()->getPatch()->write(m_name +"_field");
    vtk.removeData("field");
    vtk.removeData("dumping");
    vtk.unsetCounter();

};

/*!
 * Directly apply deformation field to target geometry.
 */
void
PropagateVectorField::apply(){
    if (getGeometry() == NULL) return;
    if (getGeometry()->isEmpty() || m_field.isEmpty()) return;
    darray3E vertexcoords;
    long int ID;
    for (const auto & vertex : m_geometry->getVertices()){
        vertexcoords = vertex.getCoords();
        ID = vertex.getId();
        vertexcoords += m_field[ID];
        getGeometry()->modifyVertex(vertexcoords, ID);
    }
}

/*!
 * Force solver to get deformation in a finite number of substep
 * \param[in] sstep number of substep. Default is 1.
 */
void
PropagateVectorField::setSolverMultiStep(unsigned int sstep){
    unsigned int loc(1);
    m_nstep = std::max(loc,sstep);
}

/*!
 * subdivide dirichlet boundary conditions  for multi step purposes
 */
void
PropagateVectorField::subdivideBC(){
    for (auto & val : m_bc_dir){
        val /= double(m_nstep);
    }
}

/*!
 * restore dirichlet boundary conditions for multi step purposes
 */
void
PropagateVectorField::restoreBC(){
    for (auto & val : m_bc_dir){
        val *= double(m_nstep);
    }
}

/*!
 * restore geometry to target vertices
 * \param[in] list of vertices to be restored
 */

void
PropagateVectorField::restoreGeometry(bitpit::PiercedVector<bitpit::Vertex> & vertices){

    for (bitpit::Vertex & vertex : vertices){
        long ID = vertex.getId();
        m_field[ID] = getGeometry()->getVertexCoords(ID) - vertex.getCoords();
        getGeometry()->modifyVertex(vertex.getCoords(), ID);
    }

}

/*!
 * Execution command. After the execution the result constraint field is stored in the class.
 */
void
PropagateVectorField::execute(){

    if(getGeometry() == NULL){
        (*m_log)<<"Error in "<<m_name<<" .No target volume mesh linked"<<std::endl;
        throw std::runtime_error("Error in PropagateVectorField execute. No target volume mesh linked");
    }

    if(m_bsurface == NULL ){
        (*m_log)<<"Error in "<<m_name<<" .No Dirichlet Boundary patch linked"<<std::endl;
        throw std::runtime_error("Error in PropagateVectorField execute. No Dirichlet Boundary patch linked");
    }

    if(!checkBoundariesCoherence()){
        (*m_log)<<"Error in "<<m_name<<" .Boundary patches linked are uncoherent with target bulk geometry"
        "or bc-fields not coherent with boundary patches"<<std::endl;
        throw std::runtime_error("Error in PropagateVectorField execute. Boundary patches linked are uncoherent"
        "with target bulk geometry or bc-fields not coherent with boundary patches");
    }

//     livector2D stencils;
//     dvector2D weights;
//     dvector1D rhs;
//     liimap dataInv = getGeometry()->getMapDataInv();
//
//     if (m_laplace){
//         bitpit::PiercedVector<bitpit::Vertex> vertices0;
//         if (m_nstep > 1){
//             subdivideBC();
//             vertices0      = getGeometry()->getVertices();
//         }
//
//
//         for(int istep=0; istep<m_nstep; istep++){
//             //TODO need to provide an updater for stencils weights and dumping function to speed up the multi-step stage.
//             computeDumpingFunction();
//             computeStencils(dataInv, stencils, weights);
//             correctStencils(dataInv, stencils, weights);
//             computeRHS(m_bc_dir, dataInv, rhs);
//
//             solveLaplace(stencils, weights, rhs, dataInv, m_field);
//
//             if (m_nstep > 1){
//                 apply();
//                 if(m_dumpingActive){
//                     //update vertices of candidate dumping surface
//                     MimmoObject * dumptarget = m_dsurface;
//                     if(dumptarget == NULL) dumptarget = m_bsurface;
//                     for(const auto & vert: dumptarget->getVertices()){
//                         dumptarget->modifyVertex(getGeometry()->getVertexCoords(vert.getId()), vert.getId());
//                     }
//                 }
//                 (*m_log)<<"                        "<<m_name<<" performing substep :"<<std::to_string(istep+1)<<std::endl;
//
//             }
//         }//end loop step
//
//         if (m_nstep > 1){
//             restoreGeometry(vertices0);
//             if(m_dumpingActive){
//                 //update vertices of candidate dumping surface
//                 MimmoObject * dumptarget = m_dsurface;
//                 if(dumptarget == NULL) dumptarget = m_bsurface;
//                 for(const auto & vert: dumptarget->getVertices()){
//                     dumptarget->modifyVertex(getGeometry()->getVertexCoords(vert.getId()), vert.getId());
//                 }
//             }
//         }
//
//         restoreBC();
//
//     }else{
//         computeDumpingFunction();
//         computeStencils(dataInv, stencils, weights);
//         correctStencils(dataInv, stencils, weights);
//         computeRHS(m_bc_dir, dataInv, rhs);
//
//         solveSmoothing(m_sstep, stencils, weights, rhs, dataInv, m_field);
//     }
}

} //end of mimmo namespace
