-- trivial protocol example
-- declare our protocol
trivial_proto = Proto("chaoticrage", "Chaotic Rage")

-- create a function to dissect it
function trivial_proto.dissector(buffer,pinfo,tree)
	if buffer(2,2):uint() == 0 then
		pinfo.cols.protocol = "CR:S"
	else
		pinfo.cols.protocol = "CR:C"
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
		
		if t == 0x01 then
			msg = "INFO_REQ"
			
		elseif t == 0x02 then
			msg = "INFO_RESP"
			
		elseif t == 0x03 then
			msg = "JOIN_REQ"
			
		elseif t == 0x04 then
			msg = "JOIN_OKAY"
			len = len + 2
			
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
			
		elseif t == 0x0A then
			msg = "CLIENT_STATE"
			len = len + 7
			
		elseif t == 0x0B then
			msg = "UNIT_ADD"
			len = len + 30
			
		elseif t == 0x0C then
			msg = "UNIT_UPDATE"
			len = len + 30
			
		elseif t == 0x0D then
			msg = "UNIT_REM"
			len = len + 2
			
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

-- load the udp.port table, register our protocol
udp_table = DissectorTable.get("udp.port")
udp_table:add(17778,trivial_proto)
