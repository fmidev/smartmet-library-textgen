--
-- FrostTest.lua
--
-- Runs same test cases as 'textgen/test/FrostStoryTest.cpp' but in Lua.
--

require "TestTools"

local wa= require "WeatherAnalysis"

local formatters= {
    fi= wa.Formatter("fi")
    sv= wa.Formatter("sv")
    en= wa.Formatter("en")
}

local fs= require "FrostStories"
    --
    -- functions that give the stories


--
-- = TEST( paragraph_ud, { fi=expected_str, sv=..., en=... } )
--
local function TEST( para, ok_tbl )

    for lang,ok in pairs(ok_tbl) do
        local s= formatters[lang]( para )
        if s==ok then
            TEST_OK()
        else
            TEST_FAIL()
        end
    end
end


--
local function frost_mean()

    local area= wa.Area("25,60")
	local t1= os.time{ year=2000, month=1, day=1 }
	local t2= os.time{ year=2000, month=1, day=2 }

    local some= wa.SOME( t1, area, wa.Period(t1,t2), "mean" )

    some.options {    
        precision= 10,
        frost_limit= 20,
        severe_frost_limit= 10,
        frostseason= true,
    }

    some.options {
        fake_mean= "0,0",
        fake_severe_mean= "0,0"
    }
    TEST( fs.mean(some), { fi="", sv="", en="" } )

    some.options {
        fake_mean= "10,0",
        fake_severe_mean= "0,0"
    }
    TEST( fs.mean(some), { fi="", sv="", en="" } )

    some.options {
        fake_mean= "20,0",
        fake_severe_mean= "0,0"
    }
    TEST( fs.mean(some), { fi="Hallan todennäköisyys on 20%.", 
                           sv="Sannolikheten för nattfrost är 20%.",
                           en="Probability of frost is 20%." } )
end


--
local function frost_maximum()

    local area= wa.Area("25,60")
	local t1= os.time{ year=2000, month=1, day=1 }
	local t2= os.time{ year=2000, month=1, day=2 }

    local some= wa.SOME( t1, area, wa.Period(t1,t2), "maximum" )

    some.options {    
        precision= 10,
        frost_limit= 20,
        severe_frost_limit= 10,
        frostseason= true,
    }

    some.options {
        fake_maximum= "0,0",
        fake_severe_maximum= "0,0"
    }
    TEST( fs.maximum(some), { fi="", sv="", en="" } )

    some.options {
        fake_maximum= "10,0",
        fake_severe_maximum= "0,0"
    }
    TEST( fs.maximum(some), { fi="", sv="", en="" } )

    some.options {
        fake_maximum= "20,0",
        fake_severe_maximum= "0,0"
    }
    TEST( fs.mean(some), { fi="Hallan todennäköisyys on 20%.", 
                           sv="Sannolikheten för nattfrost är 20%.",
                           en="Probability of frost is 20%." } )

    some.options {
        fake_maximum= "20,0",
        fake_severe_maximum= "10,0"
    }
    TEST( fs.mean(some), { fi="Ankaran hallan todennäköisyys on 10%.", 
                           sv="Sannolikheten för sträng nattfrost är 10%.",
                           en="Probability of severe frost is 10%." } )
end

--
-- ... rest of tests left out, since these are not meaningful unless taken from
--     ACTUAL HISTORIC DATA. The C++ tests (s.a. 'FrostStoryTest.cpp') only tests
--     string creation, not the full data path!     --AKa 26-Jun-2009
--

--
-- Actual test
--
io.stderr:write( [[
FrostStory tests
================
]] )

frost_mean()
frost_maximum()
--...

-- done
