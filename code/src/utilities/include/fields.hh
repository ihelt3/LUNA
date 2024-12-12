/*------------------------------------------------------------------------*\
**  
**  @file:      fields.hh
**
**  @author:    Isaiah Helt (ihelt@gatech.edu)
**
**  @brief:     Header file of different fields classes
**
\*------------------------------------------------------------------------*/

#ifndef _FIELDS_HH_
#define _FIELDS_HH_

#include <memory>
#include <vector>
#include <string>
#include <MeshEntities.hh>
#include <variant>

#include "mesh.hh"

namespace UTILITIES {

// Field Type enum
enum fieldTypeEnum {
    INVALID,
    PRESSURE,
    VELOCITY,
    MASSFLUX,
    TEMPERATURE
};

/*------------------------------------------------------------------------*\
**  Class field declaration
\*------------------------------------------------------------------------*/

template<typename fieldType>
class field 
{

public:
    // Constructor
        // Standard constructor: initializes from mesh and initial value
        field(std::shared_ptr<MESH::mesh> baseMesh, fieldType initialValue, std::string meshLocation="cell", fieldTypeEnum type=fieldTypeEnum::INVALID)
        :
            _mesh(baseMesh),
            _type(type)
        {
            // Initialize internal field
            if (meshLocation == "cell") {
                _internal = std::vector<fieldType>(baseMesh->get_elements().size(), initialValue);
            }
            else if (meshLocation == "node") {
                _internal = std::vector<fieldType>(baseMesh->get_nodes().size(), initialValue);
            }
            else if (meshLocation == "face") {
                _internal = std::vector<fieldType>(baseMesh->get_faces().size(), initialValue);
            }
            
        }

        field(std::shared_ptr<MESH::mesh> baseMesh, std::vector<fieldType> internalValue, fieldTypeEnum type=fieldTypeEnum::INVALID)
        :
            _mesh(baseMesh),
            _internal(internalValue),
            _type(type)
        {}

    // Member functions
        // Get field residual
        fieldType get_residual();

    // Get methods  
        // get pointer to mesh
        const MESH::mesh& get_mesh() const { return *_mesh; }
        // get internal field
        const std::vector<fieldType>& get_internal() const { return _internal; }
        // get field type
        const fieldTypeEnum get_type() const { return _type; }
        // get the old field
        const std::vector<fieldType>& get_old() const { return _old; }

    // Set Methods
        // set internal field
        void set_internal(const std::vector<fieldType>& internal);
    

private:
    // Member Data
        // Pointer to mesh
        std::shared_ptr<MESH::mesh> _mesh;
        // Iteration index
        int _index;
        // interal field values
        std::vector<fieldType> _internal;
        // OLD field value
        std::vector<fieldType> _old;
        // Field Type
        fieldTypeEnum _type;
        // flag for internal field initialized
        bool _initialized = false;
};



};


#endif // _FIELDS_HH_