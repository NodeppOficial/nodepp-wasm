/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace { 
    
    int SET( const string_t& name, const string_t& value ){ 
        return EM_ASM_INT({
            var value   = UTF8ToString($1);
            var key     = UTF8ToString($0);
            window[key] = value; return 1;
        }, name.get(), value.get() );
    }

    string_t GET( const string_t& name ){ 
        char res [UNBFF_SIZE]; auto size = EM_ASM_INT({
            stringToUTF8( window[key], $0, $1 );
            return str.length; 
        }, res, UNBFF_SIZE ); return { res, size };
    } 

    int DEL( const string_t& name ){ 
        return EM_ASM_INT({
            var key = UTF8ToString($0);
            delete window[key]; return 1;
        }, name.get() );
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    namespace env {

        int set( const string_t& name, const string_t& value ){ return nodepp::SET( name, value ); }

        string_t get( const string_t& name ){ return nodepp::GET( name ); }

        int remove( const string_t& name ){ return nodepp::DEL( name );  } 

    }
    
    /*─······································································─*/

    bool  is_child(){ return !env::get("CHILD").empty(); }

    bool is_parent(){ return  env::get("CHILD").empty(); }
    
    string_t shell(){ return env::get("SHELL"); }

    string_t  home(){ return env::get("HOME"); }
    
}}

/*────────────────────────────────────────────────────────────────────────────*/