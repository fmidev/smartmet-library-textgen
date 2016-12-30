--
-- FireWarningTest.lua
--
-- Runs same test cases as 'textgen/test/FireWarningTest.cpp' but in Lua.
--

require "TestTools"

local m= require "FireWarning"
assert(m)

-- { [1..46]= "None"|"GrassFireWarning"|"FireWarning", ... } = read( dir_str, os.time{ year=yyyy, month=mm, day=dd } )
--
local FireWarnings= assert( m.read )

local DIR= "."     -- in current dir

--
local function constructors() 

    -- 24.5.2005 has data
    --
    EXPECT_OK( function()
        local w= FireWarnings(DIR, os.time{ year=2005, month=5, day=24 })
        assert(w[10])
    end )

    -- Next day finds previous day data
	--
	EXPECT_OK( function()
	   local w= FireWarnings(DIR,os.time{ year=2005, month=5, day=25 })
	   assert(w[10])
    end )

    -- Following day must fail
    --
    EXPECT_ERROR_RE( function()
        FireWarnings(DIR,os.time{ year=2005, month=5, day=26 })
    end,
        "^.+No such file or directory$" )

    -- And the day before 24.05.2005
	--
    EXPECT_ERROR_RE( function()
        FireWarnings(DIR,os.time{ year=2005, month=5, day=23 })
    end,
        "^.+No such file or directory$" )

	TEST_PASSED()
end


--
local function state()

    local date= os.time{ year=2005, month=5, day=24 }
    local warnings= FireWarnings(DIR,date)

	assert( warnings[1] == "FireWarning" )
	assert( warnings[2] == "None" )
    assert( warnings[3] == "FireWarning" )
    assert( warnings[32] == "GrassFireWarning" )

    assert( warnings[0] == nil )
    assert( warnings[100] == nil )

	TEST_PASSED()
end

--
-- Actual test
--
io.stderr:write( [[
FireWarnings tests
==================
]] )

constructors()
state()

-- done

