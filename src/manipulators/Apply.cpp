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
#include "Apply.hpp"


namespace mimmo{

/*!
 * Default constructor of Apply
 */
Apply::Apply():BaseManipulation(){
	m_name = "mimmo.Apply";
	m_input.clear();
	m_scalarinput.clear();
	m_factor = 1.;
};

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
Apply::Apply(const bitpit::Config::Section & rootXML){

	m_name = "mimmo.Apply";
	m_factor = 1.;

	std::string fallback_name = "ClassNONE";
	std::string input = rootXML.get("ClassName", fallback_name);
	input = bitpit::utils::string::trim(input);
	if(input == "mimmo.Apply"){
		absorbSectionXML(rootXML);
	}else{
		warningXML(m_log, m_name);
	};
}

/*!Default destructor of Apply
 */
Apply::~Apply(){};

/*!Copy constructor of Apply.
 */
Apply::Apply(const Apply & other):BaseManipulation(other){
	m_input = other.m_input;
	m_scalarinput = other.m_scalarinput;
	m_factor = other.m_factor;
};

/*!
 * Swap function
 * \param[in] x object to be swapped
 */
void Apply::swap(Apply & x) noexcept
{
	//std::swap(m_input, x.m_input);
	m_input.swap(x.m_input);
	m_scalarinput.swap(x.m_scalarinput);
	std::swap(m_factor, x.m_factor);
	BaseManipulation::swap(x);
}

/*! It builds the input/output ports of the object
 */
void
Apply::buildPorts(){
	bool built = true;
	built = (built && createPortIn<dmpvecarr3E*, Apply>(this, &Apply::setInput, M_GDISPLS, true, 1));
	built = (built && createPortIn<dmpvector1D*, Apply>(this, &Apply::setScalarInput, M_SCALARFIELD, true, 1));
	built = (built && createPortIn<MimmoObject*, Apply>(this, &BaseManipulation::setGeometry, M_GEOM, true));
	built = (built && createPortOut<MimmoObject*, Apply>(this, &BaseManipulation::getGeometry, M_GEOM));
	built = (built && createPortOut<dmpvecarr3E*, Apply>(this, &Apply::getOutput, M_GDISPLS));
	m_arePortsBuilt = built;
};


/*!It sets the displacements input.
 * \param[in] input Input displacements of the geometry vertices.
 */
void
Apply::setInput(dmpvecarr3E *input){
    if(!input)  return;
	m_input = *input;
};

/*!It sets the displacements given as scalar input. It makes sense only for surface gemetries.
 * The displacements will be directed as the surface normals.
 * \param[in] input Input displacements of the geometry vertices given as module of normal directed vectors.
 */
void
Apply::setScalarInput(dmpvector1D *input){
    if(!input) return;
	m_scalarinput = *input;
};

/*!It sets the displacements scalar factor.
 * The displacements will be scaled by using this factor.
 * \param[in] alpha Scale factor of displacements field.
 */
void
Apply::setScaling(double alpha){
	m_factor = alpha;
};

/*!It get the displacements output.
 * \return Output displacements of the geometry vertices.
 */
dmpvecarr3E*
Apply::getOutput(){
    return	&m_output;
};
/*!Execution command.
 * It applies the deformation stored in the input of base class (casting the input
 * for apply object to dvecarr3E) to the linked geometry.
 * After exec() the original geometry will be permanently modified.
 */
void
Apply::execute(){
    if(!getGeometry()){
        (*m_log)<<m_name + " : NULL pointer to linked geometry found"<<std::endl;
        return;
    }

    if(getGeometry()->isEmpty()){
#if MIMMO_ENABLE_MPI
        (*m_log)<<"WARNING "<<m_name <<"on rank "<<m_rank<<" : empty linked geometry found"<<std::endl;
#else
        (*m_log)<<"WARNING "<<m_name <<" : empty linked geometry found"<<std::endl;
#endif
    }

	checkInput();

	m_output = m_input;

	darray3E vertexcoords;
	long int ID;
	for (const auto & vertex : m_geometry->getVertices()){
		vertexcoords = vertex.getCoords();
		ID = vertex.getId();
		std::array<double,3> val = m_factor*m_input[ID];
		m_output[ID] = val;
		vertexcoords += val;
		getGeometry()->modifyVertex(vertexcoords, ID);
	}

#if MIMMO_ENABLE_MPI
	getGeometry()->updatePointGhostExchangeInfo();
#endif

};


/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
Apply::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

	BITPIT_UNUSED(name);
	BaseManipulation::absorbSectionXML(slotXML, name);

    if(slotXML.hasOption("Scaling")){
        std::string input = slotXML.get("Scaling");
        input = bitpit::utils::string::trim(input);
        double val = 1.;
        if(!input.empty()){
            std::stringstream ss(input);
            ss>>val;
        }
        setScaling(val);
    }

};

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void
Apply::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

	BITPIT_UNUSED(name);
	BaseManipulation::flushSectionXML(slotXML, name);

    slotXML.set("Scaling", std::to_string(m_factor));

};

/*!
 * Check if the input is related to the target geometry.
 * If not create a zero input field.
 */
void
Apply::checkInput(){

	bool check = false;
	if (m_input.size()){
		check = m_input.getDataLocation() == mimmo::MPVLocation::POINT;
		check = check && m_input.getGeometry() == m_geometry;
		check = check && m_input.completeMissingData({{0.0,0.0,0.0}});
	}
	if (m_scalarinput.size()){
		check = m_scalarinput.getDataLocation() == mimmo::MPVLocation::POINT;
		check = check && m_geometry->getType() == 1;
		check = check && m_scalarinput.getGeometry() == m_geometry;
		check = check && m_scalarinput.completeMissingData(0.0);
		if (check){
			bitpit::SurfaceKernel* skernel = static_cast<bitpit::SurfaceKernel*>(m_geometry->getPatch());
			bitpit::PiercedVector<darray3E> vNormals;
			bitpit::ConstProxyVector<long> verts;
			std::size_t size;
			long idN;
			for(const auto & cell: m_geometry->getCells()){
				verts = cell.getVertexIds();
				size = verts.size();
				for(std::size_t i=0; i<size; ++i){
					idN = verts[i];
					if(!vNormals.exists(idN)){
						vNormals.insert(idN, skernel->evalVertexNormal(cell.getId(), i));
					}
				}
			}
			m_input.clear();
			for(const auto & vertex: m_geometry->getVertices()){
				m_input.setDataLocation(mimmo::MPVLocation::POINT);
				m_input.setGeometry(m_geometry);
				long id = vertex.getId();
				m_input.insert(id, darray3E(m_scalarinput[id]*vNormals[id]));
			}
		}
	}
	if (!check){
		m_log->setPriority(bitpit::log::Verbosity::DEBUG);
		(*m_log)<<"Not valid input found in "<<m_name<<". Proceeding with default zero field"<<std::endl;
		m_log->setPriority(bitpit::log::Verbosity::NORMAL);
		m_input.clear();
		m_input.setGeometry(m_geometry);
		m_input.setDataLocation(mimmo::MPVLocation::POINT);
		m_input.reserve(m_geometry->getNVertices());
		for (const auto & vertex : m_geometry->getVertices()){
			m_input.insert(vertex.getId(), {{0.0,0.0,0.0}});
		}
	}
};

}
