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
#include "IOVTKScalar.hpp"
#include <vtkSmartPointer.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkTriangleFilter.h>
#include <vtkCellArray.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

using namespace std;
namespace mimmo{

/*!Default constructor of IOVTKScalar.
 */
IOVTKScalar::IOVTKScalar(){
    m_name         = "mimmo.IOVTKScalar";
    m_read        = false;
    m_rfilename    = "mimmoVTKScalar";
    m_write        = false;
    m_wfilename    = "mimmoVTKScalar";
    m_rdir        = "./";
    m_wdir        = "./";
    m_polydata    = NULL;
    m_polydataInternal = false;
    m_local        = false;
    m_normalize    = true;
    m_scaling    = 1.0;
}


/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
IOVTKScalar::IOVTKScalar(const bitpit::Config::Section & rootXML){

    m_name      = "mimmo.IOVTKScalar";
    m_read      = false;
    m_rfilename = "mimmoVTKScalar";
    m_write     = false;
    m_wfilename = "mimmoVTKScalar";
    m_rdir      = "./";
    m_wdir      = "./";
    m_polydata  = NULL;
    m_polydataInternal = false;
    m_local     = false;
    m_normalize = true;
    m_scaling   = 1.0;

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.IOVTKScalar"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!Default destructor of IOVTKScalar.
 */
IOVTKScalar::~IOVTKScalar(){
    if (m_local) delete getGeometry();
    if (m_polydataInternal) m_polydata->Delete();
    m_polydataInternal = NULL;
};

/*!Copy constructor of IOVTKScalar. If to-be-copied class has an internal MimmoObject
 * locally instantiated, this will be soft linked (copied its pointer only)
 * in the current class;
 */
IOVTKScalar::IOVTKScalar(const IOVTKScalar & other):BaseManipulation(other){
    m_read = other.m_read;
    m_rfilename = other.m_rfilename;
    m_write = other.m_write;
    m_wfilename = other.m_wfilename;
    m_rdir = other.m_rdir;
    m_wdir = other.m_wdir;
    m_polydata = other.m_polydata;
    m_polydataInternal = false;
    m_field = other.m_field;
    m_normalize = other.m_normalize;
    m_scaling = other.m_scaling;
    m_local = false;
    
};

/*!Assignment operator. If to-be-copied class has an internal MimmoObject
 * locally instantiated, this will be soft linked (copied its pointer only)
 * in the current class;
 */
IOVTKScalar& IOVTKScalar::operator=(IOVTKScalar other){
        swap(other);
        return *this;
};

/*!
 * Swap function
 * \param[in] x object to be swapped
 */
void IOVTKScalar::swap(IOVTKScalar & x) noexcept
{
   std::swap(m_read, x.m_read);
   std::swap(m_rfilename, x.m_rfilename);
   std::swap(m_write, x.m_write);
   std::swap(m_wfilename, x.m_wfilename);
   std::swap(m_rdir, x.m_rdir);
   std::swap(m_wdir, x.m_wdir);
   std::swap(m_polydata, x.m_polydata);
   std::swap(m_polydataInternal, x.m_polydataInternal);
   //std::swap(m_field, x.m_field);
   m_field.swap(x.m_field);
   std::swap(m_normalize, x.m_normalize);
   std::swap(m_scaling, x.m_scaling);
   std::swap(m_local, x.m_local);
    BaseManipulation::swap(x);
}

/*! It builds the input/output ports of the object
 */
void
IOVTKScalar::buildPorts(){
    bool built = true;

    built = (built && createPortIn<MimmoObject*, IOVTKScalar>(this, &IOVTKScalar::setGeometry, M_GEOM));
    built = (built && createPortIn<double, IOVTKScalar>(this, &IOVTKScalar::setScaling, M_VALUED));
    built = (built && createPortIn<vtkPolyData*, IOVTKScalar>(this, &IOVTKScalar::setPolyData, M_POLYDATA_));
    built = (built && createPortIn<dmpvector1D, IOVTKScalar>(this, &IOVTKScalar::setField, M_SCALARFIELD));

    built = (built && createPortOut<MimmoObject*, IOVTKScalar>(this, &IOVTKScalar::getGeometry, M_GEOM));
    built = (built && createPortOut<vtkPolyData*, IOVTKScalar>(this, &IOVTKScalar::getPolyData, M_POLYDATA_));
    built = (built && createPortOut<dmpvector1D, IOVTKScalar>(this, &IOVTKScalar::getField, M_SCALARFIELD));
    m_arePortsBuilt = built;
};

/*!
 * Return pointer to the currently linked VTK PolyData object
 * \return pointer to VTK PolyData object
 */
vtkPolyData*
IOVTKScalar::getPolyData(){
    return m_polydata;
}

/*!
 * Return the currently set scaling parameter
 * \return scaling parameter
 */
double
IOVTKScalar::getScaling(){
    return m_scaling;
}

/*!
 * Returning field associated to the class
 * \return scalar field
 */
dmpvector1D
IOVTKScalar::getField(){
    return m_field;
}

/*!
 * Set the pointer to the target VTK PolyData object
 * \param[in] polydata vtkPolyData object pointer
 */
void
IOVTKScalar::setPolyData(vtkPolyData* polydata){
    if(m_polydataInternal) {
        m_polydata->Delete();
        m_polydataInternal = false;
    }
    m_polydata = polydata;
}

/*!
 * Set the scaling parameter to remodulate field
 * \param[in] scaling scaling parameter
 */
void
IOVTKScalar::setScaling(double scaling){
    m_scaling = scaling;
}

/*!
 * Set the field associated to the mesh.
 * \param[in] field scalar field of doubles
 */
void
IOVTKScalar::setField(dmpvector1D field){
    m_field = field;
}

/*!
 * Activate field normalization w.r.t. the maximum field value found
 * \param[in] normalize true/false to activate field normalization
 */
void
IOVTKScalar::setNormalize(bool normalize){
    m_normalize = normalize;
}

/*!
 * It sets the condition to read the geometry on file during the execution.
 * \param[in] read Does it read the geometry in execution?
 */
void
IOVTKScalar::setRead(bool read){
    m_read = read;
}

/*!
 * It sets the name of directory to read the geometry.
 * \param[in] dir Name of directory.
 */
void
IOVTKScalar::setReadDir(string dir){
    m_rdir = dir;
}

/*!
 * It sets the name of file to read the geometry.
 * \param[in] filename Name of input file.
 */
void
IOVTKScalar::setReadFilename(string filename){
    m_rfilename = filename;
}

/*!
 * It sets the condition to write the geometry on file during the execution.
 * \param[in] write Does it write the geometry in execution?
 */
void
IOVTKScalar::setWrite(bool write){
    m_write = write;
}

/*!
 * It sets the name of directory to write the geometry.
 * \param[in] dir Name of directory.
 */
void
IOVTKScalar::setWriteDir(string dir){
    m_wdir = dir;
}

/*!
 * It sets the name of file to write the geometry.
 * \param[in] filename Name of output file.
 */
void
IOVTKScalar::setWriteFilename(string filename){
    m_wfilename = filename;
}


/*!
 * It reads the mesh geometry from an input file.
 * \return False if file doesn't exist or is not a polydata.
 */
bool
IOVTKScalar::read(){

    std::ifstream infile(m_rdir+"/"+m_rfilename+".vtk");
    bool check = infile.good();
    if (!check) return false;
    infile.close();

    if (getGeometry() == NULL){
        //Local Instantiation of mimmo Object.
        m_local = true;
        MimmoObject* mimmo0 = new MimmoObject(1);
        setGeometry(mimmo0);
    }
    
    string inputFilename = m_rdir+"/"+m_rfilename+".vtk";
    int    np = 0;
    int    nt = 0;
    darray3E point;

    // Get all data from the file
    vtkSmartPointer<vtkGenericDataObjectReader> reader =
            vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    // All of the standard data types can be checked and obtained like this:
    if(reader->IsFilePolyData())
    {
        //unlock m_polydata to NULL;
        if(m_polydataInternal) m_polydata->Delete();
        m_polydata = NULL;
        m_polydata = vtkPolyData::New();
        m_polydata->DeepCopy(reader->GetPolyDataOutput());
        m_polydataInternal = true;
        vtkPolyData* output = reader->GetPolyDataOutput();

        vtkSmartPointer<vtkTriangleFilter> tri= vtkSmartPointer<vtkTriangleFilter>::New();
        tri->SetInputData(output);
        tri->Update();

        vtkPolyData* output2 = tri->GetOutput();

        vtkSmartPointer<vtkPoints> points = output2->GetPoints();
        vtkCellArray *cells = output2->GetPolys();
        //vtkCellData *cdata = output2->GetCellData();
        vtkPointData *pdata = output2->GetPointData();

        double point_[3];
        for (vtkIdType id=0; id<points->GetNumberOfPoints(); id++ ){
            points->GetPoint(id, point_);
            for (int i=0; i<3; i++) point[i] = point_[i];
            getGeometry()->addVertex(point);
        }

        vtkIdType* conn_;
        vtkIdType npts;
        nt = cells->GetNumberOfCells();
        livector1D connectivity(3);
        bitpit::ElementType eltype = bitpit::ElementType::TRIANGLE;
        cells->InitTraversal();
        for (vtkIdType id=0; id<nt; id++ ){
            cells->GetNextCell(npts, conn_);
            for (int i=0; i<npts; i++) connectivity[i] = conn_[i];
            getGeometry()->addConnectedCell(connectivity, eltype);
        }

        np = getGeometry()->getNVertices();
        vtkDataArray* data = pdata->GetArray(0);
        m_field.clear();
        if (data != NULL){
            double maxf = 0.0;
            for (long int i=0; i<np; i++){
                vtkIdType id = i;
                m_field.insert(i, data->GetComponent(id,0));
                maxf = max(maxf, abs(m_field[i]));
            }
            if (m_normalize){
                for (int i=0; i<np; i++){
                    m_field[i] /= maxf;
                    m_field[i] *= m_scaling;
                }
            }
            else{
                for (int i=0; i<np; i++){
                    m_field[i] *= m_scaling;
                }
            }
        }
        m_field.setGeometry(getGeometry());
        m_field.setDataLocation(MPVLocation::POINT);
    }
    else{
        (*m_log) << m_name << " error: polydata not found in : "<< m_rfilename << std::endl;
        return false;
    }

    return true;

}

/*!
 * It writes the mesh geometry on output .vtu file.
 * It modifies the linked polydata with the linked geometry if exists or it writes the linked geometry if only this exists.
 *\return False if not polydata neither geometry are linked.
 */
bool
IOVTKScalar::write(){

    if (m_polydata == NULL && getGeometry() == NULL) return false;

    if (m_polydata != NULL){
        if (getGeometry() != NULL){
            vtkSmartPointer<vtkPoints> points = m_polydata->GetPoints();
            double point_[3];
            darray3E point;
            int np = points->GetNumberOfPoints();
            np = min(np, int(getGeometry()->getNVertices()));
            for (vtkIdType id=0; id<np; id++ ){
                point = getGeometry()->getVertexCoords(id);
                for (int i=0; i<3; i++) point_[i] = point[i];
                points->SetPoint(id, point_);
            }
        }

        string outputFilename = m_wdir+"/"+m_wfilename+".vtk";
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetFileName(outputFilename.c_str());
        writer->SetInputData(m_polydata);
        writer->Write();

    }
    else{

        m_polydata = vtkPolyData::New();
        m_polydata->Allocate();
        m_polydataInternal = true;
        
        /* Set polydata points. */
        vtkPoints* points = vtkPoints::New() ;
        double point_[3];
        darray3E point;
        int np = int(getGeometry()->getNVertices());
        points->SetNumberOfPoints(np);
        for (vtkIdType id=0; id<np; id++ ){
            point = getGeometry()->getVertexCoords(id);
            for (int i=0; i<3; i++) point_[i] = point[i];
            points->SetPoint(id, point_);
        }
        m_polydata->SetPoints(points);
        points->Delete();
        points = NULL;

        /* Set polydata cells. */
        bitpit::PiercedVector<bitpit::Cell> & cells = getGeometry()->getCells();
        for (auto & cell : cells){
            bitpit::ConstProxyVector<long> vList = cell.getVertexIds();
            std::vector<vtkIdType> c (vList.size(), 0);
            int countV = 0;
            for (const auto & iV : vList){
                c[countV] =  vtkIdType(iV);
                ++countV;
            }
            if (vList.size() == 3){
                m_polydata->InsertNextCell(VTK_TRIANGLE, 3, c.data());
            }
            else if (vList.size() == 4 ){
                m_polydata->InsertNextCell(VTK_QUAD, 4, c.data());
            }
            else{
                m_polydata->InsertNextCell(VTK_POLYGON, vList.size(), c.data());
            }
        }

        /* Set polydata field. */
        //check if a valid scalarfield is connected
        bool check = m_field.getGeometry()==getGeometry();
        check = check && m_field.getDataLocation()==MPVLocation::POINT;
        check = check && m_field.completeMissingData(0.0);
        if(check){
            //get MPV as normal vector sequenced as geometry vertices actual ordering.
            dvector1D field = m_field.getDataAsVector();
            
            vtkSmartPointer<vtkPointData> pdata = m_polydata->GetPointData();
            vtkDoubleArray* data = vtkDoubleArray::New();
            data->SetName( "mimmo.field" );
            data->SetNumberOfComponents( 1 );
            for (int i=0; i<np; i++){
                data->InsertNextTuple( &field[i]);
            }
            pdata->AddArray(data);
            data->Delete();
            data = NULL;
        }
        
        string outputFilename = m_wdir+"/"+m_wfilename+".vtk";
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetFileName(outputFilename.c_str());
        writer->SetInputData(m_polydata);
        writer->Write();
    }
    return true;
}

/*!
 * Execution command.
 * It reads the geometry if the condition m_read is true.
 * It writes the geometry if the condition m_write is true.
 */
void
IOVTKScalar::execute(){
    bool check = true;
    if (m_read) check = read();
    if (!check){
        (*m_log) << m_name << " error: file not found : "<< m_rfilename << std::endl;
        (*m_log) << " " << std::endl;
        throw std::runtime_error (m_name + ": file not found : " + m_rfilename);
    }
    if (m_write) check = write();
    if (!check){
        (*m_log) << m_name << " error: write not done : geometry not linked " << std::endl;
        (*m_log) << " " << std::endl;
//        throw std::runtime_error (m_name + ": write not done : geometry not linked ");
        return;
    }
}


/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void 
IOVTKScalar::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

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

    if(slotXML.hasOption("WriteFlag")){
        input = slotXML.get("WriteFlag");
        bool value = false;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setWrite(value);
    };


    if (m_read){
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
    }

    if(m_write){
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
    }

    if(slotXML.hasOption("Normalize")){
        input = slotXML.get("Normalize");
        bool value = false;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setNormalize(value);
    };

    if(slotXML.hasOption("Scaling")){
        input = slotXML.get("Scaling");
        double value = 1.0;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss >> value;
        }
        setScaling(value);
    };
};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void 
IOVTKScalar::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    BaseManipulation::flushSectionXML(slotXML, name);
    std::string output;

    output = std::to_string(uint(m_read));
    slotXML.set("ReadFlag", output);

    if(m_read){
        slotXML.set("ReadDir", m_rdir);
        slotXML.set("ReadFilename", m_rfilename);
    }

    output = std::to_string(uint(m_write));
    slotXML.set("WriteFlag", output);

    if(m_write){
        slotXML.set("WriteDir", m_wdir);
        slotXML.set("WriteFilename", m_wfilename);
    }

    output = std::to_string(uint(m_normalize));
    slotXML.set("Normalize", output);
    slotXML.set("Scaling", std::to_string(m_scaling));
};

}


