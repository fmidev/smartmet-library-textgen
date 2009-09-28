--
-- Frost.lua
--
-- Returns a set of story functions, having to do with night frost.
--
-- Usage:
--      require "Frost"
--
-- Author:
--      AKa 25-Jun-2009: based on C++ 'Frost*.cpp' stories.
--

--
-- [paragraph_ud]= mean( some_ud )
--
function mean( some )

    local opt= some.options

    if not opt.frostseason then
        LOG "Frost season is not on"
        return
    end

    local precision= assert( opt.precision )
    local normal_limit= assert( opt.frost_limit )
    local severe_limit= assert( opt.severe_frost_limit )

    local frost= wa.analyze( some, Frost ??? )
	if not frost then
	   error "Frost is not available"
    end

	LOG( "Frost Mean(Maximum) is "..frost )

	-- Quick exit if the mean is zero
	--
	if frost==0 then
	   return
    end

    frost= to_precision( frost, precision )

	-- Severe frost
    --
    local severe_frost= wa.analyze( some, SevereFrost )
    assert(severe_frost)

	LOG( "SevereFrost Mean(Maximum) is "..severe_frost )

    severe_frost= to_precision( severe_frost, precision )

	if severe_frost >= opt.severe_limit then
	   return { "ankaran hallan todennäköisyys",
			    "on",
			    math.floor(severe_frost),
			    "%" }

    elseif frost >= opt.normal_limit then
	   return { "hallan todennäköisyys",
			    "on",
			    math.floor(frost),
			    "%" }
    end
end


-- TBD: ...jatka muilla stooreilla...

