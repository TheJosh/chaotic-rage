----
----  Lua script for the AI "zomb".
----


function target() 
	
	units = visible_units();
	if (#units == 0) then return end
	
	table.sort(units, function(a,b)
		return a.dist > b.dist				-- finds the furthest unit, so we can actually see it working
	end);
	
	me = get_location()
	dir = units[1].location - me
	
	move(dir)
	
end


add_interval(2 * 1000, target)
target();
