"""
Semester project Fall 2022
@author Matteo Oldani
@version 0.1
@date 2023-01-18

"""
from Crypto.Cipher import AES

key = b'YELLOW SUBMARINE'
ptxt = b'AAAAAAAAAAAAAAAA'

cipher = AES.new(key, AES.MODE_ECB)
ctxt = cipher.encrypt(ptxt)

print(ctxt)
for b in ctxt:
	print(b, end = " ")
print()
