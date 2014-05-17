----
----  Test scripting features & gametype concepts
----


bind_gamestart(function(slot)
	
	show_alert_message("testing 1,2,3")
	
	ui.basicKeyPress("h", function()
		show_alert_message("cooooool")
	end)
	
end);

