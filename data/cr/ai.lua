----
----  Lua script for the AI "zomb".
----


add_interval(10 * 1000, function()
	
	debug("10 secs...!");
	
end);


debug("---  vector tests  ---");

a=vector3(10,20,30);
debug("A:    ", a.x, a.y, a.z);

b=vector3(1,2,3);
debug("B:    ", b.x, b.y, b.z);

c = a + b;
debug("A+B:  ", c.x, c.y, c.z);

c = a - b;
debug("A-B:  ", c.x, c.y, c.z);

c = a * 5.0;
debug("A*5:  ", c.x, c.y, c.z);

c = a / 5.0;
debug("A/5:  ", c.x, c.y, c.z);

c = -a;
debug("-A:   ", c.x, c.y, c.z);

c = #a;
debug("#A:   ", c);

a=vector3(10,20,30);
b=vector3(10,20,30);
if (a == b) then
	debug("A==B:   Okay");
else
	debug("A==B:   Fail");
end;

a=vector3(10,20,30);
b=vector3(1,2,3);
if (a ~= b) then
	debug("A~=B:   Okay");
else
	debug("A~=B:   Fail");
end;

debug("----------------------");

a=vector3(10,20,30);
c=v3normalize(a);
debug("v3normalize:   ", c.x, c.y, c.z);
c=v3normalise(a);
debug("v3normalise:   ", c.x, c.y, c.z);

a=vector3(2,0,0);
b=vector3(0,2,0);
c=v3cross(a,b);
debug("v3cross:       ", c.x, c.y, c.z);

a=vector3(2,0,0);
b=vector3(1,3,5);
c=v3dot(a,b);
debug("v3dot:         ", c);

debug("----------------------");

