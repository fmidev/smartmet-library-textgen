--
-- Implementation of 'textgen/sources/FireWarnings.cpp' in Lua
--

-- Firewarning areas run from 1 to 46
--
local MAX_AREACODE= 46

-- Time difference of an hour
--
local H= 60*60     -- seconds

-- Format for 'YYYYMMDD' (see 'man strftime')
--
local YYYYMMDD_FMT= "%Y%m%d"

do  -- selftest
    local t= os.time( { year=2009, month=1, day=2 } )
    assert( os.date( YYYYMMDD_FMT, t ) == "20090102" )
end


--
-- { [areacode_int]= warning_str, ... }= read( data_dir_str, date_time )
--
-- data_dir_str: Directory where "*.palot_koodina" files are. No slash at the end
--               (though Posix systems will not complain)
-- date_time:    Lua time value (see 'os.time', essentially the same as C 'time_t');
--               only the date part of the time is important.
--
local function read( data_dir, date )

    if data_dir=="" then
        data_dir= "."
    end

    local fn= data_dir.."/".. os.date(YYYYMMDD_FMT,date) ..".palot_koodina"

    local f,err= io.open( fn, "r" )
    if not f then
        -- If that file is not there, try yesterday's
        --
        f= io.open( os.date(YYYYMMDD_FMT,date-24*H) ..".palot_koodina", "r" )
        if not f then
            error( "Cannot read '"..fn.."': "..err )
        end
    end

    -- Read the whole file
    --
    -- Format:
    --      YYYYMMDDHHMM
    --      <areacode> <state>
    --      ...
    --
    --      areacode: 1..46 (integers; unordered)
    --      state: 0..2 (integers)
--[[
200505241122
20 2
2 0
4 2
1 2
3 2
5 0
...
]]
    --
    local s= f:read'*a'

    local lookup= {
        [0]= "None",
        [1]= "GrassFireWarning",
        [2]= "FireWarning"
    }

    local ret= {}
        -- [areacode_int] -> warning_str

    for area,st in s:gmatch("\n(%d+) (%d+)") do
        area,st= tonumber(area), tonumber(st)    -- 'gmatch' returns them as strings

        if (area<1) or (area>MAX_AREACODE) then
            error( "Invalid areacode: "..area )
        elseif not lookup[st] then
            error( "Invalid warning code: "..st )
        elseif ret[area] then
            error( "Areacode "..area.." duplicated" )
        end
        
        ret[area]= lookup[st]
    end
    
    return ret
end


-- Return a namespace to a file requiring us.
--
-- Usage:
--      local m= require "FireWarning"
--      assert( m.read )
--      local t= m.read( "", os.time{ year=2009, month=1, day=2 } )
--      print( t[8] )   --> string
--
return {
    read= read
}

