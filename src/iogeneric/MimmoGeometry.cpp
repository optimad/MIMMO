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
#include "MimmoGeometry.hpp"
#include "customOperators.hpp"
#include "VTUGridReader.hpp"
#include <iostream>

using namespace std;
using namespace bitpit;

namespace mimmo {

/*!Default constructor of MimmoGeometry.
 */
MimmoGeometry::MimmoGeometry(){
    m_name         = "mimmo.Geometry";
    setDefaults();
}

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
MimmoGeometry::MimmoGeometry(const bitpit::Config::Section & rootXML){

    m_name = "mimmo.Geometry";
    setDefaults();

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.Geometry"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!Default destructor of MimmoGeometry.
 */
MimmoGeometry::~MimmoGeometry(){
    clear();
};

/*!Copy constructor of MimmoGeometry.If to-be-copied object has an
 * internal MimmoObject instantiated, it will be soft linked
 * in the current class (its pointer only will be copied in m_geometry member);
 */
MimmoGeometry::MimmoGeometry(const MimmoGeometry & other):BaseManipulation(other){
    m_rinfo = other.m_rinfo;
    m_winfo = other.m_winfo;
    m_read = other.m_read;
    m_write = other.m_write;
    m_wformat = other.m_wformat;
    m_codex = other.m_codex;
    m_buildSkdTree = other.m_buildSkdTree;
    m_buildKdTree = other.m_buildKdTree;
    m_refPID = other.m_refPID;
    m_multiSolidSTL = other.m_multiSolidSTL;

    if(other.m_isInternal){
        m_geometry = other.m_intgeo.get();
    }
    m_isInternal = false;
};

/*!
 * Assignement operator of MimmoGeometry.If to-be-copied object has an
 * internal MimmoObject instantiated, it will be soft linked
 * in the current class (its pointer only will be copied in m_geometry member);
 */
MimmoGeometry & MimmoGeometry::operator=(MimmoGeometry other){
    swap(other);
    return *this;
};

/*!
 * Swap method.
 * \param[in] x object to be swapped
 */
void MimmoGeometry::swap(MimmoGeometry & x) noexcept
{
    std::swap(m_rinfo, x.m_rinfo);
    std::swap(m_winfo, x.m_winfo);
    std::swap(m_read, x.m_read);
    std::swap(m_write, x.m_write);
    std::swap(m_wformat, x.m_wformat);
    std::swap(m_codex, x.m_codex);
    std::swap(m_buildSkdTree, x.m_buildSkdTree);
    std::swap(m_buildKdTree, x.m_buildKdTree);
    std::swap(m_refPID, x.m_refPID);
    std::swap(m_multiSolidSTL, x.m_multiSolidSTL);
    std::swap(m_isInternal, x.m_isInternal);
    std::swap(m_intgeo, x.m_intgeo);
    BaseManipulation::swap(x);
}

/*!
 * Building the ports available in the class
 */
void
MimmoGeometry::buildPorts(){
    bool built = true;
    built = (built && createPortIn<MimmoObject*, MimmoGeometry>(this, &mimmo::MimmoGeometry::setGeometry, M_GEOM));
    built = (built && createPortOut<MimmoObject*, MimmoGeometry>(this, &mimmo::MimmoGeometry::getGeometry, M_GEOM));
    m_arePortsBuilt = built;
}

/*!
 * Return a const pointer to your current class.
 */
const MimmoGeometry * MimmoGeometry::getCopy(){
    return this;
}

/*!
 * Get current geometry pointer. Reimplementation of BaseManipulation::getGeometry
 */
MimmoObject * MimmoGeometry::getGeometry(){
    if(m_isInternal)    return m_intgeo.get();
    else                return m_geometry;
}

/*!
 * Get current geometry pointer. Reimplementation of BaseManipulation::getGeometry,
 * const overloading
 */
const MimmoObject * MimmoGeometry::getGeometry() const{
    if(m_isInternal)    return m_intgeo.get();
    else                return m_geometry;
}

/*!
 * Set proper member of the class to defaults
 */
void
MimmoGeometry::setDefaults(){

    m_read            = false;
    m_rinfo.fname    = "mimmoGeometry";
    m_rinfo.ftype    = static_cast<int>(FileType::STL);
    m_rinfo.fdir    = "./";

    m_write            = false;
    m_winfo.fname    = "mimmoGeometry";
    m_winfo.ftype    = static_cast<int>(FileType::STL);
    m_winfo.fdir    = "./";

    m_wformat        = Short;
    m_isInternal      = true;
    m_codex            = true;
    m_buildSkdTree    = false;
    m_buildKdTree    = false;
    m_refPID = 0;
    m_multiSolidSTL = true;
}

/*!It sets the condition to read the geometry on file during the execution.
 * \param[in] read Does it read the geometry in execution?
 */
void
MimmoGeometry::setRead(bool read){
    m_read = read;
}

/*!It sets the condition to read the geometry on file during the execution.
 * \param[in] read Does it read the geometry in execution?
 */
void
MimmoGeometry::_setRead(bool read){
    m_read = read;
}

/*!It sets the type of file to read the geometry during the execution.
 * \param[in] type Extension of file.
 * NOTE: use it only for converter mode, otherwise setFileType is recommended
 */
void
MimmoGeometry::setReadFileType(FileType type){
    m_rinfo.ftype = type._to_integral();
}

/*!It sets the type of file to read the geometry during the execution.
 * \param[in] type Label of file type (0 = bynary STL).
 * NOTE: use it only for converter mode, otherwise setFileType is recommended
 */
void
MimmoGeometry::setReadFileType(int type){
    auto maybe_type = FileType::_from_integral_nothrow(type);
    if(!maybe_type) type = 0;
    m_rinfo.ftype = type;
}

/*!It sets the name of directory to read the geometry.
 * \param[in] dir Name of directory.
 * NOTE: use it only for converter mode, otherwise setDir is recommended
 */
void
MimmoGeometry::setReadDir(string dir){
    m_rinfo.fdir = dir;
}

/*!It sets the name of file to read the geometry.
 * \param[in] filename Name of input file.
 * NOTE: use it only for converter mode, otherwise setFilename is recommended
 */
void
MimmoGeometry::setReadFilename(string filename){
    m_rinfo.fname = filename;
}

/*!It sets the type of file to write the geometry during the execution.
 * \param[in] type Extension of file.
 * NOTE: use it only for converter mode, otherwise setFileType is recommended
 */
void
MimmoGeometry::setWriteFileType(FileType type){
    m_winfo.ftype = type._to_integral();
}

/*!It sets the type of file to write the geometry during the execution.
 * \param[in] type Label of file type (0 = bynary STL).
 * NOTE: use it only for converter mode, otherwise setFileType is recommended
 */
void
MimmoGeometry::setWriteFileType(int type){
    auto maybe_type = FileType::_from_integral_nothrow(type);
    if(!maybe_type) type = 0;
    m_winfo.ftype = type;
}

/*!It sets the condition to write the geometry on file during the execution.
 * \param[in] write Does it write the geometry in execution?
 */
void
MimmoGeometry::setWrite(bool write){
    m_write = write;
}

/*!It sets the condition to write the geometry on file during the execution.
 * \param[in] write Does it write the geometry in execution?
 */
void
MimmoGeometry::_setWrite(bool write){
    m_write = write;
}

/*!It sets the name of directory to write the geometry.
 * \param[in] dir Name of directory.
 * NOTE: use it only for converter mode, otherwise setDir is recommended
 */
void
MimmoGeometry::setWriteDir(string dir){
    m_winfo.fdir = dir;
}

/*!It sets the name of file to write the geometry.
 * \param[in] filename Name of output file.
 * NOTE: use it only for converter mode, otherwise setFilename is recommended
 */
void
MimmoGeometry::setWriteFilename(string filename){
    m_winfo.fname = filename;

}

/*!Set the mode of the block as reader, writer or converter.
 * \param[in] mode Modality of the block
 * NOTE: in case of converter the directory and filename will be the same
 * for input & output
 * (the I/O extensions have to be set by setReadFileType & setWriteFileType).
 */
void
MimmoGeometry::setIOMode(IOMode mode){
    if (mode._to_integral() == IOMode::READ){
        _setRead();
        _setWrite(false);
    }
    else if (mode._to_integral() == IOMode::WRITE){
        _setRead(false);
        _setWrite();
    }
    else if (mode._to_integral() == IOMode::CONVERT){
        _setRead();
        _setWrite();
    }
    else{
        (*m_log) << m_name << " warning: no allowed IOMode found -> set as reader" << std::endl;
        _setRead();
        _setWrite(false);
    }
}

/*!Set the mode of the block as reader, writer or converter.
 * \param[in] mode Modality of the block
 * NOTE: in case of converter the directory and filename will be the same
 * for input & output
 * (the I/O extensions have to be set by setReadFileType & setWriteFileType).
 */
void
MimmoGeometry::setIOMode(int mode){
    if (mode == IOMode::READ){
        _setRead();
        _setWrite(false);
    }
    else if (mode == IOMode::WRITE){
        _setRead(false);
        _setWrite();
    }
    else if (mode == IOMode::CONVERT){
        _setRead();
        _setWrite();
    }
    else{
        (*m_log) << m_name << " warning: no allowed IOMode find -> set as reader" << std::endl;
        _setRead();
        _setWrite(false);
    }
}

/*!It sets the name of directory to read/write the geometry.
 * \param[in] dir Name of directory.
 */
void
MimmoGeometry::setDir(string dir){
    m_rinfo.fdir = dir;
    m_winfo.fdir = dir;
}

/*!It sets the name of file to read/write the geometry.
 * \param[in] filename Name of output file.
 */
void
MimmoGeometry::setFilename(string filename){
    m_winfo.fname = filename;
    m_rinfo.fname = filename;
}

/*!It sets the type of file to read/write the geometry during the execution.
 * \param[in] type Extension of file.
 * NOTE: in case of converter mode use separate setReadFileType and setWriteFileType
 */
void
MimmoGeometry::setFileType(FileType type){
    m_winfo.ftype = type._to_integral();
    m_rinfo.ftype = type._to_integral();
}

/*!It sets the type of file to read/write the geometry during the execution.
 * \param[in] type Label of file type (0 = bynary STL).
 * NOTE: in case of converter mode use separate setReadFileType and setWriteFileType
 */
void
MimmoGeometry::setFileType(int type){
    auto maybe_type = FileType::_from_integral_nothrow(type);
    if(!maybe_type) type = 0;
    m_winfo.ftype = type;
    m_rinfo.ftype = type;
}

/*!
 * set codex ASCII false, BINARY true for writing sessions ONLY.
 * Default is Binary/Appended. Pay attention, binary writing is effective
 * only those file formats which support it.(ex STL, STVTU, SQVTU, VTVTU, VHVTU)
 * \param[in] binary codex flag.
 */
void MimmoGeometry::setCodex(bool binary){
    m_codex = binary;
}

/*!
 * Set ASCII Multi Solid STL writing. The method has effect only while writing
 * standard surface triangulations in STL format type.
 * \param[in] multi boolean, true activate Multi Solid STL writing.
 */
void MimmoGeometry::setMultiSolidSTL(bool multi){
    m_multiSolidSTL = multi;
}


/*!Sets your current class as a "HARD" copy of the argument.
 * Hard copy means that only your current geometric object MimmoObject is
 * copied as a stand alone internal member and stored in the unique pointer member m_intgeo.
 * Other members are exactly copied
 * \param[in] other pointer to MimmoGeometry class.
 */
void
MimmoGeometry::setHARDCopy(MimmoGeometry * other){

    if(other->isEmpty()){
        (*m_log)<<"warning MimmoGeometry::setHARDCopy() : attempt to set hard copy from an empty object.Do nothing"<<std::endl;
        return;
    }
    //make a soft copy
    MimmoGeometry temp = *other;
    swap(temp);

    //hard copy the internal mimmoobject.
    std::unique_ptr<MimmoObject> obj = other->getGeometry()->clone();
    m_intgeo = std::move(obj);
    m_isInternal = true;
    m_geometry = NULL;
}

/*!
 * Set geometry from an external MimmoObject source, softly linked.
 * Reimplementation of BaseManipulation::setGeometry
 * \param[in] external pointer to external geometry
 */
void
MimmoGeometry::setGeometry(MimmoObject * external){
    if(external == NULL)    return;
    if(external->isEmpty()) return;
    if(getGeometry() == external) return;

    m_intgeo.reset(nullptr);
    m_geometry = external;
    m_isInternal = false;
};

/*!
 * Force your class to allocate an internal MimmoObject of type 1-Superficial mesh
 * 2-Volume Mesh,3-Point Cloud, 4-3DCurve. Other internal object allocated or externally linked geometries
 * will be destroyed/unlinked.
 * \param[in] type 1-Surface MimmoObject, 2-Volume MimmoObject, 3-Point Cloud, 4-3DCurve. Default is 1, no other type are supported
 */
void
MimmoGeometry::setGeometry(int type){
    if(type > 4)    type = 1;
    int type_ = std::max(type,1);
    m_geometry = NULL;
    m_intgeo.reset(nullptr);
    std::unique_ptr<MimmoObject> dum(new MimmoObject(type_));
    m_intgeo = std::move(dum);
    m_isInternal = true;
};

/*!
 * Return a pointer to the Vertex structure of the MimmoObject geometry actually pointed or allocated by
 * the class. If no geometry is actually available return a nullptr
 * \return pointer to the vertices structure
 */
bitpit::PiercedVector<bitpit::Vertex> *
MimmoGeometry::getVertices(){
    if(isEmpty())    return NULL;
    return &(getGeometry()->getVertices());
};

/*!
 * Return a pointer to the Cell structure of the MimmoObject geometry actually pointed or allocated by
 * the class. If no geometry is actually available return a nullptr
 * \return pointer to the cells structure
 */
bitpit::PiercedVector<bitpit::Cell> * MimmoGeometry::getCells(){
    if(isEmpty())    return NULL;
    return    &(getGeometry()->getCells());

};

/*!
 * Wrapping to set vertices to your internal MimmoObject structure ONLY. Vertices in the internal
 * structure will be erased and substituted by the new ones. Connectivity information in a pre-existent
 * cell list will be lost, so be careful. If an internal object is not
 * allocated and the class is pointing to an external MimmoObject does nothing and return. Return without
 * doing anything even in case of argument pointing to nullptr;
 * \param[in] vertices pointer to a vertex PiercedVector structure
 */
void
MimmoGeometry::setVertices(bitpit::PiercedVector<bitpit::Vertex> * vertices){
    if(m_intgeo.get() == NULL || vertices == NULL) return ;
    m_intgeo->setVertices(*vertices);
};

/*!
 * Wrapping to set cells to your internal MimmoObject structure ONLY. Cells in the internal
 * structure will be erased and substituted by the new ones. If an internal object is not
 * allocated and the class is pointing to an external MimmoObject does nothing and return. Return without
 * doing anything even in case of argument pointing to nullptr;
 * \param[in] cells pointer to a cell PiercedVector structure
 */
void
MimmoGeometry::setCells(bitpit::PiercedVector<bitpit::Cell> * cells){
    if(m_intgeo.get() == NULL || cells == NULL) return;
    m_intgeo->setCells(*cells);
};

/*!It sets the PIDs of all the cells of the geometry Patch.
 * \param[in] pids PIDs of the cells of geometry mesh, in compact sequential order. If pids size does not match number of current cell does nothing
 */
void
MimmoGeometry::setPID(livector1D pids){
    getGeometry()->setPID(pids);
};

/*!It sets the PIDs of part of/all the cells of the geometry Patch.
 * \param[in] pidsMap PIDs map list w/ id of the cell as first value, and pid as second value.
 */
void
MimmoGeometry::setPID(std::unordered_map<long, long> pidsMap){
    getGeometry()->setPID(pidsMap);
};

/*!
 * Set a reference PID for the whole geometry cell. If the geometry is already pidded, all existent pid's
 * will be translated w.r.t. the value of the reference pid. Default value is 0. Maximum value allowed is 30000.
 * The effect of reference pidding will be available only after the execution of the class. After execution, reference PID will
 * be reset ot zero.
 * \param[in] pid reference pid to assign to the geometry cells
 */
void
MimmoGeometry::setReferencePID(long pid){
    m_refPID = std::max(long(0),pid);
}

/*!It sets if the SkdTree of the patch has to be built during execution.
 * \param[in] build If true the SkdTree is built in execution and stored in
 * the related MimmoObject member.
 */
void
MimmoGeometry::setBuildBvTree(bool build){
    setBuildSkdTree(build);
}

/*!It sets if the SkdTree of the patch has to be built during execution.
 * \param[in] build If true the SkdTree is built in execution and stored in
 * the related MimmoObject member.
 */
void
MimmoGeometry::setBuildSkdTree(bool build){
    m_buildSkdTree = build;
}

/*!It sets if the KdTree of the patch has to be built during execution.
 * \param[in] build If true the KdTree is built in execution and stored in
 * the related MimmoObject member.
 */
void
MimmoGeometry::setBuildKdTree(bool build){
    m_buildKdTree = build;
}

/*!
 * Check if geometry is not linked or not locally instantiated in your class.
 * True - no geometry present, False otherwise.
 * \return is the geometry empty?
 */
bool MimmoGeometry::isEmpty(){
    if(getGeometry() == NULL)   return true;
    return getGeometry()->isEmpty();
}

/*!
 * Check if geometry is not linked or not locally instantiated in your class.
 * True - no geometry present, False otherwise. Const overloading.
 * \return is the geometry empty?
 */
bool MimmoGeometry::isEmpty() const{
    if(getGeometry() == NULL)   return true;
    return getGeometry()->isEmpty();
}

/*!
 * Check if geometry is internally instantiated (true) or externally linked(false).
 * Return false if no geometry is checked. Please verify it with isEmpty method first
 * \return is the geometry internally instantiated?
 */
bool MimmoGeometry::isInternal(){
    return (m_isInternal);
}

/*!
 * Clear all stuffs in your class
 */
void
MimmoGeometry::clear(){
    setDefaults();
    m_intgeo.reset(nullptr);
    BaseManipulation::clear();
};

/*!It sets the format to export .nas file.
 * \param[in] wform Format of .nas file (Short/Long).
 */
void
MimmoGeometry::setFormatNAS(WFORMAT wform){
    m_wformat = wform;
}

/*!It writes the mesh geometry on output .vtu file.
 *\return False if geometry is not linked.
 */
bool
MimmoGeometry::write(){

    if (isEmpty()) return false;

    //adjusting with reference pid;
    {
        auto locpids = getGeometry()->getCompactPID();
        auto pidsMap = getGeometry()->getPIDTypeListWNames();
        for( auto & val: locpids) val+=m_refPID;
        setPID(locpids);
        auto pids = getGeometry()->getPIDTypeList();
        for( auto & val: pids) getGeometry()->setPIDName(val, pidsMap[val - m_refPID]);
        //action completed, reset m_refPID to zero.
        m_refPID = 0;
    }
    //writing
    switch(FileType::_from_integral(m_winfo.ftype)){

    case FileType::STL :
        //Export STL
    {
        auto pidsMap = getGeometry()->getPIDTypeListWNames();
        std::unordered_map<int, std::string> mpp;
        for(const auto & touple : pidsMap){
            mpp[touple.first] = touple.second;
        }
        string name = (m_winfo.fdir+"/"+m_winfo.fname+".stl");
        dynamic_cast<SurfUnstructured*>(getGeometry()->getPatch())->exportSTL(name, m_codex, m_multiSolidSTL, false, &mpp);
        return true;
    }
    break;


    case FileType::SURFVTU :
    case FileType::VOLVTU :
    case FileType::CURVEVTU:
        //Export Surface/Volume/3DCurve VTU
    {
        if(!m_codex)    getGeometry()->getPatch()->getVTK().setCodex(bitpit::VTKFormat::ASCII);
        else            getGeometry()->getPatch()->getVTK().setCodex(bitpit::VTKFormat::APPENDED);
        getGeometry()->getPatch()->write(m_winfo.fdir+"/"+m_winfo.fname);
        getGeometry()->getPatch()->getVTK().setCodex(bitpit::VTKFormat::APPENDED);
        return true;
    }
    break;

    case FileType::NAS :
        //Export Nastran file
    {
        liimap mapDataInv;
        dvecarr3E    points = getGeometry()->getVerticesCoords(&mapDataInv);
        livector1D   pointsID = getGeometry()->getVertices().getIds(false);
        livector2D   connectivity = getGeometry()->getCompactConnectivity(mapDataInv);
        livector1D   elementsID = getGeometry()->getCells().getIds(false);
        NastranInterface nastran;
        nastran.setWFormat(m_wformat);
        livector1D pids = getGeometry()->getCompactPID();
        std::unordered_set<long>  pidsset = getGeometry()->getPIDTypeList();
        if (pids.size() == connectivity.size()){
            nastran.write(m_winfo.fdir,m_winfo.fname,points, pointsID, connectivity,elementsID, &pids, &pidsset);
        }else{
            nastran.write(m_winfo.fdir,m_winfo.fname,points, pointsID, connectivity, elementsID);
        }
        return true;
    }
    break;

    case FileType::OFP : //Export ascii OpenFOAM point cloud
    {

        dvecarr3E points = getGeometry()->getVerticesCoords();
        writeOFP(m_winfo.fdir, m_winfo.fname, points);
        return true;
    }
    break;

    case FileType::PCVTU :
        //Export Point Cloud VTU
    {
        dvecarr3E    points = getGeometry()->getVerticesCoords();
        ivector2D    connectivity(points.size(), ivector1D(1,0));
        int counter = 0;
        for(auto & c:connectivity){
            c[0] = counter;
            counter++;
        }
        bitpit::VTKUnstructuredGrid  vtk(m_winfo.fdir, m_winfo.fname, bitpit::VTKElementType::VERTEX);
        vtk.setGeomData( bitpit::VTKUnstructuredField::POINTS, points) ;
        vtk.setGeomData( bitpit::VTKUnstructuredField::CONNECTIVITY, connectivity) ;
        vtk.setDimensions(connectivity.size(), points.size());
        if(!m_codex)    vtk.setCodex(bitpit::VTKFormat::ASCII);
        else            vtk.setCodex(bitpit::VTKFormat::APPENDED);
        vtk.write() ;
        return true;
    }
    break;

    case FileType::MIMMO :
        //Export in mimmo (bitpit) dump format
    {
        string name = (m_winfo.fdir+"/"+m_winfo.fname+".geomimmo");
        std::filebuf buffer;
        std::ostream out(&buffer);
        buffer.open(name, std::ios::out);
        if (buffer.is_open()){
            getGeometry()->dump(out);
            buffer.close();
        }
        return true;
    }
    break;

    default: //never been reached
        break;
    }

    return false;
};

/*!It reads the mesh geometry from an input file and reverse it in the internal
 * MimmoObject container. If an external container is linked skip reading and doing nothing.
 * \return False if file doesn't exists or not found geometry container address.
 */
bool
MimmoGeometry::read(){

    if(!m_isInternal) return false;

//#if MIMMO_ENABLE_MPI
//    if (m_rank == 0) {
//#endif

    switch(FileType::_from_integral(m_rinfo.ftype)){

    //Import STL
    case FileType::STL :
    {
        setGeometry(1);
        string name;

        {
            std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname+".stl");
            bool check = infile.good();
            name = m_rinfo.fdir+"/"+m_rinfo.fname+".stl";
            if (!check){
                infile.open(m_rinfo.fdir+m_rinfo.fname+".STL");
                check = infile.good();
                name = m_rinfo.fdir+"/"+m_rinfo.fname+".STL";
                if (!check) return false;
            }
        }

        ifstream in(name);
        string    ss, sstype;
        getline(in,ss);
        stringstream ins;
        ins << ss;
        ins >> sstype;
        bool binary = true;
        if (sstype == "solid" || sstype == "SOLID") binary = false;
        in.close();

        std::unordered_map<int,std::string> mapPIDSolid;
        dynamic_cast<SurfUnstructured*>(getGeometry()->getPatch())->importSTL(name, binary, 0, false, &mapPIDSolid);

        getGeometry()->cleanGeometry();

        //count PID if multi-solid
        auto & mapset = getGeometry()->getPIDTypeList();
        for(const auto & cell : getGeometry()->getCells() ){
            mapset.insert(cell.getPID());
        }
        std::unordered_map<long, std::string> & mapWNames = getGeometry()->getPIDTypeListWNames();
        for(const auto & val :mapset ){
            mapWNames[val] = mapPIDSolid[val];
        }

    }
    break;

    case FileType::SURFVTU :
        //Import Surface VTU meshes
    {
        std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname+".vtu");
        bool check = infile.good();
        if (!check) return false;

        setGeometry(1);
        VTUGridStreamer vtustreamer;
        VTUGridReader  input(m_rinfo.fdir, m_rinfo.fname, vtustreamer, *(getGeometry()->getPatch()));

        input.read() ;

        getGeometry()->resyncPID();
    }
    break;

    case FileType::VOLVTU :
        //Import Volume VTU meshes
    {
        std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname+".vtu");
        bool check = infile.good();
        if (!check) return false;

        setGeometry(2);
        VTUGridStreamer vtustreamer;
        VTUGridReader  input(m_rinfo.fdir, m_rinfo.fname, vtustreamer, *(getGeometry()->getPatch()));
        input.read() ;

        getGeometry()->resyncPID();

    }
    break;

