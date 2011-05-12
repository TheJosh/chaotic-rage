--
-- Lua script for the gametype "Zombies".
--

debug('Hello world from Lua');

bind_gamestart(function()
	debug('Adding an NPC');
	add_npc();
	debug('NPC added!');
end);



