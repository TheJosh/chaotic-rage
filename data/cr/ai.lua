----
----  Lua script for the AI "zomb".
----


add_interval(10 * 1000, function()
	
	print("10 secs...!");
	
end);

add_interval(10 * 1000, function() 

	units = visible_units();

	print("Found " .. #units .. " units")

	for i, v in ipairs(units) do
		print(i, v.dist)
	end

end);