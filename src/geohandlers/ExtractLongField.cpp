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

#include "ExtractFields.hpp"
#include "SkdTreeUtils.hpp"

namespace mimmo{

/*!
 * Default constructor.
 */
ExtractLongField::ExtractLongField():ExtractField(){
    m_name = "mimmo.ExtractLongField";
}

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
ExtractLongField::ExtractLongField(const bitpit::Config::Section & rootXML){

    std::string fallback_name = "ClassNONE";
    std::string input_name = rootXML.get("ClassName", fallback_name);
    input_name = bitpit::utils::string::trim(input_name);

    m_name = "mimmo.ExtractLongField";

    if(input_name == "mimmo.ExtractLongField"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!
 * Default destructor
 */
ExtractLongField::~ExtractLongField(){}


/*!
 * Copy Constructor.
 * \param[in] other class of type ExtractField
 */
ExtractLongField::ExtractLongField(const ExtractLongField & other):ExtractField(other){
	m_field = other.m_field;
	m_result = other.m_result;
}

/*!
 * Assignement operator of ExtractField.
 * \param[in] other class of type ExtractField
 */
ExtractLongField & ExtractLongField::operator=(const ExtractLongField & other){
	ExtractField::operator=(other);
	m_field = other.m_field;
	m_result = other.m_result;
	return *this;
};

/*!
 * Swap function of ExtractLongField
 * \param[in] x object to be swapped.
 */
void ExtractLongField::swap(ExtractLongField & x ) noexcept
{
    m_field.swap(x.m_field);
    m_result.swap(x.m_result);
    ExtractField::swap(x);
};


/*!
 * Build the ports of the class;
 */
void
ExtractLongField::buildPorts(){

    ExtractField::buildPorts();

    bool built = m_arePortsBuilt;
    built = (built && createPortIn<MimmoPiercedVector<long>*, ExtractLongField>(this, &mimmo::ExtractLongField::setField, M_LONGFIELD, true));
    built = (built && createPortOut<MimmoPiercedVector<long>*, ExtractLongField>(this, &mimmo::ExtractLongField::getExtractedField, M_LONGFIELD));

    m_arePortsBuilt = built;
}

/*!
 * Get extracted field.
 * \return extracted field
 */
MimmoPiercedVector<long> *
ExtractLongField::getExtractedField(){
    return &m_result;
}

/*!
 * Get linked field where result need to be extracted from.
 * \return original field
 */
MimmoPiercedVector<long>
ExtractLongField::getOriginalField(){
    return m_field;
}

/*!
 * Set input field for extraction.
 * \param[in] field input field related to whole geometry
 */
void
ExtractLongField::setField(MimmoPiercedVector<long> *field){
    if(!field) return;
    m_field = *field;
}

/*!
 * Clear content of the class
 */
void
ExtractLongField::clear(){
    m_field.clear();
    m_result.clear();
    ExtractField::clear();
}

/*!
 * Plot extracted field over the target geometry
 */
void
ExtractLongField::plotOptionalResults(){

	m_result.setName("field");
	write(m_result.getGeometry(), m_result);

}

/*!
 * Extract field from your original field by using the provided target geometry
 * \return true if extract without errors
 */
bool
ExtractLongField::extract(){

    if (getGeometry() == nullptr || m_field.getGeometry() == nullptr) return false;
    //checking internal ids coherence of the field.
    if(!m_field.checkDataIdsCoherence()) return false;

    mimmo::MPVLocation refLoc = m_field.getDataLocation();
    if(refLoc == mimmo::MPVLocation::UNDEFINED) refLoc = mimmo::MPVLocation::POINT;

    m_result.clear();
    m_result.setDataLocation(refLoc);

    switch(m_mode){
    case ExtractMode::ID :
        extractID(refLoc);
        m_result.setGeometry(getGeometry());
    break;

    case ExtractMode::PID :
        extractPID(refLoc);
        m_result.setGeometry(m_field.getGeometry());
    break;

    case ExtractMode::MAPPING :
        extractMapping(refLoc);
        m_result.setGeometry(getGeometry());
    break;

    default :
        assert(false && "unrecognized field location");
        break;
    }

    return true;

}

/*!
 * Perform extraction by ID mode and refer data to target geometry vertex, cell or interface,
 * according to the location specified.
 * \param[in] loc MPVLocation enum identifying data location POINT, CELL or INTERFACE.
 */
void ExtractLongField::extractID(mimmo::MPVLocation loc){

    switch(loc){
        case mimmo::MPVLocation::POINT:
        for (const auto & ID : getGeometry()->getVertices().getIds()){
            if (m_field.exists(ID)){
                m_result.insert(ID, m_field[ID]);
            }
        }
        break;
        case mimmo::MPVLocation::CELL:
            for (const auto & ID : getGeometry()->getCells().getIds()){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
        break;
        case mimmo::MPVLocation::INTERFACE:
            getGeometry()->updateInterfaces();
            for (const auto & ID : getGeometry()->getInterfaces().getIds()){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
        break;
        default:
            //do nothing
        break;
    }
}

/*!
 * Perform extraction by PID mode and refer data to target geometry vertex, cell or interface,
 * according to the location specified.
 * \param[in] loc MPVLocation enum identifying data location POINT, CELL or INTERFACE.
 */
void ExtractLongField::extractPID(mimmo::MPVLocation loc){
    //Extract by PIDs
    livector1D commonPID;
    {
        std::unordered_set<long> pidTarget = getGeometry()->getPIDTypeList();
        std::unordered_set<long> pidLinked = m_field.getGeometry()->getPIDTypeList();
        std::set<long> unionPID(pidTarget.begin(), pidTarget.end());
        unionPID.insert(pidLinked.begin(), pidLinked.end());
        for(auto val: unionPID){
            if(pidLinked.count(val) && pidTarget.count(val)){
                commonPID.push_back(val);
            }
        }
    }
    livector1D cellExtracted = m_field.getGeometry()->extractPIDCells(commonPID);

    switch(loc){
        case mimmo::MPVLocation::POINT:
        {
            livector1D vertExtracted = m_field.getGeometry()->getVertexFromCellList(cellExtracted);
            for (const auto & ID : vertExtracted){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
        }
        break;
        case mimmo::MPVLocation::CELL:
            for (const auto & ID : cellExtracted){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
            break;
        case mimmo::MPVLocation::INTERFACE:
        {
            livector1D interfExtracted = m_field.getGeometry()->getInterfaceFromCellList(cellExtracted);
            for (const auto & ID : interfExtracted){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
        }
        break;
        default:
            //do nothing
            break;
    }
}

/*!
 * Perform extraction by MAPPING mode and refer data to target geometry vertex, cell or interface,
 * according to the location specified.
 * \param[in] loc MPVLocation enum identifying data location POINT, CELL or INTERFACE.
 */
void ExtractLongField::extractMapping(mimmo::MPVLocation loc){

    m_field.getGeometry()->buildSkdTree();
    getGeometry()->buildSkdTree();
#if MIMMO_ENABLE_MPI
    livector1D cellExtracted = mimmo::skdTreeUtils::selectByGlobalPatch(m_field.getGeometry()->getSkdTree(), getGeometry()->getSkdTree(), m_tol);
#else
    livector1D cellExtracted = mimmo::skdTreeUtils::selectByPatch(m_field.getGeometry()->getSkdTree(), getGeometry()->getSkdTree(), m_tol);
#endif

    switch(loc){
        case mimmo::MPVLocation::POINT:
        {
            livector1D vertExtracted = getGeometry()->getVertexFromCellList(cellExtracted);
            for (const auto & ID : vertExtracted){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
        }
        break;
        case mimmo::MPVLocation::CELL:
            for (const auto & ID : cellExtracted){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
            break;
        case mimmo::MPVLocation::INTERFACE:
        {
            livector1D interfExtracted = getGeometry()->getInterfaceFromCellList(cellExtracted);
            for (const auto & ID : interfExtracted){
                if (m_field.exists(ID)){
                    m_result.insert(ID, m_field[ID]);
                }
            }
        }
        break;
        default:
            //do nothing
            break;
    }
}

}
