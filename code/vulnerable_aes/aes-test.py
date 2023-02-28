from Crypto.Cipher import AES

key = b'YELLOW SUBMARINE'
ptxt = b'AAAAAAAAAAAAAAAA'

cipher = AES.new(key, AES.MODE_ECB)
ctxt = cipher.encrypt(ptxt)

print(ctxt)
for b in ctxt:
	print(b, end = " ")
print()
