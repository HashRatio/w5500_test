import hashlib
import binascii
import string
prev_hash = "4d16b6f85af6e2198f44ae2a6de67f78487ae5611b77c6c0440b921e00000000"
coinbase1 = "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff20020862062f503253482f04b8864e5008"
coinbase2 = "072f736c7573682f000000000100f2052a010000001976a914d23fcdf86f7e756a64a7a9688ef9903327048ed988ac00000000"
merkle_branch = []
version = "00000002"
nbits = "1c2ac4af"
ntime = "504e86b9"
extra_nonce1 = "55555555"
extra_nonce2 = "aaaaaaaa"
coinbase = coinbase1 + extra_nonce1 + extra_nonce2 + coinbase2

merkle_branch = [
"1111111111111111111111111111111111111111111111111111111111111111",
"2222222222222222222222222222222222222222222222222222222222222222",
"3333333333333333333333333333333333333333333333333333333333333333",
"4444444444444444444444444444444444444444444444444444444444444444",
"5555555555555555555555555555555555555555555555555555555555555555",
]

#print len(coinbase1)
#print coinbase,len(coinbase)
coinbase_hash_bin = hashlib.sha256(hashlib.sha256(binascii.unhexlify(coinbase)).digest()).digest()
#print binascii.hexlify(coinbase_hash_bin)
def build_merkle_root(merkle_branch, coinbase_hash_bin):
    merkle_root = coinbase_hash_bin
    for h in merkle_branch:
        merkle_root = hashlib.sha256(hashlib.sha256(merkle_root + binascii.unhexlify(h)).digest()).digest()
    return binascii.hexlify(merkle_root)
	
print build_merkle_root(merkle_branch,coinbase_hash_bin)

