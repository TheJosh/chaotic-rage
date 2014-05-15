----
----  Test scripting features & gametype concepts
----


bind_gamestart(function(slot)
	
	show_alert_message("testing 1,2,3")
	
	local buttonbar = ui.addDialogButtonBar("test", {"hello", "world", "yay"}, function(index)
		show_alert_message("you clicked on button " .. index)
	end)
	
	add_timer(5000, function()
		buttonbar.close()
	end)
	
	show_alert_message("cooooool")
	
end);

