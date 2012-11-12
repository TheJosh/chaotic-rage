-- trivial protocol example
-- declare our protocol
trivial_proto = Proto("chaoticrage", "Chaotic Rage")

-- create a function to dissect it
function trivial_proto.dissector(buffer,pinfo,tree)
	pinfo.cols.protocol = "CR"
	
	if buffer(2,2):uint() == 0 then
		pinfo.cols.info = "Type = Server; seq = " .. buffer(0,2):uint()
	else
		pinfo.cols.info = "Type = Client " .. buffer(2,2):uint() .. "; ack seq = " .. buffer(0,2):uint()
	end
	
	local subtree = tree:add(trivial_proto, buffer(), "Chaotic Rage Protocol Data")
	subtree:add(buffer(0,2), "Seq: " .. buffer(0,2):uint())
	subtree:add(buffer(2,2), "Code: " .. buffer(2,2):uint())
	local messages = subtree:add(buffer(4), "Messages")
	
	local p = 4
	local buflen = buffer:len()
	
	while p < buflen do
		local t = buffer(p,1):uint();
		local len = 1
		local msg = ""
		
		
		if t == 0x0B then
			-- Unit add
			local sub = messages:add(buffer(p, 31), "UNIT_ADD")
			p = p + 1
			
			sub:add(buffer(p + 0, 2), "slot: " .. buffer(p + 0, 2):uint());
			sub:add(buffer(p + 2, 4), "q x: " .. buffer(p + 2, 4):float());
			sub:add(buffer(p + 6, 4), "q y: " .. buffer(p + 6, 4):float());
			sub:add(buffer(p + 10, 4), "q z: " .. buffer(p + 10, 4):float());
			sub:add(buffer(p + 14, 4), "q w: " .. buffer(p + 14, 4):float());
			sub:add(buffer(p + 18, 4), "b x: " .. buffer(p + 18, 4):float());
			sub:add(buffer(p + 22, 4), "b y: " .. buffer(p + 22, 4):float());
			sub:add(buffer(p + 26, 4), "b z: " .. buffer(p + 26, 4):float());
			
			p = p + 30
			
		elseif t == 0x0C then
			-- Unit update
			local sub = messages:add(buffer(p, 31), "UNIT_UPDATE")
			p = p + 1
			
			sub:add(buffer(p + 0, 2), "slot: " .. buffer(p + 0, 2):uint());
			sub:add(buffer(p + 2, 4), "q x: " .. buffer(p + 2, 4):float());
			sub:add(buffer(p + 6, 4), "q y: " .. buffer(p + 6, 4):float());
			sub:add(buffer(p + 10, 4), "q z: " .. buffer(p + 10, 4):float());
			sub:add(buffer(p + 14, 4), "q w: " .. buffer(p + 14, 4):float());
			sub:add(buffer(p + 18, 4), "b x: " .. buffer(p + 18, 4):float());
			sub:add(buffer(p + 22, 4), "b y: " .. buffer(p + 22, 4):float());
			sub:add(buffer(p + 26, 4), "b z: " .. buffer(p + 26, 4):float());
			
			p = p + 30
			
		elseif t == 0x0A then
			-- Client state
			local sub = messages:add(buffer(p, 7), "CLIENT_STATE")
			p = p + 1
			
			sub:add(buffer(p + 0, 2), "x: ");
			sub:add(buffer(p + 2, 2), "y: ");
			sub:add(buffer(p + 4, 2), "delta: ");
			sub:add(buffer(p + 6, 1), "keys: ");
			
			p = p + 6
			
		elseif t == 0x04 then
			-- Join acceptance
			local sub = messages:add(buffer(p, 3), "JOIN_OKAY")
			p = p + 1
			
			sub:add(buffer(p + 0, 2), "slot: " .. buffer(p + 0, 2):uint());
			
			p = p + 2
			
		elseif t == 0x0D then
			-- Unit removed
			local sub = messages:add(buffer(p, 3), "UNIT_REM")
			p = p + 1
			
			sub:add(buffer(p + 0, 2), "slot: " .. buffer(p + 0, 2):uint());
			
			p = p + 2
			
			
		else
			-- Simple messages, no sub-dissection
			if t == 0x01 then
				msg = "INFO_REQ"
			elseif t == 0x02 then
				msg = "INFO_RESP"
			elseif t == 0x03 then
				msg = "JOIN_REQ"
			elseif t == 0x05 then
				msg = "JOIN_DENY"
			elseif t == 0x06 then
				msg = "JOIN_ACK"
			elseif t == 0x07 then
				msg = "JOIN_DONE"
			elseif t == 0x08 then
				msg = "CHAT_REQ"
			elseif t == 0x09 then
				msg = "CHAT_RESP"
			elseif t == 0x0E then
				msg = "WALL_ADD"
			elseif t == 0x0F then
				msg = "WALL_REM"
			elseif t == 0x12 then
				msg = "PLAYER_DROP"
			elseif t == 0x13 then
				msg = "QUIT_REQ"
			elseif t == 0x14 then
				msg = "PLAYER_QUIT"
			end
			
			messages:add(buffer(p, len), msg)
			p = p + len
		end
	end
end

-- load the udp.port table, register our protocol
udp_table = DissectorTable.get("udp.port")
udp_table:add(17778,trivial_proto)