    case FileType::NAS :
        //Import Surface NAS
    {
        std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname+".nas");
        bool check = infile.good();
        if (!check) return false;
        infile.close();

        dvecarr3E    Ipoints ;
        livector2D    Iconnectivity ;
        livector1D    pointsID;
        livector1D    cellsID;

        NastranInterface nastran;
        nastran.setWFormat(m_wformat);

        livector1D pids;
        nastran.read(m_rinfo.fdir, m_rinfo.fname, Ipoints, pointsID, Iconnectivity, cellsID, pids );

        bitpit::ElementType eltype;

        setGeometry(1);

        int sizeV, sizeC;
        sizeV = Ipoints.size();
        sizeC = Iconnectivity.size();
        m_intgeo->getPatch()->reserveVertices(sizeV);
        m_intgeo->getPatch()->reserveCells(sizeC);

        int counter = 0;
        for(const auto & vv : Ipoints){
            m_intgeo->addVertex(vv, pointsID[counter]);
            ++counter;
        }
        counter = 0;
        for(const auto & cc : Iconnectivity)    {
            eltype = bitpit::ElementType::UNDEFINED;
            std::size_t ccsize = cc.size();
            if(ccsize == 1)  eltype = bitpit::ElementType::VERTEX;
            if(ccsize == 3)  eltype = bitpit::ElementType::TRIANGLE;
            if(ccsize == 4)  eltype = bitpit::ElementType::QUAD;
            m_intgeo->addConnectedCell(cc, eltype, cellsID[counter]);
            ++counter;
        }
        m_intgeo->setPID(pids);
    }

    break;

    //Import ascii OpenFOAM point cloud
    case FileType::OFP :
    {

        std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname);
        bool check = infile.good();
        if (!check) return false;
        infile.close();

        dvecarr3E    Ipoints;
        readOFP(m_rinfo.fdir, m_rinfo.fname, Ipoints);

        setGeometry(3);

        int sizeV = Ipoints.size();
        m_intgeo->getPatch()->reserveVertices(sizeV);

        for(const auto & vv : Ipoints)        m_intgeo->addVertex(vv);

    }
    break;

    //Import point cloud from vtu
    case FileType::PCVTU :
    {

        std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname+".vtu");
        bool check = infile.good();
        if (!check) return false;

        setGeometry(3);
        VTUPointCloudStreamer vtustreamer;
        VTUGridReader  input(m_rinfo.fdir, m_rinfo.fname, vtustreamer, *(getGeometry()->getPatch()), bitpit::VTKElementType::VERTEX);
        input.read() ;

    }
    break;

    case FileType::CURVEVTU :
        //Import 3D Curve VTU
    {
        std::ifstream infile(m_rinfo.fdir+"/"+m_rinfo.fname+".vtu");
        bool check = infile.good();
        if (!check) return false;

        setGeometry(4);
        VTUGridStreamer vtustreamer;
        VTUGridReader  input(m_rinfo.fdir, m_rinfo.fname, vtustreamer, *(getGeometry()->getPatch()));
        input.read() ;

        getGeometry()->resyncPID();
    }
    break;

    case FileType::MIMMO :
        //Import in mimmo (bitpit) restore format
    {
        string name = (m_rinfo.fdir+"/"+m_rinfo.fname+".geomimmo");
        std::filebuf buffer;
        std::istream in(&buffer);
        buffer.open(name, std::ios::in);
        if (buffer.is_open()){
            m_intgeo->restore(in);
            buffer.close();
        }
        return true;
    }
    break;

    default: //never been reached
        break;

    }

    //adjusting with reference pid;
    auto locpids = getGeometry()->getCompactPID();
    auto pidsMap = getGeometry()->getPIDTypeListWNames();
    for( auto & val: locpids) val+=m_refPID;
    setPID(locpids);
    auto pids = getGeometry()->getPIDTypeList();
    for( auto & val: pids) getGeometry()->setPIDName(val, pidsMap[val - m_refPID]);

