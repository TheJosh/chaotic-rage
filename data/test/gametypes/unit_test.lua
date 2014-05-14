----
----  Test scripting features & gametype concepts
----

testid = 0
numsuccess = 0
numfailure = 0
asserts = 0
box = nil


function test(name, func)
	asserts = 0
	testid = testid + 1
	
	func()
	
	if asserts == 0 then
		box:append("Test " .. testid .. " " .. name .. " success")
		numsuccess = numsuccess + 1
	else
		box:append("Test " .. testid .. " " .. name .. " failure")
		numfailure = numfailure + 1
	end
end

function throwAssert(msg)
	box:append("Assert: " .. msg)
	asserts = asserts + 1
end

function assertEquals(expect, actual) 
	if expect ~= actual then
		box:append(actual .. " does not match expected " .. expect)
	end
end



bind_gamestart(function(slot)
	box = ui.addDialogTextBox("Unit test results")
	box.text = "Running tests"
	box:append("")

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
	
	box:append("")
	box:append("Completed " .. testid .. " tests")
	box:append("Success " .. numsuccess)
	box:append("Failure " .. numfailure)
end);
