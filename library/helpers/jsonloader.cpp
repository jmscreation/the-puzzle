/*
    Author:     jmscreator
    License:   Always free to use forever (See MIT License)
*/

#include "jsonloader.h"


namespace json {
    /*
        JSON Wrapper Basic Variables
    */

    // public
    bool displayErrors = false; // display error messages
    bool autoInitValues = true; // auto initialize values when saving properties

    // private
    bool _ready = false;
    rapidjson::Document _jsonData;

    /*
        Load External Parameters From JSON Data
    */

    bool loadProperty(const Object& config, const char* name, Object& parameter) {
        if(!_ready){
            if(displayErrors){
                std::cout << "Must parse JSON data before loading property\n";
            }
            return false;
        }

        if(config.IsObject() && config.HasMember(name)){
            if(config[name].IsObject()){
                parameter.SetObject();
                parameter.CopyFrom(config[name], _jsonData.GetAllocator());
                return true;
            } else {
                if(displayErrors) {
                    std::cout << name << " is not a valid object\n";
                }
            }
        }
        return false;
    }

    bool loadPropertyArray(const Object& config, const char* name, Array& parameter) {
        if(!_ready){
            if(displayErrors){
                std::cout << "Must parse JSON data before loading property\n";
            }
            return false;
        }

        if(config.IsObject() && config.HasMember(name)){
            if(config[name].IsArray()){
                parameter.SetArray();
                Object copy;
                copy.CopyFrom(config[name], _jsonData.GetAllocator()); // make copy of array to prevent issues on additional loading

                rapidjson::GenericArray<false, Object> array = copy.GetArray(); // get non-const array of copy
                for(auto it = array.begin(); it != array.end(); ++it){
                    parameter.PushBack(*it, _jsonData.GetAllocator()); // move copied elements
                }

                return true;
            } else {
                if(displayErrors) {
                    std::cout << name << " is not a valid array\n";
                }
            }
        }
        return false;
    }

    /*
        Save Internal Parameters Into JSON Data
    */

    bool saveProperty(Object& config, const char* name, const Object& parameter) {
        if(!parameter.IsObject()){
            if(displayErrors){
                std::cout << "Internal JSON parameter must be an object\n";
            }
            return false;
        }

        if(!config.IsObject() && autoInitValues){
            config.SetObject();
        }

        if(config.IsObject()){
            if(!config.HasMember(name)){
                config.AddMember(rapidjson::Value().SetString(rapidjson::StringRef(name)), (rapidjson::Value().SetObject()).Move(), _jsonData.GetAllocator());
            }

            if(config.HasMember(name)){
                if(!config[name].IsObject()) config[name].SetObject();
                config[name].CopyFrom(parameter, _jsonData.GetAllocator());
                return true;
            }

        }

        if(displayErrors){
            std::cout << "Internal JSON object must be an object or failed to add member\n";
        }
        return false;
    }

    bool saveProperty(Object& config, const char* name, const std::string& parameter) {
        if(!config.IsObject() && autoInitValues){
            config.SetObject();
        }

        if(config.IsObject()){
            if(!config.HasMember(name)){
                config.AddMember((rapidjson::Value().SetString(rapidjson::StringRef(name))).Move(),
                            (rapidjson::Value().SetNull()).Move(), _jsonData.GetAllocator());
            }

            if(config.HasMember(name)){
                config[name].SetString(parameter.data(), parameter.size(), _jsonData.GetAllocator());
                
                return true;
            }
        }

        if(displayErrors) {
            std::cout << "Failed to store member in JSON\n";
        }
        return false;
    }

    bool savePropertyArray(Object& config, const char* name, const Array& parameter) {
        if(!parameter.IsArray()){
            if(displayErrors){
                std::cout << "Internal JSON parameter must be an array\n";
            }
            return false;
        }

        if(!config.IsObject() && autoInitValues){
            config.SetObject();
        }

        if(config.IsObject()){
            if(!config.HasMember(name)){
                config.AddMember(rapidjson::Value().SetString(rapidjson::StringRef(name)).Move(),
                            (rapidjson::Value().SetArray()).Move(), _jsonData.GetAllocator());
            }

            if(config.HasMember(name)){
                if(!config[name].IsArray()) config[name].SetArray();
                config[name].CopyFrom(parameter, _jsonData.GetAllocator());
                return true;
            }

        }

        if(displayErrors){
            std::cout << "Internal JSON object must be an object or failed to add member\n";
        }
        return false;
    }

    bool saveValue(Object& value, const Object& parameter) {

        value.CopyFrom(parameter, _jsonData.GetAllocator());

        return true;
    }

    bool appendArrayValue(Array& array, const std::string& parameter) {
        if(!array.IsArray() && autoInitValues){
            array.SetArray();
        }
        
        if(!array.IsArray()){
            if(displayErrors){
                std::cout << "JSON array must be an array\n";
            }
            return false;
        }

        array.PushBack( (rapidjson::Value().SetString(parameter.data(), parameter.size())).Move(), _jsonData.GetAllocator() );
        return true;
    }

    bool appendArrayValue(Array& array, const Object& parameter) {
        if(!array.IsArray() && autoInitValues){
            array.SetArray();
        }
        
        if(!array.IsArray()){
            if(displayErrors){
                std::cout << "JSON array must be an array\n";
            }
            return false;
        }

        array.PushBack( (rapidjson::Value().CopyFrom(parameter, _jsonData.GetAllocator())).Move(), _jsonData.GetAllocator() );
        return true;
    }

    /*
        Control Functions For Parsing / Exporting / Clearing JSON Database
    */

    bool parseData(const char* data, size_t length) {
        rapidjson::Document& config = _jsonData;
        clearData();
        config.Parse(data, length);
        if(config.GetParseError() != rapidjson::ParseErrorCode::kParseErrorNone){
            if(displayErrors){
                std::cout << "JSON Failed to parse - Error @" << config.GetErrorOffset()
                          << " -> " << rapidjson::GetParseError_En(config.GetParseError()) << "\n";
            }
            return false;
        }
        if(!config.IsObject()){
            if(displayErrors){
                std::cout << "Initial JSON must be an object\n";
            }
            return false;
        }
        _ready = true;
        return true;
    }

    bool exportData(const rapidjson::Value& config, std::string& output) {
        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
        if(!config.Accept(writer)){
            if(displayErrors){
                std::cout << "Failed to export JSON structure into string buffer\n";
            }
            return false;
        }

        output.assign(buf.GetString(), buf.GetSize());

        return true;
    }

    bool exportData(std::string& output) {
        return exportData(_jsonData, output);
    }

    void clearData() {
        _jsonData.SetNull();
        _ready = false;
    }
}