//#if MIMMO_ENABLE_MPI
//    }
//#endif

    //action completed, reset m_refPID to zero.
    m_refPID = 0;

    return true;
};

/*!Execution command.
 * It reads the geometry if the condition m_read is true.
 * It writes the geometry if the condition m_write is true.
 */
void
MimmoGeometry::execute(){
    bool check = true;
    if (m_read) check = read();
    if (!check){
        (*m_log) << m_name << " error: file not found : "<< m_rinfo.fname << std::endl;
        (*m_log) << " " << std::endl;
        throw std::runtime_error (m_name + " : file not found : " + m_rinfo.fname);
    }
    check = true;
    if (m_write) check = write();
    if (!check){
        (*m_log) << m_name << " error: write not done : geometry not linked " << std::endl;
        (*m_log) << " " << std::endl;
//        throw std::runtime_error (m_name + " : write not done : geometry not linked ");
        return;
    }
    if (m_buildSkdTree) getGeometry()->buildSkdTree();
    if (m_buildKdTree) getGeometry()->buildKdTree();
}

/*!
 *    Read openFoam format geometry file and absorb it as a point cloud ONLY.
 *\param[in]    inputDir    folder of file
 *\param[in]    surfaceName    name of file
 *\param[out]    points        list of points in the cloud
 *
 */
