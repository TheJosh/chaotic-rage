----
----  Map building tool
----

map_config = ""


-- Mouse pick handler for adding vehicles
function pickVehicle(x, y, z)
	local types = {"tank", "ute"}

	typeselect = ui.addDialogButtonBar("Vehicle", types,
		function(typeid)
			game.addVehicleXZ(types[typeid], x, z)
			map_config = map_config .. string.format("vehicle {  type = %q  x = %.2f  y = %.2f  }\n", types[typeid], x, z)
			
			add_timer(100, function()
				typeselect:close()
			end)
		end
	)
end

-- Mouse pick handler for adding objects
function pickObject(x, y, z)
	local types = {
		"crate_china", "crate_military", "crate_sealed", "crate_steel", "crate_wood", "crate_zombie",
		"war_fence_metal",
		"concrete_pipe_straight", "concrete_pipe_bend", "concrete_pipe_joiner", "concrete_pipe_tee", "concrete_pipe_channel",
		"rock_01",
		"table", "trashbox"
	}

	typeselect = ui.addDialogButtonBar("Object", types,
		function(typeid)
			game.addObjectXZ(types[typeid], x, z)
			map_config = map_config .. string.format("object {  type = %q  x = %.2f  y = %.2f  }\n", types[typeid], x, z)
			
			add_timer(100, function()
				typeselect:close()
			end)
		end
	)
end

-- Mouse pick handler for adding weapons
function pickWeapon(x, y, z)
	local types = mod.getAllWeaponTypes()
	local keys = {}
	local vals = {}
	for k,v in pairs(types) do
		table.insert(keys, k)
		table.insert(vals, v)
	end

	typeselect = ui.addDialogButtonBar("Weapon", vals,
		function(typeid)
			game.addPickupXZ("weapon_" .. keys[typeid], x, z)
			map_config = map_config .. string.format("pickup {  type = %q  x = %.2f  y = %.2f  }\n", "weapon_" .. keys[typeid], x, z)
			
			add_timer(100, function()
				typeselect:close()
			end)
		end
	)
end

-- Mouse pick handler for adding pickups
function pickPickup(x, y, z)
	local types = {
		"ammo_current",
		"medbox",
		"double_time",
		"chaotic",
		"rage",
	}

	typeselect = ui.addDialogButtonBar("Pickup", types,
		function(typeid)
			game.addPickupXZ(types[typeid], x, z)
			map_config = map_config .. string.format("pickup {  type = %q  x = %.2f  y = %.2f  }\n", types[typeid], x, z)
			
			add_timer(100, function()
				typeselect:close()
			end)
		end
	)
end


-- Add a toolbar; bound to "m" key
function createToolbar()
	toolbar = ui.addDialogButtonBar("Toolbar", {
		"Add vehicle",
		"Add object",
		"Add weapon",
		"Add pickup",
		"Show config",
		"Close toolbar"
	}, function(btn)
		add_timer(100, function()
			toolbar:close()
		end)

		if btn == 1 then
			mouse_pick(pickVehicle)
		elseif btn == 2 then
			mouse_pick(pickObject)
		elseif btn == 3 then
			mouse_pick(pickWeapon)
		elseif btn == 4 then
			mouse_pick(pickPickup)
		elseif btn == 5 then
			local dialog = ui.addDialogTextBox("Map Config")
			dialog.text = map_config
		end
	end)
end


-- Add a player
bind_playerjoin(function(slot)
	game.addPlayer(get_selected_unittype(), factions.team1, slot)
end)

-- New game; bind M key
bind_gamestart(function()
	ui.basicKeyPress("m", createToolbar)
	weather.disableRandom()
end)
