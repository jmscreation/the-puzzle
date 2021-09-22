/*
    Author:     jmscreator
    License:   Always free to use forever (See MIT License)
*/

#ifndef __JSON_LOADER__
#define __JSON_LOADER__

//include rapidjson Parse Library
#include <string>
#include <iostream>
#include <type_traits>

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"


namespace json {

    // public
    typedef rapidjson::Value Object;
    typedef rapidjson::Value Array;
    extern bool displayErrors;
    extern bool autoInitValues;

    // private
    extern bool _ready;
    extern rapidjson::Document _jsonData;

    //public
    bool loadProperty(const Object& config, const char* name, Object& parameter);
    bool loadPropertyArray(const Object& config, const char* name, Array& parameter);
    
    bool saveProperty(Object& config, const char* name, const Object& parameter);
    bool saveProperty(Object& config, const char* name, const std::string& parameter);
    bool saveValue(Object& value, const Object& parameter);
    bool savePropertyArray(Object& config, const char* name, const Array& parameter);
    bool appendArrayValue(Array& array, const std::string& parameter);
    bool appendArrayValue(Array& array, const Object& parameter);

    // Load Templates

    template<class T>
    bool loadProperty(const Object& config, const char* name, T& parameter) {
        if(std::is_pointer<T>::value){
            if(displayErrors){
                std::cout << "Pointer cannot be loaded from JSON\n";
            }
            return false;
        }

        if(!_ready){
            if(displayErrors){
                std::cout << "Must parse JSON data before loading property\n";
            }
            return false;
        }

        if(config.IsObject() && config.HasMember(name)){
            if(std::is_arithmetic<T>::value && config[name].IsNumber()) {
                if(config[name].IsUint()) { parameter = config[name].GetUint(); return true; }
                if(config[name].IsInt()) { parameter = config[name].GetInt(); return true; }

                if(config[name].IsInt64()) { parameter = config[name].GetInt64(); return true; }
                if(config[name].IsUint64()) { parameter = config[name].GetUint64(); return true; }

                if(config[name].IsDouble()) { parameter = config[name].GetDouble(); return true; }
                if(config[name].IsFloat()) { parameter = config[name].GetFloat(); return true; }
            }

            if(config[name].Is<T>()){
                parameter = config[name].Get<T>();
                return true;
            } else {
                if(displayErrors) {
                    std::cout << name << " is not a valid type\n";
                }
            }
        }
        return false;
    }

    template<class T>
    bool loadValue(const Object& value, T& parameter) {
        if(std::is_pointer<T>::value){
            if(displayErrors){
                std::cout << "Pointer cannot be loaded from JSON\n";
            }
            return false;
        }
        
        if(std::is_arithmetic<T>::value && value.IsNumber()) {
            if(value.IsUint()) { parameter = value.GetUint(); return true; }
            if(value.IsInt()) { parameter = value.GetInt(); return true; }

            if(value.IsInt64()) { parameter = value.GetInt64(); return true; }
            if(value.IsUint64()) { parameter = value.GetUint64(); return true; }

            if(value.IsDouble()) { parameter = value.GetDouble(); return true; }
            if(value.IsFloat()) { parameter = value.GetFloat(); return true; }
        }

        if(value.Is<T>()){
            parameter = value.Get<T>();
            return true;
        } else {
            if(displayErrors) {
                std::cout << "given value is not a valid type\n";
            }
        }
        return false;
    }

    template<class T>
    bool loadProperty(const char* name, T& parameter) {
        return loadProperty(_jsonData, name, parameter);
    }

    template<class T>
    bool loadPropertyArray(const char* name, T& parameter) {
        return loadPropertyArray(_jsonData, name, parameter);
    }


    // Save Templates

    template<class T>
    bool saveProperty(Object& config, const char* name, const T& parameter) {
        if(std::is_pointer<T>::value){
            if(displayErrors){
                std::cout << "Pointer cannot be stored into JSON\n";
            }
            return false;
        }

        if(!config.IsObject() && autoInitValues){
            config.SetObject();
        }

        if(config.IsObject()){
            if(!config.HasMember(name)){
                config.AddMember((rapidjson::Value().SetString(rapidjson::StringRef(name))).Move(),
                            (rapidjson::Value().SetNull()).Move(), _jsonData.GetAllocator());
            }

            if(config.HasMember(name)){
                if(std::is_arithmetic<T>::value) {
                    config[name].Set<T>(parameter);
                    return true;
                }

                if(displayErrors) {
                    std::cout << "Invalid type to export\n";
                }
                return false;
            }
        }

        if(displayErrors) {
            std::cout << "Failed to store member in JSON\n";
        }
        return false;
    }

    template<class T>
    bool saveValue(Object& value, const T& parameter) {
        if(std::is_pointer<T>::value){
            if(displayErrors){
                std::cout << "Pointer cannot be stored into JSON\n";
            }
            return false;
        }
        
        if(std::is_arithmetic<T>::value) {
            value.Set<T>(parameter);
        }

        if(displayErrors){
            std::cout << "Invalid type to export\n";
        }
        return false;
    }

    template<class T>
    bool saveProperty(const char* name, const T& parameter) {
        if(!_ready){
            _jsonData.SetObject();
            _ready = true;
        }
        return saveProperty(_jsonData, name, parameter);
    }

    template<class T>
    bool savePropertyArray(const char* name, const T& parameter) {
        return savePropertyArray(_jsonData, name, parameter);
    }

    template<class T>
    bool appendArrayValue(Array& array, const T& parameter) {
        if(!array.IsArray() && autoInitValues){
            array.SetArray();
        }

        if(!array.IsArray()){
            if(displayErrors){
                std::cout << "JSON array must be an array\n";
            }
            return false;
        }
        if(std::is_pointer<T>::value){
            if(displayErrors){
                std::cout << "Pointer cannot be stored into JSON\n";
            }
            return false;
        }
        
        if(std::is_arithmetic<T>::value) {
            array.PushBack( (rapidjson::Value().Set<T>(parameter)).Move(), _jsonData.GetAllocator());
        }

        array.PushBack( (rapidjson::Value().CopyFrom(parameter, _jsonData.GetAllocator())).Move(), _jsonData.GetAllocator() );
        return true;
    }

    // Control Functions

    bool parseData(const char* data, size_t length);

    bool exportData(const rapidjson::Value& config, std::string& output);

    bool exportData(std::string& output);

    void clearData();

}

#endif // __JSON_LOADER__