void MimmoGeometry::readOFP(string& inputDir, string& surfaceName, dvecarr3E& points){

    ifstream is(inputDir +"/"+surfaceName);

    points.clear();
    int ip = 0;
    int np;
    darray3E point;
    string sread;
    char par;

    for (int i=0; i<18; i++){
        getline(is,sread);
    }
    is >> np;
    getline(is,sread);
    getline(is,sread);

    points.resize(np);
    while(!is.eof() && ip<np){
        is.get(par);
        for (int i=0; i<3; i++) is >> point[i];
        is.get(par);
        getline(is,sread);
        points[ip] = point;
        ip++;
    }
    is.close();
}
/*!
 *    Write geometry file in openFoam format as a point cloud ONLY.
 *\param[in]    outputDir    folder of file
 *\param[in]    surfaceName    name of file
 *\param[out]    points        list of points in the cloud
 *
 */
void MimmoGeometry::writeOFP(string& outputDir, string& surfaceName, dvecarr3E& points){

    ofstream os(outputDir +"/"+surfaceName);
    char nl = '\n';

    string separator(" ");
    string parl("("), parr(")");
    string hline;

    hline = "/*--------------------------------*- C++ -*----------------------------------*\\" ;
    os << hline << nl;
    hline = "| =========                 |                                                 |";
    os << hline << nl;
    hline = "| \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |";
    os << hline << nl;
    hline = "|  \\\\    /   O peration     | Version:  2.4.x                                 |";
    os << hline << nl;
    hline = "|   \\\\  /    A nd           | Web:      www.OpenFOAM.org                      |";
    os << hline << nl;
    hline = "|    \\\\/     M anipulation  |                                                 |";
    os << hline << nl;
    hline = "\\*---------------------------------------------------------------------------*/";
    os << hline << nl;
    hline = "FoamFile";
    os << hline << nl;
    hline = "{";
    os << hline << nl;
    hline = "    version     2.0;";
    os << hline << nl;
    hline = "    format      ascii;";
    os << hline << nl;
    hline = "    class       vectorField;";
    os << hline << nl;
    hline = "    location    \"constant/polyMesh\";";
    os << hline << nl;
    hline = "    object      points;";
    os << hline << nl;
    hline = "}";
    os << hline << nl;
    hline = "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //";
    os << hline << nl;
    os << nl;
    os << nl;
    int np = points.size();
    os << np << nl;
    os << parl << nl;
    for (int i=0; i<np; i++){
        os << parl;
        for (int j=0; j<2; j++){
            os << setprecision(16) << points[i][j] << separator;
        }
        os << setprecision(16) << points[i][2] << parr << nl;
    }
    os << parr << nl;
    os << nl;
    os << nl;
    hline = "// ************************************************************************* //";
    os << hline << nl;

    os.close();
}

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
MimmoGeometry::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    std::string input;

    BaseManipulation::absorbSectionXML(slotXML, name);

    if(slotXML.hasOption("IOMode")){
        input = slotXML.get("IOMode");
        input = bitpit::utils::string::trim(input);
        if(!input.empty()){
            for(auto c: IOMode::_values()){
                if(input == c._to_string()){
                    setIOMode(c);
                }
            }
        }else{
            setIOMode(0);
        }
    };

    if(slotXML.hasOption("FileType")){
        input = slotXML.get("FileType");
        input = bitpit::utils::string::trim(input);
        if(!input.empty()){
            for(auto c: FileType::_values()){
                if(input == c._to_string()){
                    setFileType(c);
                }
            }
        }else{
            setFileType(0);
        }
    };

    if(slotXML.hasOption("ReadFileType")){
        input = slotXML.get("ReadFileType");
        input = bitpit::utils::string::trim(input);
        if(!input.empty()){
            for(auto c: FileType::_values()){
                if(input == c._to_string()){
                    setReadFileType(c);
                }
            }
        }
    };

    if(slotXML.hasOption("WriteFileType")){
        input = slotXML.get("WriteFileType");
        input = bitpit::utils::string::trim(input);
        if(!input.empty()){
            for(auto c: FileType::_values()){
                if(input == c._to_string()){
                    setWriteFileType(c);
                }
            }
        }
    };

    if(slotXML.hasOption("Dir")){
        input = slotXML.get("Dir");
        input = bitpit::utils::string::trim(input);
        if(input.empty())    input = "./";
        setDir(input);
    };


    if(slotXML.hasOption("Filename")){
        input = slotXML.get("Filename");
        input = bitpit::utils::string::trim(input);
        if(input.empty())    input = "mimmoGeometry";
        setFilename(input);
    };

    if(slotXML.hasOption("ReadDir")){
        input = slotXML.get("ReadDir");
        input = bitpit::utils::string::trim(input);
        if(!input.empty())
            setReadDir(input);
    };


    if(slotXML.hasOption("ReadFilename")){
        input = slotXML.get("ReadFilename");
        input = bitpit::utils::string::trim(input);
        if(!input.empty())
            setReadFilename(input);
    };

    if(slotXML.hasOption("WriteDir")){
        input = slotXML.get("WriteDir");
        input = bitpit::utils::string::trim(input);
        if(!input.empty())
            setWriteDir(input);
    };


    if(slotXML.hasOption("WriteFilename")){
        input = slotXML.get("WriteFilename");
        input = bitpit::utils::string::trim(input);
        if(!input.empty())
            setWriteFilename(input);
    };

    if(slotXML.hasOption("Codex")){
        input = slotXML.get("Codex");
        bool value = true;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setCodex(value);
    };

    //to be deprecated BvTree field option. Substituted by SkdTree.
    if(slotXML.hasOption("BvTree")){
        input = slotXML.get("BvTree");
        bool value = false;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setBuildSkdTree(value);
    };

    if(slotXML.hasOption("SkdTree")){
        input = slotXML.get("SkdTree");
        bool value = false;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setBuildSkdTree(value);
    };


    if(slotXML.hasOption("KdTree")){
        input = slotXML.get("KdTree");
        bool value = false;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setBuildKdTree(value);
    };

    if(slotXML.hasOption("AssignRefPID")){
        input = slotXML.get("AssignRefPID");
        long value = 0;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setReferencePID(value);
    };

    if(slotXML.hasOption("WriteMultiSolidSTL")){
        input = slotXML.get("WriteMultiSolidSTL");
        bool value = true;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setMultiSolidSTL(value);
    };

};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
MimmoGeometry::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    BaseManipulation::flushSectionXML(slotXML, name);

    std::string output;

    {
        std::string temp = (IOMode::_from_integral(m_write+m_read*m_write))._to_string();
        slotXML.set("IOMode", temp);
    }

    if (m_read && m_write)
    {
        slotXML.set("ReadDir", m_rinfo.fdir);
        slotXML.set("ReadFilename", m_rinfo.fname);
        slotXML.set("WriteDir", m_winfo.fdir);
        slotXML.set("WriteFilename", m_winfo.fname);
    }
    else{
        slotXML.set("Dir", m_rinfo.fdir);
        slotXML.set("Filename", m_rinfo.fname);
    }

    if (m_read && m_write)
    {
        std::string temp = (FileType::_from_integral(m_rinfo.ftype))._to_string();
        slotXML.set("ReadFileType", temp);
        temp = (FileType::_from_integral(m_winfo.ftype))._to_string();
        slotXML.set("WriteFileType", temp);
    }
    else
    {
        std::string temp = (FileType::_from_integral(m_rinfo.ftype))._to_string();
        slotXML.set("FileType", temp);
    }

    output = std::to_string(m_codex);
    slotXML.set("Codex", output);

    output = std::to_string(m_buildSkdTree);
    slotXML.set("SkdTree", output);

    output = std::to_string(m_buildKdTree);
    slotXML.set("KdTree", output);
    slotXML.set("AssignRefPID", std::to_string(m_refPID));
    slotXML.set("WriteMultiSolidSTL", std::to_string(m_multiSolidSTL));
};



