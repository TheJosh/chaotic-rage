----
----  Test scripting features & gametype concepts
----

testid = 0
numsuccess = 0
numfailure = 0
asserts = 0


function test(name, func)
	asserts = 0
	testid = testid + 1
	
	func()
	
	if asserts == 0 then
		show_alert_message("Test " .. testid .. " " .. name .. " - success")
		numsuccess = numsuccess + 1
	else
		show_alert_message("Test " .. testid .. " " .. name .. " - failure")
		numfailure = numfailure + 1
	end
end

function throwAssert(msg)
	show_alert_message(msg)
	asserts = asserts + 1
end

function assertEquals(expect, actual) 
	if expect ~= actual then
		throwAssert(actual .. " does not match expected " .. expect)
	end
end



bind_gamestart(function(slot)
	
	test("vec create", function()
		local vec = physics.Vector(10.0, 15.0, 20.0)
		assertEquals(10.0, vec.x)
		assertEquals(15.0, vec.y)
		assertEquals(20.0, vec.z)
	end)
	
	test("vec component modify", function()
		local vec = physics.Vector(10.0, 15.0, 20.0)
		vec.x = vec.x + 1
		vec.y = vec.y - 1
		vec.z = vec.z + 0.5
		assertEquals(11.0, vec.x)
		assertEquals(14.0, vec.y)
		assertEquals(20.5, vec.z)
	end)
	
	test("vec add", function()
		local vec = physics.Vector(10.0, 15.0, 20.0)
		vec = vec + physics.Vector(1.0, -1.0, 0.5)
		assertEquals(11.0, vec.x)
		assertEquals(14.0, vec.y)
		assertEquals(20.5, vec.z)
	end)
	
	test("vec sub", function()
		local vec = physics.Vector(10.0, 15.0, 20.0)
		vec = vec - physics.Vector(1.0, -1.0, 0.5)
		assertEquals(9.0, vec.x)
		assertEquals(16.0, vec.y)
		assertEquals(19.5, vec.z)
	end)
	
	
	show_alert_message("Success: " .. numsuccess)
	show_alert_message("Failure: " .. numfailure)
end);
