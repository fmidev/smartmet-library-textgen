--
-- TestTools.lua
--
-- Sets some global functions to help testing
--

--
-- Local store of things that have failed in the _last_ (ongoing) test
--
local fails= {}   -- { { reason=str, error=str }, ... }

--
-- t= ADD_LINEINFO(t)
--
-- Add location of who called 'EXPECT_...' function to the failure table
--
local function ADD_LINEINFO(t)

    local dt= debug.getinfo(3,"lS")   -- function who called the function who called us

    t.where= (dt.source ..":".. dt.currentline )
    return t
end

--
-- = EXPECT_OK( func )
--
-- Run the function and expect it NOT to fail (call 'error' or 'assert')
--
function EXPECT_OK( func )
    local ok,err= pcall( func )
    if not ok then
        fails[#fails+1]= ADD_LINEINFO{ reason="expected success", error=err }
    end
end

--
-- = EXPECT_ERROR_RE( func, regex_str )
--
function EXPECT_ERROR_RE( func, expected_re )
    assert( expected_re )

    local ok,err= pcall( func )
    if ok then
        fails[#fails+1]= ADD_LINEINFO{ reason= "expected failure but succeeded" }
    elseif not err:match(expected_re) then
        fails[#fails+1]= ADD_LINEINFO{ reason= "wrong error message, expected to match '"..expected_re.."'", error=err }
    end
end

--
-- = TEST_PASSED( [title_str] )
--
-- Default title is the name of the function, who called us.
--
function TEST_PASSED( title )
    if not title then
        local dt= debug.getinfo(2,"n")   -- 2 = function who called us
        title= dt.name or "(unnamed)"
    end

    io.stdout:write( title..(string.rep( '.', 52-#title )) )
    
    if not fails[1] then
        print "passed"
    else
        print "FAILED"
        for _,t in ipairs(fails) do
            print( "\treason:\t"..(t.reason).." ("..(t.where)..")"..
                   "\n\terror:\t"..(t.error or "nil") )
        end
    end
end
