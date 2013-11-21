from socket import *
import sys

s = socket(AF_INET, SOCK_DGRAM)
s.bind(('1.2.3.14', 0))

if sys.argv[1] == 'on':
    for _ in range(2):  s.sendto(bytes([8] * 16), ('1.2.3.4', 30000))
elif sys.argv[1] == 'off':
    for _ in range(2):  s.sendto(bytes([7] * 16), ('1.2.3.4', 30000))
elif sys.argv[1] == 'audio' and sys.argv[1] == 'off':
    for _ in range(2):  s.sendto(bytes([2] * 16), ('1.2.3.4', 30000))
elif sys.argv[1] == 'audio' and sys.argv[2] == 'on':
    for _ in range(2):  s.sendto(bytes([3] * 16), ('1.2.3.4', 30000))
elif sys.argv[1] == 'touch' and sys.argv[2] == 'off':
    for _ in range(2):  s.sendto(bytes([4] * 16), ('1.2.3.4', 30000))
elif sys.argv[1] == 'touch' and sys.argv[2] == 'on':
    for _ in range(2):  s.sendto(bytes([5] * 16), ('1.2.3.4', 30000))
elif sys.argv[1] == 'freq':
    freq = '0' * 6 + sys.argv[2]
    freq = freq[len(freq) - 6:]
    s.sendto(('\x06' * 8 + freq).encode('utf-8'), ('1.2.3.4', 30000))

