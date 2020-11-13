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

#include "StitchGeometry.hpp"

namespace mimmo{

/*!
 * Default constructor of StitchGeometry.
 * Format admissible are linked to your choice of topology. See MimmoObject documentation.
 * In case of unsuitable topology, default format(3D-surface) is forced
 * \param[in] topo    Topology of your geometries (1-surface, 2-volume, 3-points cloud, 4-3DCurve)
 */
StitchGeometry::StitchGeometry(int topo){
    m_name         = "mimmo.StitchGeometry";
    m_geocount = 0;
    m_topo     = std::min(1, topo);
    if(m_topo > 4)    m_topo = 1;
    m_repid = false;
}

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
StitchGeometry::StitchGeometry(const bitpit::Config::Section & rootXML){

    std::string fallback_name = "ClassNONE";
    std::string fallback_topo = "-1";
    std::string input_name = rootXML.get("ClassName", fallback_name);
    std::string input_topo = rootXML.get("Topology", fallback_topo);
    input_name = bitpit::utils::string::trim(input_name);
    input_topo = bitpit::utils::string::trim(input_topo);

    int topo = std::stoi(input_topo);
    m_topo = std::max(1,topo);
    if (m_topo >4) m_topo = 1;
    m_geocount = 0;
    m_name = "mimmo.StitchGeometry";
    m_repid = false;

    if(input_name == "mimmo.StitchGeometry"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!
 * Default destructor of StitchGeometry.
 */
StitchGeometry::~StitchGeometry(){
    clear();
};

/*!
 * Copy constructor of StitchGeometry.
 * Soft Copy of MimmoObject, division maps and resulting patch are not copied;
 * Relaunch the execution to eventually restore it.
 */
StitchGeometry::StitchGeometry(const StitchGeometry & other):BaseManipulation(other){
    m_topo = other.m_topo;
    m_extgeo = other.m_extgeo;
    m_geocount = other.m_geocount;
    m_patch.reset(nullptr);
    m_repid = other.m_repid;
};

/*!
 * Assignement operator of StitchGeometry. Soft copy of MimmoObject
 */
StitchGeometry & StitchGeometry::operator=(StitchGeometry other){
    swap(other);
    return *this;
};


/*!
 * Swap function of StitchGeometry. Division Maps and results are not swapped, ever.
 * \param[in] x object to be swapped.
 */
void StitchGeometry::swap(StitchGeometry & x ) noexcept
{
    std::swap(m_topo,x.m_topo);
    std::swap(m_extgeo, x.m_extgeo);
    std::swap(m_geocount, x.m_geocount);
    std::swap(m_repid, x.m_repid);
    BaseManipulation::swap(x);
};


/*!
 * Building ports of the class
 */
void
StitchGeometry::buildPorts(){
    bool built = true;

    built = (built && createPortIn<MimmoObject*, StitchGeometry>(this, &mimmo::StitchGeometry::addGeometry, M_GEOM, true));

    built = (built && createPortOut<MimmoObject*, StitchGeometry>(this, &mimmo::StitchGeometry::getGeometry, M_GEOM));
    m_arePortsBuilt = built;
}

/*!
 * Return kind of topology supported by the object.
 * \return topology supported (1-surface, 2-volume, 3-points cloud)
 */
int
StitchGeometry::getTopology(){
    return m_topo;
}

/*!
 * Get current stitched geometry pointer. Reimplementation of BaseManipulation::getGeometry,
 * \return Pointer to stitched geometry.
 */
MimmoObject *
StitchGeometry::getGeometry(){
    return m_patch.get();
}

/*!
 * Add an external geometry to be stitched.Topology of the geometry must be coeherent
 * with topology of the class;
 * \param[in] geo  Pointer to MimmoObject
 */
void
StitchGeometry::addGeometry(MimmoObject* geo){
    if(geo == NULL) return;
    if(geo->getType() != m_topo)    return;
    if(m_extgeo.count(geo)    > 0)    return;

    m_extgeo.insert(std::make_pair(geo,m_geocount) );
    m_geocount++;
};

/*!
 * Check if stitched geometry is present or not.
 * \return true - no geometry present, false otherwise.
 */
bool
StitchGeometry::isEmpty(){
    return m_patch->isEmpty();
}

/*!
 * Clear all stuffs in your class
 */
void
StitchGeometry::clear(){
    m_extgeo.clear();
    m_geocount = 0;
    m_patch.reset(nullptr);
    BaseManipulation::clear();
};

/*!
 * Force automatic repidding of all the patch stitched. If a patch has pid already, they will be
 * kept, with different pid number.
 */
void
StitchGeometry::forceRePID(bool flag){
    m_repid = flag;
};

/*!Execution command.
 * It stitches together multiple geometries in the same object.
 */
void
StitchGeometry::execute(){
    if(m_extgeo.empty()){
        (*m_log)<<m_name + " : no source geometries to stich were found"<<std::endl;
    }

    std::unique_ptr<MimmoObject> dum(new MimmoObject(m_topo));
#if MIMMO_ENABLE_MPI
    if(m_nprocs > 1){
        //TODO you need a strategy to stitch together partioned mesh, keeping a unique id
        //throughout cells and ids. For example, communicate the number of Global cells and Global verts
        //(or min/max ids) of each patch to all communicators, and using them to organize offsets
        //for safe inserting elements.
        (*m_log)<<"WARNING "<< m_name<<" : stitching not available yet for MPI version with procs > 1"<<std::endl;
        m_patch = std::move(dum);
        return;
    }
#endif

    long nCells = 0;
    long nVerts = 0;

    for(auto &obj : m_extgeo){
        nCells += obj.first->getNCells();
        nVerts += obj.first->getNVertices();
    }

    //reserving memory
    dum->getPatch()->reserveVertices(nVerts);
    dum->getPatch()->reserveCells(nCells);

    long cV = 0, cC = 0;

    std::unordered_map<MimmoObject*,long>    map_pidstart;
    //initialize map
    for(auto & obj : m_extgeo){
        map_pidstart[obj.first] = 0;
    }

    if(m_repid){
        long pidmax = -1;
        for(auto & obj : m_extgeo){
            auto pidlist = obj.first->getPIDTypeList();
            std::vector<long> temp(pidlist.begin(), pidlist.end());
            std::sort(temp.begin(), temp.end());
            map_pidstart[obj.first] = pidmax + 1;
            pidmax += (*(temp.rbegin())+1);
        }
    }


    //start filling your stitched object.
    {
        //optional vars;
        long vId, cId;
        long PID;
        bitpit::ElementType eltype;

        for(auto &obj : m_extgeo){

            std::unordered_map<long,long> mapVloc;
            auto originalPidNames = obj.first->getPIDTypeListWNames();
            std::unordered_map<long, long> newOldPid;
            for(const auto & val : obj.first->getPIDTypeList()){
                newOldPid[val + map_pidstart[obj.first]] = val;
            }
            //start extracting/reversing vertices of the current obj
            for(const auto & vv : obj.first->getVertices()){
                vId = vv.getId();
                dum->addVertex(obj.first->getVertexCoords(vId), cV);
                //update map;
                mapVloc[vId] = cV;
                cV++;
            }

            //start extracting/reversing cells of the current obj
            for(const auto & cc : obj.first->getCells()){
                cId = cc.getId();
                PID = cc.getPID() + map_pidstart[obj.first];
                eltype = cc.getType();
                //get the local connectivity and update with new vertex numbering;
                livector1D conn = obj.first->getCellConnectivity(cId);
                livector1D connloc(conn.size());

                if(eltype == bitpit::ElementType::POLYGON){
                    std::size_t size = conn.size();
                    connloc[0] = conn[0];
                    for(std::size_t i = 1; i < size; ++i){
                        connloc[i] = mapVloc[conn[i]];
                    }

                }else if(eltype == bitpit::ElementType::POLYHEDRON){
                    connloc[0] = conn[0];
                    for(int nF = 0; nF < conn[0]-1; ++nF){
                        int facePos = cc.getFaceStreamPosition(nF);
                        int beginVertexPos = facePos + 1;
                        int endVertexPos   = facePos + 1 + conn[facePos];
                        connloc[facePos] = conn[facePos];
                        for (int i=beginVertexPos; i<endVertexPos; ++i){
                            connloc[i] = mapVloc[conn[i]];
                        }
                    }
                }else{
                    int ic = 0;
                    for (const auto & v : conn){
                        connloc[ic] = mapVloc[v];
                        ++ic;
                    }
                }
                dum->addConnectedCell(connloc, eltype, PID, cC);
                //update map;
                cC++;
            }

            dum->resyncPID();
            for(const auto & val: dum->getPIDTypeList()){
                dum->setPIDName(val, originalPidNames[newOldPid[val]]);
            }
        }
    }//scope for optional vars;

    m_patch = std::move(dum);
    m_patch->cleanGeometry();
}

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void StitchGeometry::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    std::string input;

    //checking topology
    if(slotXML.hasOption("Topology")){
        std::string input = slotXML.get("Topology");
        input = bitpit::utils::string::trim(input);
        int temptop = -1;
        if(!input.empty()){
            std::stringstream ss(input);
            ss>>temptop;
        }
        if(m_topo != temptop){
            (*m_log)<<"Error in "<<m_name<<" : class Topology parameter mismatch from what is read in XML input"<<std::endl;
            throw std::runtime_error(m_name + " : xml absorbing failed");
        }
    }

    BaseManipulation::absorbSectionXML(slotXML, name);


    if(slotXML.hasOption("RePID")){
        std::string input = slotXML.get("RePID");
        input = bitpit::utils::string::trim(input);
        bool temp = false;
        if(!input.empty()){
            std::stringstream ss(input);
            ss>>temp;
        }
        forceRePID(temp);
    }


};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void StitchGeometry::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    BaseManipulation::flushSectionXML(slotXML, name);
    slotXML.set("Topology", m_topo);
    slotXML.set("RePID", std::to_string(int(m_repid)));

};

/*!
 * Plot stitched geometry in *.vtu file as optional result;
 */
void
StitchGeometry::plotOptionalResults(){

	write(getGeometry());

}

}
