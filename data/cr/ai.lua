----
----  Lua script for the AI "zomb".
----


add_interval(10 * 1000, function()
	
	debug("10 secs...!");
	
end);



a=vector3(10,20,30);
debug(a.x, a.y, a.z);

a.x,a.y,a.z = 100,200,300;
debug(a.x, a.y, a.z);

