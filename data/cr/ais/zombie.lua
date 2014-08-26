----
----  Lua script for the AI "zombie".
----


--
-- Table filtering routine
-- 
table.filter = function(tbl, func)
	local newtbl = {}
	for i,v in pairs(tbl) do
		if func(v) then
			newtbl[#newtbl + 1] = v
		end
	end
	return newtbl
end



--
-- Main function for doing stuff, gets run every 500ms
--
function target() 
	units = visible_units()
	if (#units == 0) then return end
	
	me = get_info()

	-- filters out units in the same faction
	-- TODO: this should be in a function (i.e. is_foe(unit) or is_friend(unit))
	units = table.filter(units, function(a)
		return (a.faction ~= me.faction)
	end);

	if (#units == 0) then return end

	table.sort(units, function(a,b)
		return a.dist < b.dist				-- sorts by distance
	end);
	
	-- direction of player
	dir = units[1].location - me.location
	move(dir)
	
	-- do melee attack
	if #dir < 2 then
		melee(dir)
	elseif #dir < 5 then
		fire()
	end
end


add_interval(500, target)
target()

