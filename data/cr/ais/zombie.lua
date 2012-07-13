----
----  Lua script for the AI "zomb".
----


--
-- Table filtering routine
-- 
table.filter = function(tbl, func)
	local newtbl = {}
	for i,v in pairs(tbl) do
		if func(v) then
			newtbl[i] = v
		end
	end
	return newtbl
end



--
-- Main function for doing stuff, gets run every 2 seconds
--
function target() 
	units = visible_units();
	if (#units == 0) then return end
	
	me = get_info()

	units = table.filter(units, function(a)
		return (a.faction ~= me.faction)			-- filters out units in the same faction
	end);

	if (#units == 0) then return end

	table.sort(units, function(a,b)
		return a.dist < b.dist				-- sorts by distance
	end);
	
	-- move in that direction
	dir = units[1].location - me.location
	move(dir)
	
	-- do melee attack
	if (#dir < 5) then
		melee();
	end
end


add_interval(2 * 1000, target)
target();
