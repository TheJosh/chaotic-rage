----
----  Test scripting features & gametype concepts
----


bind_playerjoin(function(slot)
	add_player(get_selected_unittype(), factions.team1, slot);

	add_timer(5000, function() 
		add_npc("zomb", "zombie", factions.team2);
	end);
end);


bind_playerdied(function(slot)
	add_timer(2000, function()
		add_player(get_selected_unittype(), factions.team1, slot);
	end);
end);



