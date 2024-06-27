import sys
from impacket.smbconnection import SMBConnection

ip = sys.argv[1]

smb = SMBConnection(ip, ip, sess_port=445)
print("SMB signing is: %s" % smb.isSigningRequired())
