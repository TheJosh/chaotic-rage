-- trivial protocol example
-- declare our protocol
trivial_proto = Proto("chaoticrage", "Chaotic Rage")

-- create a function to dissect it
function trivial_proto.dissector(buffer,pinfo,tree)
	pinfo.cols.protocol = "CR"
	
	local subtree = tree:add(trivial_proto, buffer(), "Chaotic Rage Protocol Data")
	subtree:add(buffer(0,2), "Seq: " .. buffer(0,2):uint())
	subtree:add(buffer(2,2), "Code: " .. buffer(2,2):uint())
	subtree:add(buffer(4), "Message Data")
end

-- load the udp.port table, register our protocol
udp_table = DissectorTable.get("udp.port")
udp_table:add(17778,trivial_proto)
