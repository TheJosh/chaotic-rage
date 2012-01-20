----
----  Lua script for the AI "zomb".
----


add_interval(10 * 1000, function()
	
	debug("10 secs...!");
	
end);



    function iter (a, i)
      i = i + 1
      local v = a[i]
      if v then
        return i, v
      end
    end
    
    function ipairs (a)
      return iter, a, 0
    end


units = visible_units();

for i,v in ipairs(units) do
	debug(i,v)
end