//===============================//
//====== NASTRAN INTERFACE ======//
//===============================//

/*!
 * Set the format Short/Long of data in your nastran file
 * \param[in] wf WFORMAT Short/Long of your nastran file
 */
void NastranInterface::setWFormat(WFORMAT wf){
    wformat = wf;
}

/*!
 * Write a keyword string according to WFORMAT chosen by the User
 * \param[in]     key    keyword
 * \param[in,out] os    ofstream where the keyword is written
 */
void NastranInterface::writeKeyword(std::string key, std::ofstream& os){
    os.setf(ios_base::left);
    switch (wformat)
    {
    case Short:
    {
        os << setw(8) << key;
        break;
    }
    case Long:
    {
        os << setw(8) << string(key + '*');
        break;
    }
    }
    os.unsetf(ios_base::left);
}

/*!
 * Write a 3D point to an ofstream
 * \param[in]     p         point
 * \param[in]       pointI    point label
 * \param[in,out] os        ofstream where the point is written
 */
void NastranInterface::writeCoord(const darray3E& p, const long& pointI, std::ofstream& os){
    // Fixed short/long formats:
    // 1 GRID
    // 2 ID : point ID - requires starting index of 1
    // 3 CP : co-ordinate system ID (blank)
    // 4 X1 : point x cp-ordinate
    // 5 X2 : point x cp-ordinate
    // 6 X3 : point x cp-ordinate
    // 7 CD : co-ordinate system for displacements (blank)
    // 8 PS : single point constraints (blank)
    // 9 SEID : super-element ID

    string separator_("");
    writeKeyword(string("GRID"), os);
    os << separator_;
    os.setf(ios_base::right);
    writeValue(pointI, os);
    os << separator_;
    writeValue("", os);
    os << separator_;
    writeValue(p[0], os);
    os << separator_;
    writeValue(p[1], os);
    os << separator_;
    switch (wformat)
    {
    case Short:
    {
        writeValue(p[2], os);
        os << nl;
        //        os << setw(8) << p[2]
        //                           << nl;
        os.unsetf(ios_base::right);
        break;
    }
    case Long:
    {
        os << nl;
        os.unsetf(ios_base::right);
        writeKeyword("", os);
        os.setf(ios_base::right);
        writeValue(p[2], os);
        os << nl;
        break;
    }
    default:
    {
        throw std::runtime_error ("NastranInterface : Unknown writeFormat enumeration");
    }
    }
    os.unsetf(ios_base::right);
}

