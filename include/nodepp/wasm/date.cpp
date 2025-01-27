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
#include <emscripten.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace date {

    ullong now() {
        char raw  [UNBFF_SIZE]; 
        auto size = EM_ASM_INT({
            let data = Date.now().toString();
            stringToUTF8( data, $0, $1 );
            return data.length; 
        }, raw, UNBFF_SIZE );
        return string::to_ullong( string_t( raw, size ) );
    }
    
    /*─······································································─*/

    uint year( const bool& UTC=false ){
        return EM_ASM_INT({ 
            let date = new Date();
            if( $0 ){ return date.getUTCFullYear(); }
            else    { return date.getFullYear(); } 
        }, UTC );
    }
    
    /*─······································································─*/

    uint month( const bool& UTC=false ){
        return EM_ASM_INT({ 
            let date = new Date();
            if( $0 ){ return date.getUTCMonth(); }
            else    { return date.getMonth(); } 
        }, UTC );
    }
    
    /*─······································································─*/

    uint day( const bool& UTC=false ){
        return EM_ASM_INT({ 
            let date = new Date();
            if( $0 ){ return date.getUTCDay(); }
            else    { return date.getDay(); } 
        }, UTC );
    }
    
    /*─······································································─*/
    
    uint hour( const bool& UTC=false ){
        return EM_ASM_INT({ 
            let date = new Date();
            if( $0 ){ return date.getUTCHours(); }
            else    { return date.getHours(); } 
        }, UTC );
    }
    
    /*─······································································─*/

    uint minute( const bool& UTC=false ){
        return EM_ASM_INT({ 
            let date = new Date();
            if( $0 ){ return date.getUTCMinutes(); }
            else    { return date.getMinutes(); } 
        }, UTC );
    }
    
    /*─······································································─*/

    uint second( const bool& UTC=false ){
        return EM_ASM_INT({ 
            let date = new Date();
            if( $0 ){ return date.getUTCSeconds(); }
            else    { return date.getSeconds(); } 
        }, UTC );
    }

    /*─······································································─*/

    string_t fulltime(){
        char res  [UNBFF_SIZE]; 
        auto size = EM_ASM_INT({
            let data = Date();
            stringToUTF8( data, $0, $1 );
            return data.length; 
        }, res, UNBFF_SIZE ); return { res, size };
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class date_t {
protected:

    struct NODE {
        bool utc;
        uint day;
        uint year;
        uint hour;
        uint month;
        uint minute;
        uint second;
    };  ptr_t<NODE> obj;
    
public:

    template< class... V > 
    date_t( const V&... args ) noexcept : obj( new NODE() ) { set_time( args... ); }
    
    /*─······································································─*/

    void set_time( const bool& UTC=false ) const noexcept {
        obj->utc = UTC;
    }

    void set_time( const uint& year, const bool& UTC=false ) const noexcept {
        obj->year = year; obj->utc = UTC;
    }

    void set_time( const uint& year, const uint& month, const bool& UTC=false ) const noexcept { 
        obj->month = month; obj->year = year; obj->utc = UTC;
    }

    void set_time( const uint& year, const uint& month, const uint& day, const bool& UTC=false ) const noexcept { 
        obj->month = month; obj->year = year; obj->day = day; obj->utc = UTC;
    }

    void set_time( const uint& year, const uint& month, const uint& day, const uint& hour, const bool& UTC=false ) const noexcept { 
        obj->month = month; obj->year = year; obj->day = day; obj->hour = hour; obj->utc = UTC;
    }

    void set_time( const uint& year, const uint& month, const uint& day, const uint& hour, const uint& min, const bool& UTC=false ) const noexcept { 
        obj->month = month; obj->year = year; obj->day = day; obj->hour = hour; obj->minute = min; obj->utc = UTC;
    }

    void set_time( const uint& year, const uint& month, const uint& day, const uint& hour, const uint& min, const uint& second, const bool& UTC=false ) const noexcept { 
        obj->month = month; obj->year = year; obj->day = day; obj->hour = hour; obj->minute = min; obj->second = second; obj->utc = UTC;
    }
    
    /*─······································································─*/
    
    void set_year( uint year )   const noexcept { obj->year   = year;   }
    void set_month( uint month ) const noexcept { obj->month  = month;  }
    void set_second( uint sec )  const noexcept { obj->second = sec;    }
    void set_minute( uint min )  const noexcept { obj->minute = min;    }
    void set_hour( uint hour )   const noexcept { obj->hour   = hour;   }
    void set_day( uint day )     const noexcept { obj->day    = day;    }

    /*─······································································─*/

    string_t get_fulltime() const noexcept { 
        char res [UNBFF_SIZE]; auto size = EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
            }
            stringToUTF8( data, $7, $8 ); return date.length;
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second, res, UNBFF_SIZE ); return { res, size };
    }
    
    uint get_year() const noexcept { 
        return EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
                return data.getUTCFullYear(); 
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
                return data.getFullYear(); 
            }
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second );
    }
    
    uint get_month() const noexcept {
        return EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
                return data.getUTCMonth(); 
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
                return data.getMonth(); 
            }
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second );
    }

    uint get_hour() const noexcept { 
        return EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
                return data.getUTCHours(); 
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
                return data.getHours(); 
            }
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second );
    }
    
    uint get_day() const noexcept {
        return EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
                return data.getUTCDay(); 
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
                return data.getDay(); 
            }
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second );
    }

    uint get_minute() const noexcept {
        return EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
                return data.getUTCMinutes(); 
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
                return data.getMinutes(); 
            }
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second );
    }
    
    uint get_second() const noexcept {
        return EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
                return data.getUTCSeconds(); 
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
                return data.getSeconds(); 
            }
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second );
    }

    ulong get_stamp() const noexcept { 
        char raw  [UNBFF_SIZE]; 
        auto size = EM_ASM_INT({ 
            let data = new Date();
            if( $0 ){
                data.setUTCDate($1);
                data.setUTCYear($2);
                data.setUTCHour($3);
                data.setUTCMonth($4);
                data.setUTCMinutes($5);
                data.setUTCSeconds($6);
            } else {
                data.setDate($1);
                data.setYear($2);
                data.setHour($3);
                data.setMonth($4);
                data.setMinutes($5);
                data.setSeconds($6);
            }
            let info = data.getTime().toString(); stringToUTF8( data, $7, $8 ); return info.length;
        }, obj->utc, obj->day, obj->year, obj->hour, obj->month, obj->minute, obj->second, raw, UNBFF_SIZE ); return string::to_ullong( string_t( raw, size ) );
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/