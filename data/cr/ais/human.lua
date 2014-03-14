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
	units = visible_units()
	if (#units == 0) then
		dir = vector3(random(-1.0,1.0), random(-1.0,1.0), random(-1.0,1.0))
		move(dir)
		return
	end
	
	me = get_info()

	-- sorts by distance
	table.sort(units, function(a,b)
		return a.dist < b.dist
	end);
	
	-- direction of player
	dir = units[1].location - me.location
	
	-- move, or if too far away, run randomly
	if (#dir > 100) then
		dir = vector3(random(-1.0,1.0), random(-1.0,1.0), random(-1.0,1.0))
	else 
		move(dir)
	end
	
	-- do melee attack
	if (#dir < 10) then
		begin_firing()
	else
		end_firing()
	end
end


add_interval(2 * 1000, target)
target()