/*!
 * Write a face to an ofstream
 * \param[in]     faceType     string reporting label fo the type of element
 * \param[in]       facePts    Element Points connectivity
 * \param[in]       nFace        element label
 * \param[in,out] os        ofstream where the face is written
 * \param[in]     PID       part identifier associated to the element
 */
void NastranInterface::writeFace(string faceType, const livector1D& facePts, const long& nFace, ofstream& os,long PID){
    // Only valid surface elements are CTRIA3 and CQUAD4

    // Fixed short/long formats:
    // 1 CQUAD4
    // 2 EID : element ID
    // 3 PID : property element ID; default = EID (blank)
    // 4 G1 : grid point index - requires starting index of 1
    // 5 G2 : grid point index
    // 6 G3 : grid point index
    // 7 G4 : grid point index
    // 8 onwards - not used

    // For CTRIA3 elements, cols 7 onwards are not used

    WFORMAT wformat_ = wformat;
    wformat = Short;
    string separator_("");
    writeKeyword(faceType, os);
    os << separator_;
    os.setf(ios_base::right);
    writeValue(nFace, os);
    os << separator_;
    writeValue(PID, os);
    int fp1;
    switch (wformat)
    {
    case Short:
    {
        for (int i=0; i< (int)facePts.size(); i++)
        {
            fp1 = facePts[i] + 1;
            writeValue(fp1, os);
        }

        break;
    }
    case Long:
    {
        for (int i=0; i< (int)facePts.size(); i++)
        {
            fp1 = facePts[i] + 1;
            writeValue(fp1, os);
            //            if (i == 1)
            //            {
            //                os << nl;
            //                os.unsetf(ios_base::right);
            //                writeKeyword("", os);
            //                os.setf(ios_base::right);
            //            }
        }
        break;
    }
    default:
    {
        cout << "Unknown writeFormat enumeration" << endl;
        throw std::runtime_error ("Unknown writeFormat enumeration");
    }
    }
    os << nl;
    os.unsetf(ios_base::right);
    wformat = wformat_;
}

