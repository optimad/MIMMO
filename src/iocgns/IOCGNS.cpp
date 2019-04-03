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

#include "IOCGNS.hpp"
#include <cgnslib.h>
#include <unordered_map>

using namespace std;
using namespace bitpit;

namespace mimmo{

/*!
 * Default constructor of IOCGNS.
 */
IOCGNS::IOCGNS(bool read){
    setDefaults();
    m_read  = read;
    m_write = !read;
}

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
IOCGNS::IOCGNS(const bitpit::Config::Section & rootXML){

    m_name = "mimmo.IOCGNS";
    setDefaults();

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.IOCGNS"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!
 * Default destructor of IOCGNS.
 */
IOCGNS::~IOCGNS(){};

/*!
 * Copy constructor of IOCGNS.
 */
IOCGNS::IOCGNS(const IOCGNS & other):BaseManipulation(other){
    m_read = other.m_read;
    m_write = other.m_write;
    m_rdir = other.m_rdir;
    m_rfilename = other.m_rfilename;
    m_wdir = other.m_wdir;
    m_wfilename = other.m_wfilename;
    m_surfmesh_not = other.m_surfmesh_not;

    m_storedInfo = std::move(std::unique_ptr<InfoCGNS>(new InfoCGNS(*(other.m_storedInfo.get()))));
    m_storedBC = std::move(std::unique_ptr<BCCGNS>(new BCCGNS(*(other.m_storedBC.get()))));
};

/*!
 * Assignement operator of IOCGNS.
 */
IOCGNS & IOCGNS::operator=(IOCGNS other){
    swap(other);
    return *this;
};

/*!
 * Swap function.
 * \param[in] x object to be swapped
 */
void IOCGNS::swap(IOCGNS & x) noexcept
{
    std::swap(m_read, x.m_read);
    std::swap(m_write, x.m_write);
    std::swap(m_rdir, x.m_rdir);
    std::swap(m_rfilename, x.m_rfilename);
    std::swap(m_wdir, x.m_wdir);
    std::swap(m_wfilename, x.m_wfilename);
    std::swap(m_surfmesh_not, x.m_surfmesh_not);

    std::swap(m_volmesh, x.m_volmesh);
    std::swap(m_surfmesh, x.m_surfmesh);
    std::swap(m_storedInfo, x.m_storedInfo);
    std::swap(m_storedBC, x.m_storedBC);

    BaseManipulation::swap(x);
}

/*!
 * Default values for IOCGNS.
 */
void
IOCGNS::setDefaults(){

    m_name      = "mimmo.IOCGNS";
    m_read      = false;
    m_rfilename = "mimmoVolCGNS";
    m_write     = false;
    m_wfilename = "mimmoVolCGNS";
    m_rdir      = "./";
    m_wdir      = "./";
    m_surfmesh_not = NULL;
    m_storedInfo = std::move(std::unique_ptr<InfoCGNS>(new InfoCGNS()));
    m_storedBC  = std::move(std::unique_ptr<BCCGNS>(new BCCGNS()));

    //Fill converters
    m_storedInfo->mcg_typeconverter[bitpit::ElementType::TRIANGLE] = CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(TRI_3);//CG_ElementType_t::CG_TRI_3;
    m_storedInfo->mcg_typeconverter[bitpit::ElementType::QUAD] = CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(QUAD_4);//CG_ElementType_t::CG_QUAD_4;
    m_storedInfo->mcg_typeconverter[bitpit::ElementType::TETRA] = CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(TETRA_4);//CG_ElementType_t::CG_TETRA_4;
    m_storedInfo->mcg_typeconverter[bitpit::ElementType::PYRAMID] = CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(PYRA_5);//CG_ElementType_t::CG_PYRA_5;
    m_storedInfo->mcg_typeconverter[bitpit::ElementType::WEDGE] = CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(PENTA_6);//CG_ElementType_t::CG_PENTA_6;
    m_storedInfo->mcg_typeconverter[bitpit::ElementType::HEXAHEDRON] = CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(HEXA_8); //CG_ElementType_t::CG_HEXA_8;

    // m_storedInfo->mcg_typetostring[CG_ElementType_t::CG_TRI_3] = "Elem_tri";
    // m_storedInfo->mcg_typetostring[CG_ElementType_t::CG_QUAD_4] = "Elem_quad";
    // m_storedInfo->mcg_typetostring[CG_ElementType_t::CG_TETRA_4] = "Elem_tetra";
    // m_storedInfo->mcg_typetostring[CG_ElementType_t::CG_PYRA_5] = "Elem_pyra";
    // m_storedInfo->mcg_typetostring[CG_ElementType_t::CG_PENTA_6] = "Elem_prism";
    // m_storedInfo->mcg_typetostring[CG_ElementType_t::CG_HEXA_8] = "Elem_hexa";
    m_storedInfo->mcg_typetostring[CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(TRI_3)] = "Elem_tri";
    m_storedInfo->mcg_typetostring[CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(QUAD_4)] = "Elem_quad";
    m_storedInfo->mcg_typetostring[CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(TETRA_4)] = "Elem_tetra";
    m_storedInfo->mcg_typetostring[CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(PYRA_5)] = "Elem_pyra";
    m_storedInfo->mcg_typetostring[CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(PENTA_6)] = "Elem_prism";
    m_storedInfo->mcg_typetostring[CGNS_ENUMT(ElementType_t)::CGNS_ENUMV(HEXA_8)] = "Elem_hexa";

}

/*!
 * It builds the input/output ports of the object
 */
void
IOCGNS::buildPorts(){

    bool built = true;
    built = (built && createPortIn<MimmoObject*, IOCGNS>(this, &IOCGNS::setGeometry, M_GEOM));
    built = (built && createPortIn<MimmoObject*, IOCGNS>(this, &IOCGNS::setSurfaceBoundary, M_GEOM2));
    built = (built && createPortIn<BCCGNS*, IOCGNS>(this, &IOCGNS::setBoundaryConditions, M_BCCGNS));

    built = (built && createPortOut<MimmoObject*, IOCGNS>(this, &IOCGNS::getGeometry, M_GEOM));
    built = (built && createPortOut<MimmoObject*, IOCGNS>(this, &IOCGNS::getSurfaceBoundary, M_GEOM2));
    built = (built && createPortOut<BCCGNS*, IOCGNS>(this, &IOCGNS::getBoundaryConditions, M_BCCGNS));
    m_arePortsBuilt = built;
};

/*!
 * Return all the surface bounding the current volume mesh.
 * During reading mode returns info contained in cgns, marking with PID all the possible boundary patches,
 * while in writing mode refers to the actual object pointed by the User.
 * \return pointer to surface boundary mesh
 */
MimmoObject*
IOCGNS::getSurfaceBoundary(){
    if(m_read)    return m_surfmesh.get();
    else          return m_surfmesh_not;
}

/*!
 * Return current pointer to geometry.If read mode return local read volumetric mesh, else
 * if in write mode return pointed externally geometry
 * \return pointer to volume mesh
 */
MimmoObject*
IOCGNS::getGeometry(){
    if(m_read) return m_volmesh.get();
    else       return BaseManipulation::getGeometry();
}

/*!
 * Return the Info object (class BCCGNS) for boundary conditions of surface patches
 * as PIDs associated to Surface boundary mesh.
 * Meaningful only in reading mode.
 * \return pointer to BCCGNS object with boundary conditions information read from file.
 */
BCCGNS*
IOCGNS::getBoundaryConditions(){
    return m_storedBC.get();
};

/*!
 * Return true if current class is in reading mode
 * \return reading mode flag
 */
bool
IOCGNS::isReadingMode(){
    return m_read;
}

/*!
 * Return true if current class is in writing mode
 * \return writing mode flag
 */
bool
IOCGNS::isWritingMode(){
    return m_write;
}

/*!It sets the condition to read the geometry on file during the execution.
 * \param[in] read Does it read the geometry in execution?
 */
void
IOCGNS::setRead(bool read){
    m_read = read;
    m_write = !read;
}

/*!It sets the name of directory to read the geometry.
 * If writing mode is on, set the path to read the template cgns file
 * \param[in] dir Name of directory.
 */
void
IOCGNS::setReadDir(string dir){
    m_rdir = dir;
}

/*!It sets the name of file to read the geometry.
 * If writing mode is on, set the filename where read the template cgns file
 * \param[in] filename Name of input file.
 */
void
IOCGNS::setReadFilename(string filename){
    m_rfilename = filename;
}

/*!It sets the condition to write the geometry on file during the execution.
 * \param[in] write Does it write the geometry in execution?
 */
void
IOCGNS::setWrite(bool write){
    setRead(!write);
}

/*!It sets the name of directory to write the geometry.
 * \param[in] dir Name of directory.
 */
void
IOCGNS::setWriteDir(string dir){
    m_wdir = dir;
}

/*!It sets the name of file to write the geometry.
 * \param[in] filename Name of output file.
 */
void
IOCGNS::setWriteFilename(string filename){
    m_wfilename = filename;
}

/*!
 * Set current geometry to an external volume mesh.
 * \param[in] geo Pointer to input volume mesh.
 */
void
IOCGNS::setGeometry(MimmoObject * geo){
    if(geo == NULL || m_read)    return;
    if(geo->isEmpty())          return;
    if(geo->getType() != 2)    return;
    BaseManipulation::setGeometry(geo);
}

/*!
 * Set boundary surface relative to the volume mesh.Option active only in writing mode.
 * Pre-existent boundary surfaces read from file, when class is set in read mode,  will be erased.
 * \param[in] geosurf Pointer to input volume mesh.
 */
void
IOCGNS::setSurfaceBoundary(MimmoObject* geosurf){
    if(geosurf == NULL || m_read)    return;
    if(geosurf->isEmpty())           return;
    if(geosurf->getType() != 1)      return;

    m_surfmesh_not = geosurf;
};

/*!
 * Set the Info object (class BCCGNS) for boundary conditions of surface patches
 * as PIDs associated to Surface boundary mesh.
 * Meaningful only in writing mode.
 * \param[in] bccgns Pointer to BCCGNS object with boundary conditions information read from file.
 */
void
IOCGNS::setBoundaryConditions(BCCGNS* bccgns){
    if(bccgns != NULL){
        std::unique_ptr<BCCGNS> temp(new BCCGNS(*bccgns));
        m_storedBC = std::move(temp);
    }
};


/*!Execution command.
 * It reads the geometry if the condition m_read is true.
 * It writes the geometry if the condition m_write is true.
 */
void
IOCGNS::execute(){
    bool check = true;
    if (m_read) {
        check = read();
    }
    if (!check){
        (*m_log) << "error: file corrupted or not found : "<< m_rfilename << std::endl;
        (*m_log) << " " << std::endl;
        throw std::runtime_error ("file corrupted or not found : " + m_rfilename);
    }
    if (m_write) {
        check = write();
    }
    if (!check){
        (*m_log) << "error: write not done : geometry not linked " << std::endl;
        (*m_log) << " " << std::endl;
//        throw std::runtime_error ("write not done : geometry not linked ");
        return;
    }
}

/*!It reads the mesh geometry from an input file.
 * \return False if file doesn't exists, doesn't hold a cgns unique base, zone, unstructured volume mesh.
 */
bool
IOCGNS::read(){

    std::string file = m_rdir+"/"+m_rfilename+".cgns";
    std::string error_string = "read CGNS grid: " + file;

    long nVertices;
    //     long nCells, nBoundVertices;
    int nCoords; //, nConnSections;
    std::array< std::vector<double>,3 > coords;
    std::vector<CGNS_ENUMT(ElementType_t)> orderedConns;
    std::unordered_map<int, ivector1D> conns;
    std::unordered_map<int, ivector1D > bcLists;

    //Open cgns file
    int indexfile;
    if(cg_open(file.c_str(), CG_MODE_READ, &indexfile) != CG_OK){
        return false;
    }

    //Read number of bases
    int nbases;
    if(cg_nbases(indexfile, &nbases) != CG_OK){
        return false;
    }
    if(nbases > 1){
        (*m_log) << m_name << " more than one base found in grid file -> only the first zone will be read" << std::endl;
    }

    //Read name of basis and physical dimension
    char basename[33];
    int physdim, celldim;
    if(cg_base_read(indexfile, 1, basename, &celldim, &physdim) != CG_OK){
        return false;
    }
    if(celldim != 3 || physdim !=3){
        //Only volume mesh supported
        //TODO NO SURFACE MESH (ONLY SURFACE WITHOUT VOLUME)?
        return false;
    }

    //Read number of zone in basis, check if only one.
    int nzones;
    if(cg_nzones(indexfile, 1, &nzones)!=CG_OK){
        return false;
    }
    if(nzones > 1){
        //Only single volume zone supported for now
        return false;
    }

    //Read type mesh of zone
    CGNS_ENUMT(ZoneType_t) zoneType;
    int index_dim;
    if(cg_zone_type(indexfile,1,1, &zoneType)!= CG_OK){
        return false;
    }
    if(cg_index_dim(indexfile,1,1, &index_dim)!= CG_OK){
        return false;
    }
    if(zoneType != CGNS_ENUMT(ZoneType_t)::CGNS_ENUMV(Unstructured) || index_dim != 1){
        //Only unstructured mesh supported for now (index_dim == 1 for unstructured)
        return false;
    }

    //Read size of zone (n nodes, n cells, n boundary nodes)
    std::vector<cgsize_t> sizeG(3);
    char zonename[33];
    if(cg_zone_read(indexfile,1,1, zonename, sizeG.data()) != CG_OK ){
        return false;
    }


    nVertices         = sizeG[0];
    //     nCells             = sizeG[1];
    //     nBoundVertices    = sizeG[2];

    //Read Vertices.
    if(cg_ncoords(indexfile,1,1, &nCoords)!= CG_OK){
        return false;
    }
    for(int i = 1; i <= nCoords; ++i){
        cgsize_t startIndex = 1;
        cgsize_t finishIndex = sizeG[0];
        CGNS_ENUMT(DataType_t) datatype;
        char name[33];
        coords[i-1].resize(nVertices);
        if(cg_coord_info(indexfile,1,1,i, &datatype, name)!=CG_OK){
            return false;
        }
        if(cg_coord_read(indexfile,1,1,name, datatype, &startIndex, &finishIndex, coords[i-1].data() )!=CG_OK){
            return false;
        }
    }

    //Read connectivities.
    //They are read starting from 1, fortran style. When matching up w/ coords
    //vector positions remember to diminish conn value of 1.
    //Read number of sections of connectivity structure
    int nSections;
    if(cg_nsections(indexfile,1,1, &nSections)!= CG_OK){
        return false;
    }

    for(int sec = 1; sec <= nSections; ++sec){

        //Read section sec
        char elementname[33];
        CGNS_ENUMT(ElementType_t) type;
        cgsize_t eBeg, eEnd, enBdry;
        int parent_flag;
        std::vector<cgsize_t>    connlocal;
        cgsize_t size;

        //Read elements name, type and range
        if(cg_section_read(indexfile,1,1, sec, elementname, &type, &eBeg, &eEnd,&enBdry, & parent_flag)!= CG_OK){
            return false;
        }

        //Read size of connectivity data
        if(cg_ElementDataSize(indexfile,1,1, sec,&size)!= CG_OK){
            return false;
        }
        //cgsize_t nelements = eEnd-eBeg + 1;


        connlocal.resize((size_t) size);
        int *ptr = NULL;
        if(cg_elements_read(indexfile,1,1,sec, connlocal.data(),ptr) !=CG_OK){
            return false;
        }

        conns[sec].resize(connlocal.size());
        int count=0;
        for(const auto &val: connlocal){
            conns[sec][count] = (int)val;
            ++count;
        }

        orderedConns.push_back(type);
    }

    //explore superficial boundary conditions definition, for boundary surface extraction.
    int nBcs;
    if(cg_nbocos(indexfile,1,1, &nBcs)!= CG_OK){
        return false;
    }

    //up to now i'm not able to encoding all the possible variants of bc node structure.
    // i rely on the element list . Need to study better the cgns docs.
    // TODO ????
    for(int bc=1; bc<=nBcs; ++bc){

        char name[33];
        CGNS_ENUMT(BCType_t) bocotype;
        CGNS_ENUMT(PointSetType_t) ptset_type;
        std::vector<cgsize_t> nBCElements(2);
        int normalIndex;
        cgsize_t normalListSize;
        CGNS_ENUMT(DataType_t) normalDataType;
        int ndataset;
        CGNS_ENUMT(GridLocation_t) location;

        if(cg_boco_gridlocation_read(indexfile,1,1,bc, &location) != CG_OK){
            return false;
        }

        if(cg_boco_info(indexfile,1,1,bc, name, &bocotype, &ptset_type, nBCElements.data(),
                &normalIndex, &normalListSize, &normalDataType, &ndataset) != CG_OK){
            return false;
        }

        if(ptset_type == CGNS_ENUMT(PointSetType_t)::CGNS_ENUMV(ElementList) ||
           ptset_type == CGNS_ENUMT(PointSetType_t)::CGNS_ENUMV(ElementRange) ){

            cgsize_t dim = nBCElements[0];

            std::vector<cgsize_t> localbc;

            if(ptset_type == CGNS_ENUMT(PointSetType_t)::CGNS_ENUMV(ElementList) ){
                localbc.resize((size_t) dim);
                int *ptr = NULL;

                if(cg_boco_read(indexfile,1,1,bc, localbc.data(), ptr )!= CG_OK){
                    return false;
                }

            }
            else{

                std::vector<cgsize_t> rangeidx(2);
                int *ptr = NULL;

                if(cg_boco_read(indexfile,1,1,bc, rangeidx.data(), ptr )!= CG_OK){
                    return false;
                }

                dim = rangeidx[1] - rangeidx[0] + 1;
                localbc.reserve((size_t) dim);

                for (auto idx = rangeidx[0]; idx <= rangeidx[1]; idx++){
                    localbc.push_back(idx);
                }

            }

            bcLists[bc-1].resize(localbc.size());
            int count=0;
            for(const auto &val: localbc){
                // WHY INT AND NOT LONG INT (IT IS A BITPIT PID) ??
                bcLists[bc-1][count] = (int)val;
                ++count;
            }

        }

        m_storedBC->mcg_pidtobc[bc] = bocotype;
        m_storedBC->mcg_pidtoname[bc] = name;

    }

    m_storedBC->mcg_pidtobc[0] = CGNS_ENUMV(BCTypeNull);
    m_storedBC->mcg_pidtoname[0] = "undefined";

    //Finish reading CGNS file
    cg_close(indexfile);

    //Reverse info in your grids.
    std::unique_ptr<MimmoObject> patchVol(new MimmoObject(2));
    std::unique_ptr<MimmoObject> patchBnd(new MimmoObject(1));
    long id;
    darray3E temp;

    //    Stock vertices in volume grid
    for(int i=0; i<nVertices; ++i){

        for(int j=0; j<3; ++j)    temp[j] = coords[j][i];
        id = i+1; //not C indexing, labeling coherent with connectivity.
        patchVol->addVertex(temp,id);
    }

    //debug only;
    //TODO ????

    id = 1;
    int idsection = 0;
    //Unpack 3D elements connectivities and store it in volume grid. Label same species elements w PID.
    for(const auto & val: orderedConns){

        idsection++;

        livector1D lConn;
        bitpit::ElementType btype;
        long PID = 0;
        int size = conns[idsection].size();

        switch(val){
        case CGNS_ENUMV(MIXED):

             unpack3DElementsMixedConns(patchVol.get(), patchBnd.get(), conns[idsection], id);
        break;

        case CGNS_ENUMV(TETRA_4):
        case CGNS_ENUMV(TETRA_10):
        case CGNS_ENUMV(TETRA_16):
        case CGNS_ENUMV(TETRA_20):
        case CGNS_ENUMV(TETRA_22):
        case CGNS_ENUMV(TETRA_34):
        case CGNS_ENUMV(TETRA_35):


        btype = bitpit::ElementType::TETRA;
        lConn.resize(4);
        for(int i=0; i<size; i+=4){ //first 4 element are needed only
            for(int j=0; j<4; ++j){
                lConn[j] = conns[idsection][i+j];
            }
            patchVol->addConnectedCell(lConn, btype, id );
            patchVol->setPIDCell(id,PID);
            id++;
        }
        break;

        case CGNS_ENUMV(PYRA_5):
        case CGNS_ENUMV(PYRA_13):
        case CGNS_ENUMV(PYRA_14):
        case CGNS_ENUMV(PYRA_21):
        case CGNS_ENUMV(PYRA_29):
        case CGNS_ENUMV(PYRA_30):
        case CGNS_ENUMV(PYRA_50):
        case CGNS_ENUMV(PYRA_55):

        btype = bitpit::ElementType::PYRAMID;
        lConn.resize(5);
        for(int i=0; i<size; i+=5){
            for(int j=0; j<5; ++j){
                lConn[j] = conns[idsection][i+j];
            }
            patchVol->addConnectedCell(lConn, btype, id );
            patchVol->setPIDCell(id,PID);
            id++;
        }
        break;

        case CGNS_ENUMV(PENTA_6):
        case CGNS_ENUMV(PENTA_15):
        case CGNS_ENUMV(PENTA_18):
        case CGNS_ENUMV(PENTA_24):
        case CGNS_ENUMV(PENTA_38):
        case CGNS_ENUMV(PENTA_40):
        case CGNS_ENUMV(PENTA_33):
        case CGNS_ENUMV(PENTA_66):
        case CGNS_ENUMV(PENTA_75):

        btype = bitpit::ElementType::WEDGE;
        lConn.resize(6);
        for(int i=0; i<size; i+=6){
            for(int j=0; j<6; ++j){
                lConn[j] = conns[idsection][i+j];
            }
            //remap in bitpit conn. TODO complete ref element mapper for connectivity.
            std::swap(lConn[1], lConn[2]);
            std::swap(lConn[4], lConn[5]);

            patchVol->addConnectedCell(lConn, btype, id );
            patchVol->setPIDCell(id,PID);
            id++;
        }

        break;

        case CGNS_ENUMV(HEXA_8):
        case CGNS_ENUMV(HEXA_20):
        case CGNS_ENUMV(HEXA_27):
        case CGNS_ENUMV(HEXA_32):
        case CGNS_ENUMV(HEXA_56):
        case CGNS_ENUMV(HEXA_64):
        case CGNS_ENUMV(HEXA_44):
        case CGNS_ENUMV(HEXA_98):
        case CGNS_ENUMV(HEXA_125):

        btype = bitpit::ElementType::HEXAHEDRON;
        lConn.resize(8);
        for(int i=0; i<size; i+=8){
            for(int j=0; j<8; ++j){
                lConn[j] = conns[idsection][i+j];
            }
            patchVol->addConnectedCell(lConn, btype, id );
            patchVol->setPIDCell(id,PID);
            id++;
        }
        break;

        case CGNS_ENUMV(TRI_3):
        case CGNS_ENUMV(TRI_6):
        case CGNS_ENUMV(TRI_9):
        case CGNS_ENUMV(TRI_10):
        case CGNS_ENUMV(TRI_12):
        case CGNS_ENUMV(TRI_15):

        btype = bitpit::ElementType::TRIANGLE;
        lConn.resize(3);

        for(int i=0; i<size; i+=3){
            for(int j=0; j<3; ++j){
                lConn[j] = conns[idsection][i+j];
            }
            patchBnd->addConnectedCell(lConn, btype, id );
            patchBnd->setPIDCell(id,PID);
            id++;
        }
        break;

        case CGNS_ENUMV(QUAD_4):
        case CGNS_ENUMV(QUAD_8):
        case CGNS_ENUMV(QUAD_9):
        case CGNS_ENUMV(QUAD_12):
        case CGNS_ENUMV(QUAD_16):
        case CGNS_ENUMV(QUAD_25):

        btype = bitpit::ElementType::QUAD;
        lConn.resize(4);
        for(int i=0; i<size; i+=4){
            for(int j=0; j<4; ++j){
                lConn[j] = conns[idsection][i+j];
            }
            patchBnd->addConnectedCell(lConn, btype, id );
            patchBnd->setPIDCell(id,PID);
            id++;
        }
        break;

        default:
            //do nothing
            break;
        }
    }

    //divide patchBnd in subpatch if any bc is present.
    if(bcLists.size() > 0)    {
        for(const auto & sel: bcLists){
            if(sel.second.size() > 0){
                long PID = sel.first + 1;
                for(const auto &ind : sel.second){
                    id = ind;
                    patchBnd->setPIDCell(id,PID);
                }
            }
        }//end for

    }


    //adding vstand alone vertices to boundary patches and release all structures
    {
        //TODO this is not the best way to get this. In case of polygonal meshes it does not work.
        //Anyway CGNS does not support polygons for now. So be it.
        livector2D tempConn;
        tempConn = patchBnd->getConnectivity();
        std::set<long> ordIndex;

        for(const auto & val: tempConn){
            ordIndex.insert(val.begin(), val.end());
        }

        for(const auto & val: ordIndex){
            temp = patchVol->getVertexCoords(val);
            patchBnd->addVertex(temp,val);
        }
    }

    //release the meshes
    m_volmesh = std::move(patchVol);
    m_surfmesh = std::move(patchBnd);

    return true;
}

/*!It writes the mesh geometry on output .cgns file.
 * If boundary surface, PID subdivided, is available, write all patches as CGNS Wall boundary condition .
 *\return False if valid volume geometry or surface geometry is not found.
 */
bool
IOCGNS::write(){

    /* Clear old Info if stored. */
    m_storedInfo->mcg_number.clear();

    std::string file = m_wdir+"/"+m_wfilename+".cgns";
    std::string error_string = "write CGNS grid: " + file;

    long nVertices, nCells, nBoundVertices;
    //int nCoords;
    std::array< std::vector<double>,3 > coords;

    /* Fill this structures (surface boundary connectivity has
     * to be filled with same vertex indices of volume mesh) */
    MimmoObject * vol = getGeometry();
    MimmoObject * bnd = getSurfaceBoundary();

    if( vol == NULL || bnd == NULL ) return false;
    if( vol->isEmpty() || bnd->isEmpty() ) return false;

    /* Verify if a surface boundary mesh exists. */
    //bool flagBnd = (bnd != NULL);

    /* Check number of volume cells and global vertices. */
    nVertices = vol->getNVertices();
    nCells = vol->getNCells();
    nBoundVertices=0;

    //fill coordinates;
    coords[0].resize(nVertices);
    coords[1].resize(nVertices);
    coords[2].resize(nVertices);
    {
        dvecarr3E vert = vol->getVerticesCoords();
        int count=0;
        for(const auto &val:vert){
            for(int j=0; j<3; ++j)    coords[j][count] = val[j];
            ++count;
        }
    }

    //Write Base and Zone Info
    int indexfile;
    if(cg_open(file.c_str(), CG_MODE_WRITE, &indexfile) != CG_OK){
        (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
        throw std::runtime_error ("cgns error during write " + m_wfilename);
    }

    int baseindex = 1;
    char basename[33] = "Base";
    int physdim=3, celldim=3;
    if(cg_base_write(indexfile,basename, celldim, physdim, &baseindex) != CG_OK){
        (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
        throw std::runtime_error ("cgns error during write " + m_wfilename);
    }

    int zoneindex=1;
    CGNS_ENUMT(ZoneType_t) zoneType =CGNS_ENUMT(ZoneType_t)::CGNS_ENUMV(Unstructured) ;
    //int index_dim = 1;
    char zonename[33] = "Zone  1";
    std::vector<cgsize_t> sizeG(3);
    sizeG[0] = nVertices;
    sizeG[1] = nCells;
    sizeG[2] = nBoundVertices;

    if(cg_zone_write(indexfile,baseindex, zonename, sizeG.data(), zoneType, &zoneindex) != CG_OK ){
        (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
        throw std::runtime_error ("cgns error during write " + m_wfilename);
    }

    //writing vertices.
    svector1D names(3, "CoordinateX");
    names[1] = "CoordinateY";
    names[2] = "CoordinateZ";

    CGNS_ENUMT(DataType_t) datatype= CGNS_ENUMT(DataType_t)::CGNS_ENUMV(RealDouble);

    for(int i=1; i<=3; ++i){
        if(cg_coord_write(indexfile,baseindex,zoneindex, datatype, names[i-1].data(), coords[i-1].data(), &i )!=CG_OK){
            (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
            throw std::runtime_error ("cgns error during write " + m_wfilename);
        }
    }

    //Recover CGNS Info from volume and surface mesh
    recoverCGNSInfo();

    /* Write volume elements */
    int sec = 0;
    cgsize_t eBeg = 1, eEnd;
    int nbdry = 0;
    for(const auto &connMap : m_storedInfo->mcg_typetoconn){
        sec++;
        if(connMap.second.size() == 0) continue;

        CGNS_ENUMT(ElementType_t) type = static_cast<CGNS_ENUMT(ElementType_t)>(connMap.first);

        eEnd = eBeg + m_storedInfo->mcg_number[type] - 1;

        if(cg_section_write(indexfile,baseindex,zoneindex,m_storedInfo->mcg_typetostring[type].data(), type,eBeg,eEnd, nbdry, connMap.second.data(), &sec) !=CG_OK){
            (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
            throw std::runtime_error ("cgns error during write " + m_wfilename);
        }
        eBeg = eEnd+1;
    }

    /* Write surface elements */
    for(const auto &connMap : m_storedInfo->mcg_bndtypetoconn){
        sec++;
        if(connMap.second.size() == 0) continue;

        CGNS_ENUMT(ElementType_t) type = static_cast<CGNS_ENUMT(ElementType_t)>(connMap.first);

        eEnd = eBeg + m_storedInfo->mcg_bndnumber[type] - 1;

        if(cg_section_write(indexfile,baseindex,zoneindex,m_storedInfo->mcg_typetostring[type].data(), type,eBeg,eEnd, nbdry, connMap.second.data(), &sec) !=CG_OK){
            (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
            throw std::runtime_error ("cgns error during write " + m_wfilename);
        }
        eBeg = eEnd+1;
    }

    /* Write boundary conditions if thay are present.
     */
    int bcid;
    for(const auto & bc: m_storedBC->mcg_bndpidtoindex){

        /* Default tag for boundary conditions = Wall.
         * Default name for boundary conditions = wall_PID.
         */
        CGNS_ENUMT(BCType_t) bocotype = CGNS_ENUMT(BCType_t)::CGNS_ENUMV(BCWall);
        CGNS_ENUMT(PointSetType_t) ptset_type = CGNS_ENUMT(PointSetType_t)::CGNS_ENUMV(ElementList);


        cgsize_t nelements = bc.second.size();
        int bcPID = bc.first;
        if (m_storedBC->mcg_pidtobc.count(bcPID)) bocotype = static_cast<CGNS_ENUMT(BCType_t)>(m_storedBC->mcg_pidtobc[bcPID]);

        std::string bcname = "wall_"+std::to_string(bcPID);
        if (m_storedBC->mcg_pidtoname.count(bcPID)) bcname = m_storedBC->mcg_pidtoname[bcPID];

        if(cg_boco_write(indexfile, baseindex, zoneindex, bcname.data(),
                bocotype, ptset_type, nelements, bc.second.data(), &bcid )!= CG_OK){
            (*m_log) << "error: cgns error during write " << m_wfilename << std::endl;
            throw std::runtime_error ("cgns error during write " + m_wfilename);
        }

    }

    /*
     * Default location = faceCenter ---> deactivated
    for (int ibc = 1; ibc <= bcid; ibc++){
        cg_goto(indexfile,baseindex,"Zone_t",1,"ZoneBC_t",1,"BC_t",ibc,"end");
        cg_gridlocation_write(CG_CellCenter);

    }
     */

    /* Finish writing CGNS file */
    cg_close(indexfile);
    return true;
}

/*!
 * Extract mixed connectivity of 3D elements volume mesh//surface boundary mesh in two separated objects, given an array
 * of mixed connectivity of CGNS reader
 * \param[in,out]    patchVol Pointer to Volume MimmoObject handler
 * \param[in,out]    patchSurf Pointer to Surface MimmoObject handler
 * \param[in,out]    conn    List of vertex index connectivity of CGNS_ENUMV(MIXED)type
 * \param[in,out]    startId Starting ID to labeling mixed cells. Returns incremented of found mixed 3D cells
 */
void
IOCGNS::unpack3DElementsMixedConns(MimmoObject * patchVol, MimmoObject* patchSurf, ivector1D & conn, long & startId){

    livector1D lConn;
    bitpit::ElementType btype;
    long PID = 0;
    long &id = startId;
    ivector1D::iterator it=conn.begin(), itE=conn.end();

    while(it !=itE){

        CGNS_ENUMT(ElementType_t) et = static_cast<CGNS_ENUMT(ElementType_t)>(*it);
        ++it;

        switch(et){
            case CGNS_ENUMV(TETRA_4):
            case CGNS_ENUMV(TETRA_10):
            case CGNS_ENUMV(TETRA_16):
            case CGNS_ENUMV(TETRA_20):
            case CGNS_ENUMV(TETRA_22):
            case CGNS_ENUMV(TETRA_34):
            case CGNS_ENUMV(TETRA_35):

                btype = bitpit::ElementType::TETRA;
                lConn.resize(4);
                for(int i=0; i<4; i++){
                    lConn[i] = *it;
                    ++it;
                }
                patchVol->addConnectedCell(lConn, btype, id );
                patchVol->setPIDCell(id,PID);
                id++;
                it = itE;
            break;

            case CGNS_ENUMV(PYRA_5):
            case CGNS_ENUMV(PYRA_13):
            case CGNS_ENUMV(PYRA_14):
            case CGNS_ENUMV(PYRA_21):
            case CGNS_ENUMV(PYRA_29):
            case CGNS_ENUMV(PYRA_30):
            case CGNS_ENUMV(PYRA_50):
            case CGNS_ENUMV(PYRA_55):

                btype = bitpit::ElementType::PYRAMID;
                lConn.resize(5);
                for(int i=0; i<5; i++){
                    lConn[i] = *it;
                    ++it;
                }

                patchVol->addConnectedCell(lConn, btype, id );
                patchVol->setPIDCell(id,PID);
                id++;
                it = itE;
            break;

            case CGNS_ENUMV(PENTA_6):
            case CGNS_ENUMV(PENTA_15):
            case CGNS_ENUMV(PENTA_18):
            case CGNS_ENUMV(PENTA_24):
            case CGNS_ENUMV(PENTA_38):
            case CGNS_ENUMV(PENTA_40):
            case CGNS_ENUMV(PENTA_33):
            case CGNS_ENUMV(PENTA_66):
            case CGNS_ENUMV(PENTA_75):

                btype = bitpit::ElementType::WEDGE;
                lConn.resize(6);
                for(int i=0; i<6; i++){
                    lConn[i] = *it;
                    ++it;
                }
                //remap in bitpit conn. TODO complete ref element mapper for connectivity.
                std::swap(lConn[1], lConn[2]);
                std::swap(lConn[4], lConn[5]);

                patchVol->addConnectedCell(lConn, btype, id );
                patchVol->setPIDCell(id,PID);
                id++;
                it = itE;
            break;

            case CGNS_ENUMV(HEXA_8):
            case CGNS_ENUMV(HEXA_20):
            case CGNS_ENUMV(HEXA_27):
            case CGNS_ENUMV(HEXA_32):
            case CGNS_ENUMV(HEXA_56):
            case CGNS_ENUMV(HEXA_64):
            case CGNS_ENUMV(HEXA_44):
            case CGNS_ENUMV(HEXA_98):
            case CGNS_ENUMV(HEXA_125):

                btype = bitpit::ElementType::HEXAHEDRON;
                lConn.resize(8);
                for(int i=0; i<8; i++){
                    lConn[i] = *it;
                    ++it;
                }

                patchVol->addConnectedCell(lConn, btype, id );
                patchVol->setPIDCell(id,PID);
                id++;
                it = itE;
            break;

            case CGNS_ENUMV(NODE):
            case CGNS_ENUMV(BAR_2):
            case CGNS_ENUMV(BAR_3):
            case CGNS_ENUMV(BAR_4):
            case CGNS_ENUMV(BAR_5):
                it = itE; //skip all
            break;

            case CGNS_ENUMV(TRI_3):
            case CGNS_ENUMV(TRI_6):
            case CGNS_ENUMV(TRI_9):
            case CGNS_ENUMV(TRI_10):
            case CGNS_ENUMV(TRI_12):
            case CGNS_ENUMV(TRI_15):

                btype = bitpit::ElementType::TRIANGLE;
                lConn.resize(3);
                for(int i=0; i<3; i++){
                    lConn[i] = *it;
                    ++it;
                }

                patchSurf->addConnectedCell(lConn, btype, id );
                patchSurf->setPIDCell(id,PID);
                id++;
                it = itE;
            break;

            case CGNS_ENUMV(QUAD_4):
            case CGNS_ENUMV(QUAD_8):
            case CGNS_ENUMV(QUAD_9):
            case CGNS_ENUMV(QUAD_12):
            case CGNS_ENUMV(QUAD_16):
            case CGNS_ENUMV(QUAD_25):

                btype = bitpit::ElementType::QUAD;
                lConn.resize(4);
                for(int i=0; i<4; i++){
                    lConn[i] = *it;
                    ++it;
                }

                patchSurf->addConnectedCell(lConn, btype, id );
                patchSurf->setPIDCell(id,PID);
                id++;
                it = itE;
            break;

        default:
            (*m_log)<< "error: "<< m_name << " found unrecognized CGNS element while reading. Impossible to absorb further mixed elements. "<<std::endl;
            throw std::runtime_error (m_name + " : found unrecognized CGNS element while reading. Impossible to absorb further mixed elements. ");
            return;
            break;
        }
    } //end while

};

/*! It recovers CGNS Info from linked Mimmo Objects.
 * It fills all the structures needed to export the volume and (eventually) surface
 * boundary meshes in CGNS format.
 *TODO stop writing if POLYHEDRAL mesh.
 */
void
IOCGNS::recoverCGNSInfo(){

    MimmoObject * vol = getGeometry();
    MimmoObject * bnd = getSurfaceBoundary();

    /* Verifiy if a surface mesh (boundary mesh) is set. */
    bool flagBnd = (bnd != NULL);

    if( vol == NULL) return;
    if( vol->isEmpty()) return;
    if( flagBnd && bnd->isEmpty() ) return;

    int nVolElements = 0;

    /* Fill volume map info. */
    CGNS_ENUMT(ElementType_t) cgtype;
    long int ID;
    for (auto & cell : vol->getCells()){
        cgtype = static_cast<CGNS_ENUMT(ElementType_t)>(m_storedInfo->mcg_typeconverter[cell.getType()]);
        ID = cell.getId();
        m_storedInfo->mcg_number[cgtype]++;
        m_storedInfo->mcg_typetoid[cgtype].push_back(ID);
        auto vList = cell.getVertexIds();

        //remap in bitpit conn. TODO complete ref element mapper for connectivity.
        std::vector<long> lConn;
        lConn.insert(lConn.end(), vList.begin(), vList.end());
        long temp = lConn[0];
        lConn[0]= lConn[1];
        lConn[1] = temp;
        temp = lConn[3];
        lConn[3] = lConn[4];
        lConn[4] = temp;
        for (const auto & iV : vList){
            m_storedInfo->mcg_typetoconn[cgtype].push_back(iV);
        }
    }

    /*Iterate map volume info and fill local CGNS index info
     * (volume elements will be appended before boundary elements)
     */
    //    std::map<CG_ElementType_t, std::vector<long int> >::iterator it, itend;
    std::map<int, std::vector<long int> >::iterator it, itend;
    std::vector<long int>::iterator itV, itVend;
    itend = m_storedInfo->mcg_typetoid.end();
    int cgnsidx = 0;
    for ( it = m_storedInfo->mcg_typetoid.begin(); it != itend; ++it ){
        itVend = it->second.end();
        for ( itV = it->second.begin(); itV != itVend; ++itV ){
            cgnsidx++;
            ID = (*itV);
            m_storedInfo->mcg_idtoindex[ID] = cgnsidx;
            m_storedInfo->mcg_indextoid.push_back(ID);
        }
    }
    nVolElements = cgnsidx;


    if (flagBnd){
        /* Fill surface mesh (boundary) map info. */
        bitpit::PatchKernel* bndpatch = bnd->getPatch();
        CGNS_ENUMT(ElementType_t) cgtype;
        long int ID;
        for (auto & cell : bnd->getCells()){
            cgtype = static_cast<CGNS_ENUMT(ElementType_t)>(m_storedInfo->mcg_typeconverter[cell.getType()]);
            ID = cell.getId();
            m_storedInfo->mcg_bndnumber[cgtype]++;
            m_storedInfo->mcg_bndtypetoid[cgtype].push_back(ID);
            auto vList = cell.getVertexIds();
            for (const auto &iV : vList){
                m_storedInfo->mcg_bndtypetoconn[cgtype].push_back(iV);
            }
        }

        /*Iterate map surface (boundary) info and fill local CGNS index info
         * (surface elements will be appended after volume elements)
         */
        //        std::map<CG_ElementType_t, std::vector<long int> >::iterator it, itend;
        std::map<int, std::vector<long int> >::iterator it, itend;
        std::vector<long int>::iterator itV, itVend;
        itend = m_storedInfo->mcg_bndtypetoid.end();
        int bndcgnsidx = 0;
        for ( it = m_storedInfo->mcg_bndtypetoid.begin(); it != itend; ++it ){
            itVend = it->second.end();
            for ( itV = it->second.begin(); itV != itVend; ++itV ){
                bndcgnsidx++;
                ID = (*itV);
                m_storedInfo->mcg_bndidtoindex[ID] = nVolElements + bndcgnsidx;
                m_storedInfo->mcg_bndindextoid.push_back(ID);
                m_storedBC->mcg_bndpidtoindex[bndpatch->getCell(ID).getPID()].push_back(bndcgnsidx + nVolElements);
            }
        }
    }

}

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
IOCGNS::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    std::string input;

    BaseManipulation::absorbSectionXML(slotXML, name);

    if(slotXML.hasOption("ReadFlag")){
        input = slotXML.get("ReadFlag");
        bool value = false;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setRead(value);
    };

    if(slotXML.hasOption("ReadDir")){
        input = slotXML.get("ReadDir");
        input = bitpit::utils::string::trim(input);
        if(input.empty())   input = "./";
        setReadDir(input);
    };

    if(slotXML.hasOption("ReadFilename")){
        input = slotXML.get("ReadFilename");
        input = bitpit::utils::string::trim(input);
        if(input.empty())   input = "mimmoGeometry";
        setReadFilename(input);
    };

    if(slotXML.hasOption("WriteDir")){
        input = slotXML.get("WriteDir");
        input = bitpit::utils::string::trim(input);
        if(input.empty())   input = "./";
        setWriteDir(input);
    };

    if(slotXML.hasOption("WriteFilename")){
        input = slotXML.get("WriteFilename");
        input = bitpit::utils::string::trim(input);
        if(input.empty())   input = "mimmoGeometry";
        setWriteFilename(input);
    };

};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
IOCGNS::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    BaseManipulation::flushSectionXML(slotXML, name);

    std::string output;

    output = std::to_string(m_read);
    slotXML.set("ReadFlag", output);

    slotXML.set("ReadDir", m_rdir);

    slotXML.set("ReadFilename", m_rfilename);

    slotXML.set("WriteDir", m_wdir);

    slotXML.set("WriteFilename", m_wfilename);

};


};
