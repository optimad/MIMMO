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

#include "RBFBox.hpp"

namespace mimmo{


/*! Basic Constructor.*/
RBFBox::RBFBox(){
    m_name = "mimmo.RBFBox";
    m_origin.fill(0.0);
    m_span.fill(0.0);
    m_suppR = 0.0;
};

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
RBFBox::RBFBox(const bitpit::Config::Section & rootXML){

    m_name = "mimmo.RBFBox";
    m_origin.fill(0.0);
    m_span.fill(0.0);
    m_suppR = 0.0;


    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);
    if(input == "mimmo.RBFBox"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*! Destructor */
RBFBox::~RBFBox(){};

/*! Copy Constructor
 *\param[in] other RBFBox where copy from
 */
RBFBox::RBFBox(const RBFBox & other):BaseManipulation(other){
    m_origin = other.m_origin;
    m_span   = other.m_span;
    m_suppR = other.m_suppR;
};

/*!
 * Swap function
 * \param[in] x object to be swapped
 */
void RBFBox::swap(RBFBox & x) noexcept
{
    std::swap(m_origin, x.m_origin);
    std::swap(m_span  , x.m_span);
    std::swap(m_suppR, x.m_suppR);
    BaseManipulation::swap(x);
}

/*! It builds the input/output ports of the object
 */
void
RBFBox::buildPorts(){

    bool built = true;

    built = (built && createPortIn<MimmoSharedPointer<MimmoObject>, RBFBox>(this, &mimmo::RBFBox::setGeometry, M_GEOM, true));
    built = (built && createPortIn<double, RBFBox>(this, &mimmo::RBFBox::setSupportRadius, M_VALUED));

    built = (built && createPortOut<darray3E, RBFBox>(this, &mimmo::RBFBox::getOrigin, M_POINT));
    built = (built && createPortOut<darray3E, RBFBox>(this, &mimmo::RBFBox::getSpan, M_SPAN));

    m_arePortsBuilt = built;
};

/*!Clean all stuffs in your class */
void
RBFBox::clearRBFBox(){
    clear(); //base manipulation stuff clear
    m_origin.fill(0.0);
    m_span.fill(0.0);
    m_suppR = 0.0;

};

/*!
 * Return the origin of the RBFBox.
 * \return origin of the box
 */
darray3E
RBFBox::getOrigin(){
    return(m_origin);
}

/*!
 * Return the span of the RBFBox.
 * \return span of the box
 */
darray3E
RBFBox::getSpan(){
    return(m_span);
}
/*!
 * Return the min and max points of the RBFBox.
 * \param[out] bMin min AABB point
 * \param[out] bMax max AABB point
 */
void
RBFBox::getAABB(darray3E & bMin,darray3E & bMax ){
    bMin = m_origin - 0.5*m_span;
    bMax = m_origin + 0.5*m_span;
}


/*!Set a Point Cloud MimmoObject as reference target rbfs.
 * \param[in] cloud set of rbf nodes.
 */
void RBFBox::setGeometry(MimmoSharedPointer<MimmoObject> cloud){
    if(cloud == nullptr)    return;
    if(cloud->getType() != 3)   return;
    BaseManipulation::setGeometry(cloud);
    return;
};

/*! Set the value of the support radius R of RBF kernel functions.
 * \param[in] suppR_ new value of support radius.
 */
void
RBFBox::setSupportRadius(double suppR_){
    m_suppR = std::fmax(0.0,suppR_);
}


/*! Plot the AABB as a structured grid to *vtu file.
 * \param[in] directory output directory
 * \param[in] filename  output filename w/out tag
 * \param[in] counter   integer identifier of the file
 * \param[in] binary    boolean flag for 0-"ascii" or 1-"appended" writing
 */
void
RBFBox::plot(std::string directory, std::string filename,int counter, bool binary){

#if MIMMO_ENABLE_MPI
    if(m_rank == 0){
#endif

    dvecarr3E activeP(8, darray3E({{0.0,0.0,0.0}}));

    getAABB(activeP[0], activeP[6]);

    activeP[1] = activeP[0]; activeP[1][0] += m_span[0];
    activeP[3] = activeP[0]; activeP[3][1] += m_span[1];
    activeP[2] = activeP[6]; activeP[2][2] += -1.0*m_span[2];

    activeP[7] = activeP[6]; activeP[7][0] += -1.0*m_span[0];
    activeP[5] = activeP[6]; activeP[5][1] += -1.0*m_span[1];
    activeP[4] = activeP[0]; activeP[4][2] += m_span[2];

    ivector2D activeConn(1);
    for(int i=0; i<8; ++i)    activeConn[0].push_back(i);

    bitpit::VTKFormat codex = bitpit::VTKFormat::ASCII;
    if(binary){codex=bitpit::VTKFormat::APPENDED;}
    bitpit::VTKElementType elDM = bitpit::VTKElementType::HEXAHEDRON;
    bitpit::VTKUnstructuredGrid vtk(directory, filename, elDM);
    vtk.setGeomData( bitpit::VTKUnstructuredField::POINTS, activeP) ;
    vtk.setGeomData( bitpit::VTKUnstructuredField::CONNECTIVITY, activeConn) ;
    vtk.setDimensions(1, 8);
    vtk.setCodex(codex);
    if(counter>=0){vtk.setCounter(counter);}

    vtk.write();
#if MIMMO_ENABLE_MPI
    }
    MPI_Barrier(m_communicator);
#endif
};


/*!Execute your object.
 * It calculates the RBFBox of the input set of RBFs (+1% of support radius).
 *
 */
void
RBFBox::execute(){

    MimmoSharedPointer<MimmoObject> pc = getGeometry();
    if (pc == nullptr)    {
        (*m_log)<<"Error in : "<<m_name<<" not valid point cloud linked."<<std::endl;
        throw std::runtime_error(m_name+" not valid point cloud linked.");
    }

    darray3E pMin, pMax;
    pMin.fill(std::numeric_limits<double>::max());
    pMax.fill(-1.0*std::numeric_limits<double>::max());

    int count;
    for(const bitpit::Vertex & vert : pc->getVertices()){
        count = 0;
        for(double val : vert.getCoords()){
            pMin[count] = std::min(pMin[count], (val - m_suppR*1.01));
            pMax[count] = std::max(pMax[count], (val + m_suppR*1.01));
            ++count;
        }
    }

#if MIMMO_ENABLE_MPI
    //reduce bbox data all over the ranks
    MPI_Allreduce(MPI_IN_PLACE, pMin.data(), 3, MPI_DOUBLE, MPI_MIN, m_communicator);
    MPI_Allreduce(MPI_IN_PLACE, pMax.data(), 3, MPI_DOUBLE, MPI_MAX, m_communicator);
#endif

    m_span = pMax - pMin;
    m_origin = pMin + m_span * 0.5;

};

/*!
 * Plot Optional results of the class, that is the oriented bounding box as *.vtu mesh
 */
void
RBFBox::plotOptionalResults(){
    plot(m_outputPlot, m_name, getId(), true );
}

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
RBFBox::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    BaseManipulation::absorbSectionXML(slotXML, name);

    if(slotXML.hasOption("SupportRadius")){
        std::string input = slotXML.get("SupportRadius");
        input = bitpit::utils::string::trim(input);
        double value = 0.0;
        if(!input.empty()){
            std::stringstream ss(input);
            ss >> value;
        }
        setSupportRadius(value);
    }
}

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
RBFBox::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);
    BaseManipulation::flushSectionXML(slotXML, name);
    slotXML.set("SupportRadius", std::to_string(m_suppR));
};



}