/*!
 * Write a face to an ofstream
 * \param[in]     points     list of vertices
 * \param[in]     pointsID   list of vertices labels
 * \param[in]     faces        element points connectivity
 * \param[in]     facesID   list of element labels
 * \param[in,out] os        ofstream where the geometry is written
 * \param[in]     PIDS      list of Part Identifiers for each cells (optional)
 * \return true if the geometry is correctly written.
 */
bool NastranInterface::writeGeometry(dvecarr3E& points, livector1D& pointsID, livector2D& faces, livector1D & facesID,
                                     ofstream& os, livector1D* PIDS){

    if(points.size() != pointsID.size())    return false;
    if(faces.size() != facesID.size())    return false;
    bool flagpid = (PIDS != NULL);
    if(flagpid && (PIDS->size() != faces.size())) return false;
    int counter =  0;
    for (const auto & p: points){
        writeCoord(p, pointsID[counter], os);
        ++counter;
    }

    counter = 0;
    for (const auto & f: faces){
        long PID = 1;
        if (flagpid) PID = (*PIDS)[counter];
        if (f.size() == 3){
            writeFace("CTRIA3", f, facesID[counter], os, PID);
        }
        else if (f.size() == 4){
            writeFace("CQUAD4", f, facesID[counter], os, PID);
        }else{
            throw std::runtime_error ("Error NastranInterface::writeGeometry : unknown face format");
        }
    }

    return true;
}

/*!
 * Write nastran footer to an ofstream
 * \param[in,out] os        ofstream where the footer is written
 * \param[in]     PIDSSET   list of overall available part identifiers
 */
void NastranInterface::writeFooter(ofstream& os, std::unordered_set<long>* PIDSSET){
    string separator_("");
    if (PIDSSET == NULL){
        int PID = 1;
        writeKeyword("PSHELL", os);
        os << separator_;
        writeValue(PID, os);
        for (int i = 0; i < 6; i++)
        {
            // Dummy values
            double uno = 1.0;
            os << separator_;
            writeValue(uno, os);
        }
        os << nl;
    }
    else{
        for (std::unordered_set<long>::iterator it = PIDSSET->begin(); it != PIDSSET->end(); it++)
        {
            writeKeyword("PSHELL", os);
            int PID = (*it);
            os << separator_;
            writeValue(PID, os);
            for (int i = 0; i < 6; i++)
            {
                // Dummy values
                double uno = 1.0;
                os << separator_;
                writeValue(uno, os);
            }
            os << nl;

        }
    }

    writeKeyword("MAT1", os);
    os << separator_;
    int MID = 1;
    writeValue(MID, os);
    for (int i = 0; i < 7; i++)
    {
        // Dummy values
        os << separator_;
        writeValue("", os);
    }
    os << nl;
}

