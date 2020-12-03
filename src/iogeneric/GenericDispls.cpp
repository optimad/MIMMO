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
#include "GenericDispls.hpp"
#include <bitpit_operators.hpp>
#include <fstream>

namespace mimmo {

/*!
 * Default constructor of GenericDispls.
 * \param[in] readMode True if the object is in read mode, false if in Write mode.
 */
GenericDispls::GenericDispls(bool readMode){
    m_name      = "mimmo.GenericDispls";
    m_read      = readMode;
    m_nDispl    = 0;
    m_template  = false;
    m_dir       = ".";
    m_filename  = m_name+"_source.dat";
};

/*!
 * Custom constructor reading xml data
 * \param[in] rootXML reference to your xml tree section
 */
GenericDispls::GenericDispls(const bitpit::Config::Section & rootXML){

    m_name      = "mimmo.GenericDispls";
    m_nDispl    = 0;
    m_template  = false;
    m_dir       = ".";
    m_filename  = m_name+"_source.dat";
    m_read      = true;

    std::string fallback_name = "ClassNONE";
    std::string input = rootXML.get("ClassName", fallback_name);
    input = bitpit::utils::string::trim(input);

    std::string fallback_name2 = "1";
    std::string input2 = rootXML.get("IOmode", fallback_name2);
    input2 = bitpit::utils::string::trim(input2);

    m_read = bool(std::atoi(input2.c_str()));

    if(input == "mimmo.GenericDispls"){
        absorbSectionXML(rootXML);
    }else{
        warningXML(m_log, m_name);
    };
}

/*!
 * Destructor
 */
GenericDispls::~GenericDispls(){};

/*!
 * Copy constructor of GenericDispls.
 */
GenericDispls::GenericDispls(const GenericDispls & other):BaseManipulation(other){
    m_read      = other.m_read;
    m_dir       = other.m_dir;
    m_filename  = other.m_filename;
    m_nDispl = other.m_nDispl;
    m_displ = other.m_displ;
    m_labels = other.m_labels;
    m_template = other.m_template;
};

/*!
 * Swap method.
 * \param[in] x object to be swapped
 */
void GenericDispls::swap(GenericDispls & x )noexcept
{
    std::swap(m_read, x.m_read);
    std::swap(m_dir, x.m_dir);
    std::swap(m_filename, x.m_filename);
    std::swap(m_nDispl, x.m_nDispl);
    std::swap(m_displ, x.m_displ);
    std::swap(m_labels, x.m_labels);
    std::swap(m_template, x.m_template);
    BaseManipulation::swap(x);
}
/*!
 * It builds the input/output ports of the object
 */
void
GenericDispls::buildPorts(){

    bool built = true;

    built = (built && createPortIn<dvecarr3E, GenericDispls>(this, &mimmo::GenericDispls::setDispl, M_DISPLS, !m_read));
    built = (built && createPortIn<livector1D, GenericDispls>(this, &mimmo::GenericDispls::setLabels, M_VECTORLI));
    built = (built && createPortIn<int, GenericDispls>(this, &mimmo::GenericDispls::setNDispl, M_VALUEI));

    built = (built && createPortOut<dvecarr3E, GenericDispls>(this, &mimmo::GenericDispls::getDispl, M_DISPLS));
    built = (built && createPortOut<livector1D, GenericDispls>(this, &mimmo::GenericDispls::getLabels, M_VECTORLI));
    built = (built && createPortOut<int, GenericDispls>(this, &mimmo::GenericDispls::getNDispl, M_VALUEI));

    m_arePortsBuilt = built;
}

/*!
 * Return the number of displacements actually stored into the class
 * \return number of displacements
 */
int
GenericDispls::getNDispl(){
    return m_nDispl;
};

/*!
 * Return the list of displacements actually stored into the class
 * \return list of displacements
 */
dvecarr3E
GenericDispls::getDispl(){
    return m_displ;
};

/*!
 * Return the labels attached to displacements actually stored into the class.
 * Labels are always checked and automatically fit to displacements during the class execution
 * \return list of labels
 */
livector1D
GenericDispls::getLabels(){
    return m_labels;
};

/*!
 * Return if template option is active. This method is only meant in class working in Write mode
 * \return template flag
 */
bool
GenericDispls::isTemplate(){
    return m_template;
};


/*!
 * It sets the name of the input directory. Active only in read mode.
 * \param[in] dir directory path
 */
void
GenericDispls::setReadDir(std::string dir){
    if(!m_read)    return;
    m_dir = dir;
};

/*!
 * It sets the name of the input file. Active only in read mode.
 * \param[in] filename filename with tag extension included.
 */
void
GenericDispls::setReadFilename(std::string filename){
    if(!m_read)    return;
    m_filename = filename;
};

/*!
 * It sets the name of the output directory. Active only in write mode.
 * \param[in] dir directory path
 */
void
GenericDispls::setWriteDir(std::string dir){
    if(m_read)    return;
    m_dir = dir;
};

/*!
 * It sets the name of the output file. Active only in write mode.
 * \param[in] filename filename with tag extension included.
 */
void
GenericDispls::setWriteFilename(std::string filename){
    if(m_read)    return;
    m_filename = filename;
};

/*!
 * It sets the number of displacements desired. This method does nothing
 * if displacements or labels are available already into the class.
 * The method is not active in Read mode.
 * \param[in] nD number of displacements
 */
void
GenericDispls::setNDispl(int nD){
    if(m_read) return;
    m_nDispl = nD;
};

/*!
 * It sets the labels attached to each displacements.
 * The method is not active in Read mode.
 * \param[in] labels list of label ids
 */
void
GenericDispls::setLabels(livector1D labels){
    if(m_read) return;
    m_labels.clear();
    m_labels = labels;
    if(m_displ.empty())    setNDispl(int(labels.size()));
};

/*!
 * It sets the displacements.
 * The method is not active in Read mode.
 * \param[in] displs list of displacements
 */
void
GenericDispls::setDispl(dvecarr3E displs){
    if(m_read) return;
    m_displ.clear();
    m_displ = displs;
    m_nDispl = m_displ.size();
};

/*!
 * Enables the template writing mode. The method is not active in Read mode.
 * \param[in] flag true to enable the template writing
 */
void
GenericDispls::setTemplate(bool flag){
    if(m_read) return;
    m_template = flag;
};

/*!
 * Clear all data stored in the class
 */
void
GenericDispls::clear(){
    m_nDispl      = 0;
    m_displ.clear();
    m_labels.clear();
    m_template     = false;
    m_filename     = m_name+"_source.dat";
}

/*!
 * Execution command.
 * Read data from or Write data on linked filename
 */
void
GenericDispls::execute(){

    if(m_read){
        read();
    }else{
        write();
    }
};

/*!
 * It sets infos reading from a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void GenericDispls::absorbSectionXML(const bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    std::string input;

    //checking IOmode

    if(slotXML.hasOption("IOmode")){
        input = slotXML.get("IOmode");
        bool value = true;
        if(!input.empty()){
            std::stringstream ss(bitpit::utils::string::trim(input));
            ss>>value;
        }
        if (m_read != value){
            (*m_log)<< "warning in class "<<m_name<<": cannot absorb xml parameters for class IOmode mismatching"<<std::endl;
            throw std::runtime_error (m_name + " : xml absorbing failed");
        }
    };

    BaseManipulation::absorbSectionXML(slotXML, name);

    if(m_read){
        if(slotXML.hasOption("ReadDir")){
            std::string input = slotXML.get("ReadDir");
            input = bitpit::utils::string::trim(input);
            setReadDir(input);
        };

        if(slotXML.hasOption("ReadFilename")){
            std::string input = slotXML.get("ReadFilename");
            input = bitpit::utils::string::trim(input);
            setReadFilename(input);
        };
    }else{
        if(slotXML.hasOption("WriteDir")){
            std::string input = slotXML.get("WriteDir");
            input = bitpit::utils::string::trim(input);
            setWriteDir(input);
        };

        if(slotXML.hasOption("WriteFilename")){
            std::string input = slotXML.get("WriteFilename");
            input = bitpit::utils::string::trim(input);
            setWriteFilename(input);
        };
    }

    if(slotXML.hasOption("NDispl")){
        std::string input = slotXML.get("NDispl");
        input = bitpit::utils::string::trim(input);
        int temp = 0;
        if(!input.empty()){
            std::stringstream ss(input);
            ss>>temp;
        }
        setNDispl(temp);
    };

    if(slotXML.hasOption("Template")){
        std::string input = slotXML.get("Template");
        input = bitpit::utils::string::trim(input);
        bool temp = false;
        if(!input.empty()){
            std::stringstream ss(input);
            ss>>temp;
        }
        setTemplate(temp);
    };

}

/*!
 * It sets infos from class members in a XML bitpit::Config::section.
 * \param[in] slotXML bitpit::Config::Section of XML file
 * \param[in] name   name associated to the slot
 */
void GenericDispls::flushSectionXML(bitpit::Config::Section & slotXML, std::string name){

    BITPIT_UNUSED(name);

    BaseManipulation::flushSectionXML(slotXML, name);

    slotXML.set("IOmode", std::to_string(int(m_read)));
    if(m_read){
        slotXML.set("ReadDir", m_dir);
        slotXML.set("ReadFilename", m_filename);
    }else{
        slotXML.set("WriteDir", m_dir);
        slotXML.set("WriteFilename", m_filename);
    }
    slotXML.set("NDispl", std::to_string(m_nDispl));
    slotXML.set("Template", std::to_string(int(m_template)));

};


/*!
 * Read displacement data from file
 */
void GenericDispls::read(){

    std::ifstream reading;
    std::string source = m_dir+"/"+ m_filename;
    reading.open(source.c_str());

#if MIMMO_ENABLE_MPI
    // Leave the reading only to the master rank
    if(getRank() == 0)
#endif
    {

        if(reading.is_open()){

            m_displ.clear();
            m_labels.clear();

            std::string keyword, line;
            long label;
            darray3E dtrial;

            do{
                line.clear();
                keyword.clear();
                std::getline(reading,line);
                line = bitpit::utils::string::trim(line);
                std::stringstream ss(line);
                ss>>keyword;
                keyword = bitpit::utils::string::trim(keyword);
                if(keyword == "$DISPL")    {

                    ss>>label;
                    m_labels.push_back(label);

                    dtrial.fill(0.0);
                    ss>>dtrial[0]>>dtrial[1]>>dtrial[2];
                    m_displ.push_back(dtrial);
                }

            }while(!reading.eof());

            m_nDispl = m_displ.size();

        }else{
            (*m_log)<<"error of "<<m_name<<" : cannot open "<<m_filename<< " requested. Exiting... "<<std::endl;
            throw std::runtime_error (m_name + " : cannot open " + m_filename + " requested");
        }

        reading.close();

    }

#if MIMMO_ENABLE_MPI
    // Communicate the data read by master rank to other ranks
    MPI_Bcast(&m_nDispl, 1, MPI_INT, 0, m_communicator);

    m_displ.resize(m_nDispl);
    MPI_Bcast(m_displ.data(), 3*m_nDispl, MPI_DOUBLE, 0, m_communicator);

    m_labels.resize(m_nDispl);
    MPI_Bcast(m_labels.data(), m_nDispl, MPI_LONG, 0, m_communicator);
#endif

};

/*!
 * Write displacement data to file
 */
void GenericDispls::write(){

#if MIMMO_ENABLE_MPI
    // Leave the writing only to the master rank
    if(getRank() == 0)
#endif
    {

        //assessing data;
        int sizelabels = m_labels.size();
        long maxlabel = 0;
        for(const auto & val: m_labels){
            maxlabel = std::max(maxlabel,val);
        }

        int sizedispl = m_displ.size();

        m_labels.resize(m_displ.size(), -1);
        if(sizelabels < sizedispl){
            for(int i = sizelabels; i<sizedispl; ++i){
                m_labels[i] = maxlabel+1;
                ++maxlabel;
            }
        }

        std::string filename;
        if(m_template){
            filename = "TEMPLATE_"+m_filename;
        }else{
            filename = m_filename;
        }

        std::ofstream writing;
        std::string source = m_dir+"/"+ m_filename;
        writing.open(source.c_str());
        std::string keyT1 = "{", keyT2 = "}";
        if(writing.is_open()){

            int counter = 0;
            for(const auto & dd : m_displ){
                if(m_template){
                    std::string str1 = keyT1+"x"+std::to_string(m_labels[counter])+keyT2;
                    std::string str2 = keyT1+"y"+std::to_string(m_labels[counter])+keyT2;
                    std::string str3 = keyT1+"z"+std::to_string(m_labels[counter])+keyT2;

                    writing<<"$DISPL"<<'\t'<<m_labels[counter]<<'\t'<<str1<<'\t'<<str2<<'\t'<<str3<<std::endl;
                }else{
                    writing<<"$DISPL"<<'\t'<<m_labels[counter]<<'\t'<<dd[0]<<'\t'<<dd[1]<<'\t'<<dd[2]<<std::endl;
                }
                ++counter;
            }

        }else{
            (*m_log)<<"error of "<<m_name<<" : cannot open "<<m_filename<< " requested. Exiting... "<<std::endl;
            throw std::runtime_error (m_name + " : cannot open " + m_filename + " requested");
        }

        writing.close();

    }
};


}
