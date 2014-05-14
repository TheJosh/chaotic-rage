----
----  Test scripting features & gametype concepts
----


bind_gamestart(function(slot)

	add_timer(1000, function()
		show_alert_message("Test 1 - increase x by 1")
		
		local vec = physics.Vector(10.0, 15.0, 20.0)
		show_alert_message("Initial: " .. vec.x .. " " .. vec.y .. " " .. vec.z)
		
		vec.x = vec.x + 1
		show_alert_message("Updated: " .. vec.x .. " " .. vec.y .. " " .. vec.z)
	end);
	
	add_timer(2000, function()
		show_alert_message("Test 2 - add vec(1,2,3)")
		
		local vec = physics.Vector(10.0, 15.0, 20.0)
		show_alert_message("Initial: " .. vec.x .. " " .. vec.y .. " " .. vec.z)
		
		vec = vec + physics.Vector(1.0, 2.0, 3.0)
		show_alert_message("Updated: " .. vec.x .. " " .. vec.y .. " " .. vec.z)
	end);
	
	add_timer(3000, function()
		show_alert_message("Test 3 - sub vec(1,2,3)")
		
		local vec = physics.Vector(10.0, 15.0, 20.0)
		show_alert_message("Initial: " .. vec.x .. " " .. vec.y .. " " .. vec.z)
		
		vec = vec - physics.Vector(1.0, 2.0, 3.0)
		show_alert_message("Updated: " .. vec.x .. " " .. vec.y .. " " .. vec.z)
	end);
	
end);