/*!
 * Write a bdf nastran file
 * \param[in] outputDir    output directory
 * \param[in] surfaceName    output filename
 * \param[in] points    reference of a point container that has to be written
 * \param[in] pointsID  reference of a label point container
 * \param[in] faces     reference to element-point connectivity that has to be written
 * \param[in] facesID   reference to a label element container
 * \param[in] PIDS        reference to long int vector for Part Identifiers that need to be associated to elements
 * \param[in] PIDSSET    map of overall available Part Identifiers
 */
void NastranInterface::write(string& outputDir, string& surfaceName, dvecarr3E& points, livector1D & pointsID,
                             livector2D& faces, livector1D & facesID, livector1D* PIDS, std::unordered_set<long>* PIDSSET){

    ofstream os(outputDir +"/"+surfaceName + ".nas");
    os << "TITLE=mimmo " << surfaceName << " mesh" << nl
            << "$" << nl
            << "BEGIN BULK" << nl;
    bool check = writeGeometry(points, pointsID,  faces, facesID, os, PIDS);
    if(!check){
        throw std::runtime_error ("Error in NastranInterface::write : geometry mesh cannot be written");
    }
    writeFooter(os, PIDSSET);
    os << "ENDDATA" << endl;
}

//========READ====//
/*!
 * Read a bdf nastran file
 * \param[in] inputDir    input directory
 * \param[in] surfaceName    input filename
 * \param[out] points    reference of a point container that has to be filled
 * \param[out] pointsID  reference to a label point container to be filled
 * \param[out] faces     reference to element-point connectivity that has to be filled
 * \param[out] facesID  reference to a label element container to be filled
 * \param[out] PIDS        reference to long int vector for Part Identifier storage
 */
void NastranInterface::read(string& inputDir, string& surfaceName, dvecarr3E& points, livector1D & pointsID,
                            livector2D& faces, livector1D & facesID, livector1D& PIDS){

    ifstream is(inputDir +"/"+surfaceName + ".nas");

    points.clear();
    pointsID.clear();
    faces.clear();
    facesID.clear();
    PIDS.clear();

    long ipoint = 0;
    long iface  = 0;
    darray3E point;
    livector1D face;
    long pid;
    string sread;
    string ssub = trim(sread.substr(0,8));

    while(!is.eof()){
        while(ssub != "GRID" &&
                ssub != "GRID*" &&
                ssub != "CTRIA3" &&
                ssub != "CTRIA3*" &&
                ssub != "CQUAD4" &&
                ssub != "CQUAD4*" &&
                ssub != "RBE3" &&
                !is.eof()){

            getline(is,sread);
            ssub = trim(sread.substr(0,8));

        }
        if(ssub == "GRID"){
            ipoint = stoi(sread.substr(8,8));
            point[0] = stod(convertVertex(trim(sread.substr(24,8))));
            point[1] = stod(convertVertex(trim(sread.substr(32,8))));
            point[2] = stod(convertVertex(trim(sread.substr(40,8))));
            points.push_back(point);
            pointsID.push_back(ipoint);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
        else if(ssub == "GRID*"){
            ipoint = stoi(sread.substr(16,16));
            point[0] = stod(convertVertex(trim(sread.substr(48,16))));
            point[1] = stod(convertVertex(trim(sread.substr(64,16))));
            point[2] = stod(convertVertex(trim(sread.substr(80,16))));
            points.push_back(point);
            pointsID.push_back(ipoint);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
        else if(ssub == "CTRIA3"){
            face.resize(3);
            iface = stoi(sread.substr(8,8));
            pid   = stoi(sread.substr(16,8));
            face[0] = stoi(sread.substr(24,8));
            face[1] = stoi(sread.substr(32,8));
            face[2] = stoi(sread.substr(40,8));
            faces.push_back(face);
            facesID.push_back(iface);
            PIDS.push_back(pid);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
        else if(ssub == "CTRIA3*"){
            face.resize(3);
            iface = stoi(sread.substr(16,16));
            pid   = stoi(sread.substr(32,16));
            face[0] = stoi(sread.substr(48,16));
            face[1] = stoi(sread.substr(64,16));
            face[2] = stoi(sread.substr(80,16));
            faces.push_back(face);
            facesID.push_back(iface);
            PIDS.push_back(pid);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
        else if(ssub == "CQUAD4"){
            face.resize(4);
            iface = stoi(sread.substr(8,8));
            pid = stoi(sread.substr(16,8));
            face[0] = stoi(sread.substr(24,8));
            face[1] = stoi(sread.substr(32,8));
            face[2] = stoi(sread.substr(40,8));
            face[3] = stoi(sread.substr(48,8));
            faces.push_back(face);
            facesID.push_back(iface);
            PIDS.push_back(pid);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
        else if(ssub == "CQUAD4*"){
            face.resize(3);
            iface = stoi(sread.substr(16,16));
            pid = stoi(sread.substr(32,16));
            face[0] = stoi(sread.substr(48,16));
            face[1] = stoi(sread.substr(64,16));
            face[2] = stoi(sread.substr(80,16));
            face[3] = stoi(sread.substr(96,16));
            faces.push_back(face);
            facesID.push_back(iface);
            PIDS.push_back(pid);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
        else if(ssub == "RBE3"){
        	//RBE3 is read as single vertex with its id
            face.resize(1);
            iface = stoi(sread.substr(8,8));
            std::string dummy = (sread.substr(16,8));
            face[0] = stoi(sread.substr(24,8));
            faces.push_back(face);
            facesID.push_back(iface);
            PIDS.push_back(0);
            getline(is,sread);
            ssub = trim(sread.substr(0,8));
        }
    }
    is.close();
}

/*!
 * Custom trimming of nas string
 * \return trimmed string
 */
string
NastranInterface::trim(string in){

    stringstream out;
    out << in;
    out >> in;
    return in;
}

/*!
 * Convert nas string of numerical floats
 * \return string of regular floats
 */
string
NastranInterface::convertVertex(string in){
    int pos = in.find_last_of("-");
    if (pos<(int)in.size() && pos > 0){
        in.insert(pos, "E");
    }
    else{
        pos = in.find_last_of("+");
        if (pos<(int)in.size() && pos > 0){
            in.insert(pos, "E");
        }
    }
    return in;
}

}
