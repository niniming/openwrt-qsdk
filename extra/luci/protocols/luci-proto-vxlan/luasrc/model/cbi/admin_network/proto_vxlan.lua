-- Copyright 2022 Tony <i@tony.moe>
-- Licensed to the public under the Apache License 2.0.


local map, section, net = ...
local ifname = net:get_interface():name()
local peeraddr, ipaddr, port, vid, tunlink
local ttl, tos, mtu, rxcsum, txcsum


-- general ---------------------------------------------------------------------


peeraddr = section:taboption(
  "general",
  Value,
  "peeraddr",
  translate("Remote IPv4 address"),
  translate("The IPv4 address or the fully-qualified domain name " ..
            "of the remote end.")
)
peeraddr.datatype = "or(hostname,ip4addr)"


ipaddr = section:taboption(
  "general",
  Value,
  "ipaddr",
  translate("Local IPv4 address"),
  translate("The local IPv4 address over which the tunnel is " ..
            "created (optional).")
)
ipaddr.datatype = "ip4addr"
ipaddr.optional = true


port = section:taboption(
  "general",
  Value,
  "port",
  translate("Destination port")
)
port.datatype = "port"
port.placeholder = "4789"
port.optional = true


vid = section:taboption(
  "general",
  Value,
  "vid",
  translate("VXLAN network identifier"),
  translate("ID used to uniquely identify the VXLAN")
)
vid.datatype = "range(1, 16777216)"
vid.optional = true


tunlink = section:taboption(
  "general",
  Value,
  "tunlink",
  translate("Bind interface"),
  translate("Bind the tunnel to this interface (optional).")
)
tunlink.template = "cbi/network_netlist"
tunlink.nocreate = true
tunlink.unspecified = true
tunlink.optional = true


-- advanced --------------------------------------------------------------------

ttl = section:taboption(
  "advanced",
  Value,
  "ttl",
  translate("Override TTL"),
  translate("Specify a TTL (Time to Live) for the encapsulating " ..
            "packet other than the default (64).")
)
ttl.datatype = "min(1)"
ttl.placeholder = "64"
ttl.optional = true


tos = section:taboption(
  "advanced",
  Value,
  "tos",
  translate("Override TOS"),
  translate("Specify a TOS (Type of Service).")
)
tos.datatype = "range(0, 255)"
tos.optional = true


mtu = section:taboption(
  "advanced",
  Value,
  "mtu",
  translate("Override MTU"),
  translate("Optional. Maximum Transmission Unit of tunnel interface.")
)
mtu.datatype = "range(1280,1420)"
mtu.placeholder = "1420"
mtu.optional = true


rxcsum = section:taboption(
  "advanced",
  Flag,
  "rxcsum",
  translate("Enable rx checksum")
)
rxcsum.enabled  = "1"
rxcsum.disabled = "0"
rxcsum.default  = rxcsum.enabled
rxcsum.rmempty  = false


txcsum = section:taboption(
  "advanced",
  Flag,
  "txcsum",
  translate("Enable tx checksum")
)
txcsum.enabled  = "1"
txcsum.disabled = "0"
txcsum.default  = txcsum.enabled
txcsum.rmempty  = false
