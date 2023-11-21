import matplotlib.pyplot as plt
from scapy.all import *

def get_window(packet,bool):
    # print(packet)
    if(bool):
        if packet.haslayer('TCP'):
            return packet['TCP'].window
        else:
            return 0
    else:
        if packet.haslayer('TCP') and packet['TCP'].flags=='A':
            return packet['TCP'].window
        else:
            return 0
    

def get_win(packet,bool):
    # print(packet)
    if(bool):
        if packet.haslayer('TCP') and packet['IP'].src=="10.0.0.1":
            return packet['TCP'].window
        else:
            return 0
    else:
        if packet.haslayer('TCP') and packet['IP'].src=="10.0.0.6":
            return packet['TCP'].window
        else:
            return 0
    
def plot_congestion_window(pcap_file):
    packets = rdpcap(pcap_file)
    print('Number of packets: ', len(packets))

    # Extract TCP sequence numbers
    win= [get_window(packet,1) for packet in packets]

    plt.figure(figsize=(16, 10))
    plt.subplot(1, 2, 1)
    plt.plot(win,color='red',label='A')
    plt.xlabel('Packet Number')
    plt.ylabel('TCP Window Size')
    plt.title('Congestion Window Plot using window size')

    win= [get_window(packet,0) for packet in packets]
    plt.subplot(1, 2, 2)
    plt.plot(win,color='blue',label='A')
    plt.xlabel('Packet Number')
    plt.ylabel('TCP Window Size')
    plt.title('Congestion Window Plot using window size')
    plt.show()


pcap_file = 'check2.pcap'  
plot_congestion_window(pcap_file)

