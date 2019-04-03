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

#include "MeshSelection.hpp"
#include "levelSet.hpp"
#include <cstddef>
namespace mimmo {

/*!
 * Basic Constructor
 */
GenericSelection::GenericSelection(){
    m_type = SelectionType::UNDEFINED;
    m_topo = 1; /*default to surface geometry*/
    m_dual = false; /*default to exact selection*/
};

/*!
 * Basic Destructor
 */
GenericSelection::~GenericSelection(){
    m_subpatch.reset(nullptr);
};

/*!
 * Copy Constructor, any already calculated selection is not copied.
 */
GenericSelection::GenericSelection(const GenericSelection & other):BaseManipulation(other){
    m_type = other.m_type;
    m_topo = other.m_topo;
    m_dual = other.m_dual;
};

/*!
 * Copy operator, any already calculated selection is not copied.
 */
GenericSelection & GenericSelection::operator=(const GenericSelection & other){
    *(static_cast<BaseManipulation *>(this)) = *(static_cast<const BaseManipulation * >(&other));
    m_type = other.m_type;
    m_topo = other.m_topo;
    m_dual = other.m_dual;
    /*m_subpatch is not copied and it is obtained in execution*/
    return *this;
};

/*!
 * Swap function. Assign data of this class to another of the same type and vice-versa.
 * Resulting patch of selection is not swapped, ever.
 * \param[in] x GenericSelection object
 */
void GenericSelection::swap(GenericSelection & x) noexcept
{
    std::swap(m_type, x.m_type);
    std::swap(m_topo, x.m_topo);
    std::swap(m_dual, x.m_dual);
    BaseManipulation::swap(x);
}

/*!
 * It builds the input/output ports of the object
 */
void
GenericSelection::buildPorts(){

    bool built = true;

    built = (built && createPortIn<MimmoObject *, GenericSelection>(this, &GenericSelection::setGeometry, M_GEOM, true));
    built = (built && createPortIn<bool, GenericSelection>(this, &GenericSelection::setDual,M_VALUEB));

    built = (built && createPortOut<MimmoObject *, GenericSelection>(this, &GenericSelection::getPatch,M_GEOM));
    built = (built && createPortOut<livector1D, GenericSelection>(this, &GenericSelection::constrainedBoundary, M_VECTORLI));
    m_arePortsBuilt = built;
};


/*!
 * Return type of method for selection.
 * See SelectionType enum for available methods.
 * \return type of selection method
 */
SelectionType
GenericSelection::whichMethod(){
    return    m_type;
};

/*!
 * Return pointer by copy to sub-patch extracted by the class
 * \return pointer to MimmoObject extracted sub-patch
 */
MimmoObject*
GenericSelection::getPatch(){
    return    m_subpatch.get();
};

/*!
 * Return pointer by copy to subpatch extracted by the class [Const overloading].
 * \return pointer to MimmoObject extracted sub-patch
 */
const MimmoObject*
GenericSelection::getPatch() const{
    return    m_subpatch.get();
};

/*!
 * Set link to target geometry for your selection.
 * Reimplementation of mimmo::BaseManipulation::setGeometry();
 *  \param[in] target Pointer to MimmoObject with target geometry.
 */
void
GenericSelection::setGeometry( MimmoObject * target){
    if(target == NULL)  return;
    if(target->isEmpty()) return;
    m_geometry = target;
    /*set topology informations*/
    m_topo = target->getType();

};


/*!
 * Set your class behavior selecting a portion of a target geoemetry.
 * Given a initial set up, gets the dual
 * result (its negative) of current selection.
 * For instance, in a class extracting geometry inside the volume of an
 * elemental shape, gets all other parts of it not included in the shape.
 * For a class extracting portions of geometry
 * matching an external tessellation, gets all the other parts not matching it.
 * For a class extracting portion of
 * geometry identified by a PID list, get all other parts not marked by such list.
 * \param[in] flag Active/Inactive "dual" feature true/false.
 */
void
GenericSelection::setDual(bool flag ){
    m_dual = flag;
}

/*!
 * Return actual status of "dual" feature of the class. See setDual method.
 * \return  true/false for "dual" feature activated or not
 */
bool
GenericSelection::isDual(){
    return m_dual;
};

/*!
 * Return list of constrained boundary nodes (all those boundary nodes of
 * the subpatch extracted which are not part of the boundary of the mother
 * geometry)
 * \return list of boundary nodes ID, according to bitpit::PatchKernel indexing
 */
livector1D
GenericSelection::constrainedBoundary(){

    if(getGeometry() == NULL || getPatch() == NULL)    return livector1D(0);
    if(getGeometry()->isEmpty() || getPatch()->isEmpty())    return livector1D(0);
    std::unordered_map<long, std::set<int> > survivors;

    auto daughterBCells  = getPatch()->extractBoundaryFaceCellID();
    auto motherBCells = getGeometry()->extractBoundaryFaceCellID();

    //save all those daughter boundary faces not included in mother alse
    for(const auto & sT : daughterBCells){
        if(motherBCells.count(sT.first) > 0){
            for(const auto & val: sT.second){
                if(motherBCells[sT.first].count(val) > 0)   continue;
                survivors[sT.first].insert(val);
            }
        }else{
            survivors[sT.first] = daughterBCells[sT.first];
        }
    }
    motherBCells.clear();
    daughterBCells.clear();

    //get vertex of the cleaned daughter boundary.
    std::set<long> containerVert;
    for(const auto & sT  :survivors){
        const bitpit::Cell & cell = getPatch()->getPatch()->getCell(sT.first);
        for(const auto & val: sT.second){
            bitpit::ConstProxyVector<long> faceVertIds = cell.getFaceVertexIds(val);
            containerVert.insert(faceVertIds.begin(), faceVertIds.end());
        }
    }
    //move up in livector1D container.
    livector1D result;
    result.reserve(containerVert.size());
    result.insert(result.end(), containerVert.begin(), containerVert.end());

    return result;
};


/*!
 * Execute your object. A selection is extracted and trasferred in
 * an indipendent MimmoObject structure pointed by m_subpatch member
 */
void
GenericSelection::execute(){
    if(getGeometry() == NULL) {
//        throw std::runtime_error (m_name + " : NULL pointer to target geometry found");
        (*m_log)<<m_name + " : NULL pointer to target geometry found"<<std::endl;
        return;
    }
    if(getGeometry()->isEmpty()){
//        throw std::runtime_error (m_name + " : empty geometry linked");
        (*m_log)<<m_name + " : empty geometry linked"<<std::endl;
        return;
    };

    m_subpatch.reset(nullptr);

    livector1D extracted = extractSelection();

    if(extracted.empty()) {
//        throw std::runtime_error (m_name + " : empty selection performed. check block set-up");
        (*m_log)<<m_name + " : empty selection performed. check block set-up"<<std::endl;
        return;
    }

    /*Create subpatch.*/
    std::unique_ptr<MimmoObject> temp(new MimmoObject(m_topo));


    livector1D TT;
    bitpit::ElementType eltype;
    long PID;
    if (m_topo != 3){

        livector1D vertExtracted = getGeometry()->getVertexFromCellList(extracted);
        for(const auto & idV : vertExtracted){
            temp->addVertex(getGeometry()->getVertexCoords(idV), idV);
        }

        for(const auto & idCell : extracted){

            bitpit::Cell & cell = getGeometry()->getPatch()->getCell(idCell);
            eltype = cell.getType();
            PID = (long)cell.getPID();
            TT = getGeometry()->getCellConnectivity(idCell);
            temp->addConnectedCell(TT,eltype, PID, idCell);
            TT.clear();
        }
    }else{
        for(const auto & idV : extracted){
            temp->addVertex(getGeometry()->getVertexCoords(idV),idV);
        }
    }

    auto originalmap = getGeometry()->getPIDTypeListWNames();
    auto currentPIDmap = temp->getPIDTypeList();
    for(const auto & val: currentPIDmap){
        temp->setPIDName(val, originalmap[val]);
    }
    m_subpatch = std::move(temp);
};

/*!
 * Plot optional result of the class in execution. It plots the selected patch
 * as standard vtk unstructured grid.
 */
void
GenericSelection::plotOptionalResults(){
    if(getPatch() == NULL) return;
    if(getPatch()->isEmpty()) return;

    std::string dir = m_outputPlot;
    std::string name = m_name + "_Patch_"+ std::to_string(getId());

    if (m_topo != 3){

        std::string fullpath = dir + "/" + name ;
        getPatch()->getPatch()->write(fullpath);

    }else{

        liimap mapDataInv;
        dvecarr3E points = getPatch()->getVerticesCoords(&mapDataInv);
        ivector2D connectivity;
        bitpit::VTKElementType cellType = bitpit::VTKElementType::VERTEX;

        int np = points.size();
        connectivity.resize(np);
        for (int i=0; i<np; i++){
            connectivity[i].resize(1);
            connectivity[i][0] = i;
        }

        bitpit::VTKUnstructuredGrid output(dir,name,cellType);
        output.setGeomData( bitpit::VTKUnstructuredField::POINTS, points);
        output.setGeomData( bitpit::VTKUnstructuredField::CONNECTIVITY, connectivity);
        output.setDimensions(connectivity.size(), points.size());
        output.setCodex(bitpit::VTKFormat::APPENDED);
        output.write();
    }
}

}
